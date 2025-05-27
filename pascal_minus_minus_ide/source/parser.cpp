#include "parser.h"
#include "lexer.h"
using PascalToken::TokenType;
#include <iostream>
using namespace std;

// Конструктор парсера
Parser::Parser(const vector<Token>& tokens) : tokens(tokens), pos(0) {}

const Token& Parser::current() const {
    if (pos >= tokens.size())
        throw runtime_error("Неожиданный конец входных данных");
    return tokens[pos];
}

bool Parser::match(TokenType type) {
    if (current().type == type) {
        pos++;
        return true;
    }
    return false;
}

void Parser::expect(TokenType type, const string& errorMsg) {
    if (!match(type)) {
        int errLine = current().line;
        int errCol = current().column;
        throw runtime_error(errorMsg + " в " + to_string(errLine) + " строчке, " + to_string(errCol) + " позиции");
    }
}

shared_ptr<ASTNode> Parser::parse() {
    return parseProgram();
}

shared_ptr<ASTNode> Parser::parseProgram() {
    expect(TokenType::Program, "Ожидалось ключевое слово 'program'");
    auto prog = make_shared<ASTNode>(ASTNodeType::Program);
    expect(TokenType::Identifier, "Ожидалось имя программы");
    expect(TokenType::Semicolon, "Ожидалась точка с запятой после имени программы");
    prog->children.push_back(parseBlock());
    expect(TokenType::Dot, "Ожидалась точка в конце программы");
    return prog;
}

shared_ptr<ASTNode> Parser::parseBlock() {
    auto block = make_shared<ASTNode>(ASTNodeType::Block);
    if (current().type == TokenType::Const)
        block->children.push_back(parseConstSection());
    if (current().type == TokenType::Var)
        block->children.push_back(parseVarSection());
    expect(TokenType::Begin, "Ожидалось 'begin'");
    while (current().type != TokenType::End &&
        current().type != TokenType::Dot &&
        current().type != TokenType::EndOfFile) {
        auto stmt = parseStatement();
        if (stmt != nullptr) {
            block->children.push_back(stmt);
            cout << "Добавлен оператор типа " << static_cast<int>(stmt->type) << endl;
        }
        match(TokenType::Semicolon);
    }
    expect(TokenType::End, "Ожидалось 'end'");
    return block;
}

shared_ptr<ASTNode> Parser::parseStatement() {
    cout << ">> parseStatement: " << current().value << " (" << static_cast<int>(current().type) << ")" << endl;
    if (current().type == TokenType::End ||
        current().type == TokenType::Dot ||
        current().type == TokenType::EndOfFile) {
        return nullptr;
    }
    if (current().type == TokenType::Begin)
        return parseBlock();
    if (current().type == TokenType::Writeln)
        return parseWriteln();
    if (current().type == TokenType::Write)
        return parseWrite();
    if (current().type == TokenType::Readln)
        return parseReadln();
    if (current().type == TokenType::Read)
        return parseRead();
    if (current().type == TokenType::If)
        return parseIf();
    if (current().type == TokenType::While)
        return parseWhile();
    if (current().type == TokenType::For)
        return parseFor();
    if (current().type == TokenType::Identifier) {
        size_t savedPos = pos;
        string id = current().value;
        pos++;
        if (pos < tokens.size() && tokens[pos].type == TokenType::LBracket) {
            size_t bracketPos = pos;
            int depth = 1;
            pos++;
            while (pos < tokens.size() && depth > 0) {
                if (tokens[pos].type == TokenType::LBracket) depth++;
                else if (tokens[pos].type == TokenType::RBracket) depth--;
                pos++;
            }
            if (depth == 0 && pos < tokens.size() && tokens[pos].type == TokenType::Assign) {
                pos = savedPos;
                return parseAssignment();
            }
            pos = savedPos;
        }
        if (pos < tokens.size() && tokens[pos].type == TokenType::Assign) {
            pos = savedPos;
            return parseAssignment();
        }
        if (pos < tokens.size() && tokens[pos].type == TokenType::LParen) {
            pos = savedPos;
            return parseProcedureCall();
        }
        throw runtime_error("Ожидалось ':=' или '(' или '[...]' после идентификатора '" + id + "'");
    }
    cerr << "Неизвестный токен в parseStatement: type=" << static_cast<int>(current().type) << " value=" << current().value << endl;
    throw runtime_error("Неизвестный оператор в " + to_string(current().line) + " строчке");
}





// Разбор секции констант
shared_ptr<ASTNode> Parser::parseConstSection() {
    expect(TokenType::Const, "Ожидалось 'const'");
    auto section = make_shared<ASTNode>(ASTNodeType::ConstSection);
    while (current().type == TokenType::Identifier) {
        string name = current().value;
        expect(TokenType::Identifier, "Ожидался идентификатор");
        string typeName;
        if (match(TokenType::Colon)) {
            expect(TokenType::Identifier, "Ожидался тип константы");
            typeName = tokens[pos - 1].value;
        }
        else
            throw runtime_error("Нужен тип для константы " + name);
        expect(TokenType::Equal, "Ожидался '='");
        auto value = parseExpression();
        expect(TokenType::Semicolon, "Ожидалась ';'");
        auto decl = make_shared<ASTNode>(ASTNodeType::ConstDecl, name);
        decl->children.push_back(make_shared<ASTNode>(ASTNodeType::Identifier, typeName));
        decl->children.push_back(value);
        section->children.push_back(decl);
    }
    return section;
}

// Разбор секции переменных: var x, y: integer;
shared_ptr<ASTNode> Parser::parseVarSection() {
    expect(TokenType::Var, "Ожидалось 'var'");
    auto section = make_shared<ASTNode>(ASTNodeType::VarSection);
    while (current().type == TokenType::Identifier) {
        // Собираем имена переменных через запятую
        vector<string> names;
        names.push_back(current().value);
        expect(TokenType::Identifier, "Ожидался идентификатор");
        while (match(TokenType::Comma)) {
            expect(TokenType::Identifier, "Ожидался идентификатор после запятой");
            names.push_back(tokens[pos - 1].value);
        }
        expect(TokenType::Colon, "Ожидалось ':' после списка имён");
        string typeName;
        switch (current().type) {
        case TokenType::Array: {
    expect(TokenType::Array, "Ожидалось 'array'");
    expect(TokenType::LBracket, "Ожидалось '[' после 'array'");
    auto lower = parseExpression();
    expect(TokenType::Dot, "Ожидалось '..'");
    auto upper = parseExpression();
    expect(TokenType::RBracket, "Ожидалось ']'");

    expect(TokenType::Of, "Ожидалось 'of'");
    string elemType;
    switch (current().type) {
        case TokenType::Integer:
        case TokenType::Real:
        case TokenType::Boolean:
        case TokenType::StringType:
            elemType = current().value;
            pos++;
            break;
        default:
            throw runtime_error("Ожидался базовый тип массива");
    }

    typeName = "array"; // логическая метка
    for (const auto& name : names) {
        auto decl = make_shared<ASTNode>(ASTNodeType::ArrayDecl, name);
        decl->children.push_back(lower);
        decl->children.push_back(upper);
        decl->children.push_back(make_shared<ASTNode>(ASTNodeType::Identifier, elemType));
        section->children.push_back(decl);
    }

    expect(TokenType::Semicolon, "Ожидалась ';'");
    continue; // обрабатываем следующий идентификатор
}

        case TokenType::Identifier:
        case TokenType::Integer:
        case TokenType::Real:
        case TokenType::Boolean:
        case TokenType::StringType:
            typeName = current().value;
            pos++;  // съели токен типа
            break;
        default:
            throw runtime_error("Ожидался тип переменной в " + to_string(current().line) + " строчке, " + to_string(current().column) + " позиции");
        }
        expect(TokenType::Semicolon, "Ожидалась ';' после объявления переменных");
        // Для всех имён создаём отдельные VarDecl с общим типом
        for (const auto& name : names) {
            auto decl = make_shared<ASTNode>(ASTNodeType::VarDecl, name);
            auto typeNode = make_shared<ASTNode>(ASTNodeType::Identifier, typeName);
            decl->children.push_back(typeNode);
            section->children.push_back(decl);
        }
    }
    return section;
}



// Разбор for
shared_ptr<ASTNode> Parser::parseFor() {
    expect(TokenType::For, "Ожидалось 'for'");
    string varName = current().value;
    expect(TokenType::Identifier, "Ожидался идентификатор переменной цикла");
    expect(TokenType::Assign, "Ожидалось ':='");
    auto fromExpr = parseExpression();

    bool isDownto = false;
    if (match(TokenType::To)) {
        isDownto = false;
    }
    else if (match(TokenType::Downto)) {
        isDownto = true;
    }
    else {
        throw runtime_error("Ожидалось 'to' или 'downto'");
    }

    auto toExpr = parseExpression();
    expect(TokenType::Do, "Ожидалось 'do'");
    auto body = parseStatement();

    auto forNode = make_shared<ASTNode>(ASTNodeType::ForLoop, varName);
    forNode->children.push_back(fromExpr);
    forNode->children.push_back(toExpr);
    forNode->children.push_back(body);
    if (isDownto)
        forNode->value += "|downto";
    return forNode;
}

// Разбор присваивания: <id> := <выражение>
shared_ptr<ASTNode> Parser::parseAssignment() {
    auto assign = make_shared<ASTNode>(ASTNodeType::Assignment);
    if (current().type != TokenType::Identifier)
        throw runtime_error("Ожидался идентификатор в левой части присваивания");

    string name = current().value;
    pos++;

    if (match(TokenType::LBracket)) {
        auto indexExpr = parseExpression();
        expect(TokenType::RBracket, "Ожидалась ']'");
        auto access = make_shared<ASTNode>(ASTNodeType::ArrayAccess, name);
        access->children.push_back(indexExpr);
        assign->children.push_back(access);
    }
    else {
        assign->children.push_back(make_shared<ASTNode>(ASTNodeType::Identifier, name));
    }

    expect(TokenType::Assign, "Ожидался ':='");
    assign->children.push_back(parseExpression());
    return assign;
}

// Разбор вызова процедуры или функции: <id>(...)
shared_ptr<ASTNode> Parser::parseProcedureCall() {
    string procName = current().value;
    expect(TokenType::Identifier, "Ожидался идентификатор процедуры");
    expect(TokenType::LParen, "Ожидалась '(' после имени процедуры");
    auto node = make_shared<ASTNode>(ASTNodeType::ProcCall, procName);
    // Парсим аргументы (может быть несколько через запятую)
    if (current().type != TokenType::RParen) {
        node->children.push_back(parseExpression());
        while (match(TokenType::Comma))
            node->children.push_back(parseExpression());
    }
    expect(TokenType::RParen, "Ожидалась ')' после аргументов процедуры");
    return node;
}

// Разбор условного оператора if ... then ... [else ...]
shared_ptr<ASTNode> Parser::parseIf() {
    expect(TokenType::If, "Ожидалось 'if'");
    auto node = make_shared<ASTNode>(ASTNodeType::If);
    node->children.push_back(parseExpression());
    expect(TokenType::Then, "Ожидалось 'then'");
    node->children.push_back(parseStatement());
    if (match(TokenType::Else))
        node->children.push_back(parseStatement());
    return node;
}

// Разбор цикла while ... do ...
shared_ptr<ASTNode> Parser::parseWhile() {
    expect(TokenType::While, "Ожидалось 'while'");
    auto node = make_shared<ASTNode>(ASTNodeType::While);
    node->children.push_back(parseExpression());
    expect(TokenType::Do, "Ожидалось 'do'");
    node->children.push_back(parseStatement());
    return node;
}

// Разбор оператора write(...)
shared_ptr<ASTNode> Parser::parseWrite() {
    expect(TokenType::Write, "Ожидалось 'Write'");
    auto node = make_shared<ASTNode>(ASTNodeType::Write);
    expect(TokenType::LParen, "Ожидалась '(' после Write");
    if (current().type != TokenType::RParen) {
        node->children.push_back(parseExpression());
        while (match(TokenType::Comma))
            node->children.push_back(parseExpression());
    }
    expect(TokenType::RParen, "Ожидалась ')' после Write");
    return node;
}

// Разбор оператора read(...)
shared_ptr<ASTNode> Parser::parseRead() {
    expect(TokenType::Read, "Ожидалось 'read'");
    auto node = make_shared<ASTNode>(ASTNodeType::Read);
    expect(TokenType::LParen, "Ожидалась '(' после read");
    node->children.push_back(parseExpression());
    expect(TokenType::RParen, "Ожидалась ')' после read");
    return node;
}

// Разбор оператора writeln(...)
shared_ptr<ASTNode> Parser::parseWriteln() {
    expect(TokenType::Writeln, "Ожидалось 'Writeln'");
    auto node = make_shared<ASTNode>(ASTNodeType::Writeln);
    expect(TokenType::LParen, "Ожидалась '(' после 'Writeln'");
    // Поддержка zero или более аргументов
    if (current().type != TokenType::RParen) {
        node->children.push_back(parseExpression());
        while (match(TokenType::Comma))
            node->children.push_back(parseExpression());
    }
    expect(TokenType::RParen, "Ожидалась ')' после аргументов 'Writeln'");
    return node;
}

// Разбор оператора readln(...)
shared_ptr<ASTNode> Parser::parseReadln() {
    expect(TokenType::Readln, "Ожидалось 'readln'");
    auto node = make_shared<ASTNode>(ASTNodeType::Readln);
    expect(TokenType::LParen, "Ожидалась '(' после 'readln'");
    // Поддержка одного и более аргументов (чтобы знать, куда читать)
    if (current().type != TokenType::RParen) {
        // обычно readln(x, y, ...)
        node->children.push_back(parseExpression());
        while (match(TokenType::Comma))
            node->children.push_back(parseExpression());
    }
    expect(TokenType::RParen, "Ожидалась ')' после аргументов 'readln'");
    return node;
}

// Разбор выражения с поддержкой логических и сравнительных операций
shared_ptr<ASTNode> Parser::parseExpression() {
    auto left = parseSimpleExpression();
    if (current().type == TokenType::Equal || current().type == TokenType::NotEqual ||
        current().type == TokenType::Less || current().type == TokenType::LessEqual ||
        current().type == TokenType::Greater || current().type == TokenType::GreaterEqual) {
        auto op = current(); pos++;
        auto right = parseSimpleExpression();
        auto bin = make_shared<ASTNode>(ASTNodeType::BinOp, op.value);
        bin->children = { left, right };
        left = bin;
    }
    return left;
}

// Разбор простого выражения: сложение, вычитание, or
shared_ptr<ASTNode> Parser::parseSimpleExpression() {
    auto left = parseTerm();
    while (current().type == TokenType::Plus || current().type == TokenType::Minus ||
        current().type == TokenType::Or) {
        auto op = current(); pos++;
        auto right = parseTerm();
        auto bin = make_shared<ASTNode>(ASTNodeType::BinOp, op.value);
        bin->children = { left, right };
        left = bin;
    }
    return left;
}

// Разбор терма: умножение, деление, and
shared_ptr<ASTNode> Parser::parseTerm() {
    auto left = parseFactor();
    while (current().type == TokenType::Multiply || current().type == TokenType::Divide ||
        current().type == TokenType::And || current().type == TokenType::DivKeyword || current().type == TokenType::Mod) {
        auto op = current(); pos++;
        auto right = parseFactor();
        auto bin = make_shared<ASTNode>(ASTNodeType::BinOp, op.value);
        bin->children = { left, right };
        left = bin;
    }
    return left;
}

// Разбор множителя: числа, идентификаторы, строковые литералы, унарные операторы, скобки
shared_ptr<ASTNode> Parser::parseFactor() {
    if (match(TokenType::LParen)) {
        auto expr = parseExpression();
        expect(TokenType::RParen, "Ожидалась ')'");
        return expr;
    }
    if (current().type == TokenType::RealLiteral) {
        auto node = make_shared<ASTNode>(ASTNodeType::Real, current().value);
        pos++;
        return node;
    }
    if (current().type == TokenType::Number) {
        auto node = make_shared<ASTNode>(ASTNodeType::Number, current().value);
        pos++;
        return node;
    }
    if (current().type == TokenType::True || current().type == TokenType::False) {
        string val = (current().type == TokenType::True) ? "true" : "false";
        auto node = make_shared<ASTNode>(ASTNodeType::Boolean, val);
        pos++;
        return node;
    }
    if (current().type == TokenType::StringLiteral) {
        auto node = make_shared<ASTNode>(ASTNodeType::String, current().value);
        pos++;
        return node;
    }
    if (current().type == TokenType::Identifier) {
        string name = current().value;
        pos++;
        if (match(TokenType::LBracket)) {
            auto index = parseExpression();
            expect(TokenType::RBracket, "Ожидалась ']'");

            auto access = make_shared<ASTNode>(ASTNodeType::ArrayAccess, name);
            access->children.push_back(index);
            return access;
        }
        return make_shared<ASTNode>(ASTNodeType::Identifier, name);
    }
    if (match(TokenType::Minus)) {
        auto node = make_shared<ASTNode>(ASTNodeType::UnOp, "-");
        node->children.push_back(parseFactor());
        return node;
    }
    if (match(TokenType::Not)) {
        auto node = make_shared<ASTNode>(ASTNodeType::UnOp, "not");
        node->children.push_back(parseFactor());
        return node;
    }
    int errLine = current().line;
    int errCol = current().column;
    throw runtime_error("Ожидалось число, идентификатор или выражение в скобках в " + to_string(errLine) + " строчке, " + to_string(errCol) + " позиции");
}