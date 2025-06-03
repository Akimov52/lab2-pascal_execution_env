#include "parser.h"
#include "lexer.h"
#include "error_reporter.h"
#include <iostream>
#include <stdexcept>

using PascalToken::TokenType;
using namespace std;

// ����������� �� ���������
Parser::Parser() : pos(0), errorReporter(std::make_shared<ErrorReporter>()) {}

// ����������� � ������������ ������
Parser::Parser(std::shared_ptr<IErrorReporter> errorReporter) 
    : pos(0), errorReporter(errorReporter ? errorReporter : std::make_shared<ErrorReporter>()) {}

// ����������� � �������� � ������������ ������
Parser::Parser(const vector<Token>& tokens, std::shared_ptr<IErrorReporter> errorReporter) 
    : tokens(tokens), pos(0), errorReporter(errorReporter ? errorReporter : std::make_shared<ErrorReporter>()) {}

const Token& Parser::current() const {
    if (pos >= tokens.size())
        throw runtime_error("����������� ����� ������� ������");
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
        throw runtime_error(errorMsg + " � " + to_string(errLine) + " �������, " + to_string(errCol) + " �������");
    }
}

shared_ptr<ASTNode> Parser::parse() {
    return parseProgram();
}

shared_ptr<ASTNode> Parser::parseProgram() {
    expect(TokenType::Program, "��������� �������� ����� 'program'");
    auto prog = make_shared<ASTNode>(ASTNodeType::Program);
    expect(TokenType::Identifier, "��������� ��� ���������");
    expect(TokenType::Semicolon, "��������� ����� � ������� ����� ����� ���������");
    prog->children.push_back(parseBlock());
    expect(TokenType::Dot, "��������� ����� � ����� ���������");
    return prog;
}

shared_ptr<ASTNode> Parser::parseBlock() {
    auto block = make_shared<ASTNode>(ASTNodeType::Block);
    if (current().type == TokenType::Const)
        block->children.push_back(parseConstSection());
    if (current().type == TokenType::Var)
        block->children.push_back(parseVarSection());
    expect(TokenType::Begin, "��������� 'begin'");
    while (current().type != TokenType::End &&
        current().type != TokenType::Dot &&
        current().type != TokenType::EndOfFile) {
        auto stmt = parseStatement();
        if (stmt != nullptr) {
            block->children.push_back(stmt);
        }
        match(TokenType::Semicolon);
    }
    expect(TokenType::End, "��������� 'end'");
    return block;
}

shared_ptr<ASTNode> Parser::parseStatement() {
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

        if (pos < tokens.size() && tokens[pos].type == TokenType::Assign) {
            pos = savedPos;
            return parseAssignment();
        }
        throw runtime_error("��������� ':=' ����� �������������� '" + id + "'");
    }
    throw runtime_error("����������� �������� � " + to_string(current().line) + " �������");
}

// ������ ������ ��������
shared_ptr<ASTNode> Parser::parseConstSection() {
    expect(TokenType::Const, "��������� 'const'");
    auto section = make_shared<ASTNode>(ASTNodeType::ConstSection);
    while (current().type == TokenType::Identifier) {
        string name = current().value;
        expect(TokenType::Identifier, "�������� �������������");
        string typeName;
        if (match(TokenType::Colon)) {
            expect(TokenType::Identifier, "�������� ��� ���������");
            typeName = tokens[pos - 1].value;
        }
        else
            throw runtime_error("����� ��� ��� ��������� " + name);
        expect(TokenType::Equal, "�������� '='");
        auto value = parseExpression();
        expect(TokenType::Semicolon, "��������� ';'");
        auto decl = make_shared<ASTNode>(ASTNodeType::ConstDecl, name);
        decl->children.push_back(make_shared<ASTNode>(ASTNodeType::Identifier, typeName));
        decl->children.push_back(value);
        section->children.push_back(decl);
    }
    return section;
}

// ������ ������ ����������: var x, y: integer;
shared_ptr<ASTNode> Parser::parseVarSection() {
    expect(TokenType::Var, "��������� 'var'");
    auto section = make_shared<ASTNode>(ASTNodeType::VarSection);
    while (current().type == TokenType::Identifier) {
        // �������� ����� ���������� ����� �������
        vector<string> names;
        names.push_back(current().value);
        expect(TokenType::Identifier, "�������� �������������");
        while (match(TokenType::Comma)) {
            expect(TokenType::Identifier, "�������� ������������� ����� �������");
            names.push_back(tokens[pos - 1].value);
        }
        expect(TokenType::Colon, "��������� ':' ����� ������ ���");
        string typeName;
        switch (current().type) {
        case TokenType::Identifier:
        case TokenType::Integer:
        case TokenType::Real:
        case TokenType::Boolean:
        case TokenType::StringType:
            typeName = current().value;
            pos++;  // ����� ����� ����
            break;
        default:
            throw runtime_error("�������� ��� ���������� � " + to_string(current().line) + " �������, " + to_string(current().column) + " �������");
        }
        expect(TokenType::Semicolon, "��������� ';' ����� ���������� ����������");
        // ��� ���� ��� ������ ��������� VarDecl � ����� �����
        for (const auto& name : names) {
            auto decl = make_shared<ASTNode>(ASTNodeType::VarDecl, name);
            auto typeNode = make_shared<ASTNode>(ASTNodeType::Identifier, typeName);
            decl->children.push_back(typeNode);
            section->children.push_back(decl);
        }
    }
    return section;
}

// ������ for
shared_ptr<ASTNode> Parser::parseFor() {
    expect(TokenType::For, "��������� 'for'");
    string varName = current().value;
    expect(TokenType::Identifier, "�������� ������������� ���������� �����");
    expect(TokenType::Assign, "��������� ':='");
    auto fromExpr = parseExpression();

    bool isDownto = false;
    if (match(TokenType::To)) {
        isDownto = false;
    }
    else if (match(TokenType::Downto)) {
        isDownto = true;
    }
    else {
        throw runtime_error("��������� 'to' ��� 'downto'");
    }

    auto toExpr = parseExpression();
    expect(TokenType::Do, "��������� 'do'");
    auto body = parseStatement();

    auto forNode = make_shared<ASTNode>(ASTNodeType::ForLoop, varName);
    forNode->children.push_back(fromExpr);
    forNode->children.push_back(toExpr);
    forNode->children.push_back(body);
    if (isDownto)
        forNode->value += "|downto";
    return forNode;
}

// ������ ������������: <id> := <���������>
shared_ptr<ASTNode> Parser::parseAssignment() {
    auto assign = make_shared<ASTNode>(ASTNodeType::Assignment);
    if (current().type != TokenType::Identifier)
        throw runtime_error("�������� ������������� � ����� ����� ������������");

    string name = current().value;
    pos++;

    {
        assign->children.push_back(make_shared<ASTNode>(ASTNodeType::Identifier, name));
    }

    expect(TokenType::Assign, "�������� ':='");
    assign->children.push_back(parseExpression());
    return assign;
}

// ������ ��������� ��������� if ... then ... [else ...]
shared_ptr<ASTNode> Parser::parseIf() {
    expect(TokenType::If, "��������� 'if'");
    auto node = make_shared<ASTNode>(ASTNodeType::If);
    node->children.push_back(parseExpression());
    expect(TokenType::Then, "��������� 'then'");
    node->children.push_back(parseStatement());
    if (match(TokenType::Else))
        node->children.push_back(parseStatement());
    return node;
}

// ������ ����� while ... do ...
shared_ptr<ASTNode> Parser::parseWhile() {
    expect(TokenType::While, "��������� 'while'");
    auto node = make_shared<ASTNode>(ASTNodeType::While);
    node->children.push_back(parseExpression());
    expect(TokenType::Do, "��������� 'do'");
    node->children.push_back(parseStatement());
    return node;
}

// ������ ��������� write(...)
shared_ptr<ASTNode> Parser::parseWrite() {
    expect(TokenType::Write, "��������� 'Write'");
    auto node = make_shared<ASTNode>(ASTNodeType::Write);
    expect(TokenType::LParen, "��������� '(' ����� Write");
    if (current().type != TokenType::RParen) {
        node->children.push_back(parseExpression());
        while (match(TokenType::Comma))
            node->children.push_back(parseExpression());
    }
    expect(TokenType::RParen, "��������� ')' ����� Write");
    return node;
}

// ������ ��������� read(...)
shared_ptr<ASTNode> Parser::parseRead() {
    expect(TokenType::Read, "��������� 'read'");
    auto node = make_shared<ASTNode>(ASTNodeType::Read);
    expect(TokenType::LParen, "��������� '(' ����� read");
    node->children.push_back(parseExpression());
    expect(TokenType::RParen, "��������� ')' ����� read");
    return node;
}

// ������ ��������� writeln(...)
shared_ptr<ASTNode> Parser::parseWriteln() {
    expect(TokenType::Writeln, "��������� 'Writeln'");
    auto node = make_shared<ASTNode>(ASTNodeType::Writeln);
    expect(TokenType::LParen, "��������� '(' ����� 'Writeln'");
    // ��������� zero ��� ����� ����������
    if (current().type != TokenType::RParen) {
        node->children.push_back(parseExpression());
        while (match(TokenType::Comma))
            node->children.push_back(parseExpression());
    }
    expect(TokenType::RParen, "��������� ')' ����� ���������� 'Writeln'");
    return node;
}

// ������ ��������� readln(...)
shared_ptr<ASTNode> Parser::parseReadln() {
    expect(TokenType::Readln, "��������� 'readln'");
    auto node = make_shared<ASTNode>(ASTNodeType::Readln);
    expect(TokenType::LParen, "��������� '(' ����� 'readln'");
    // ��������� ������ � ����� ���������� (����� �����, ���� ������)
    if (current().type != TokenType::RParen) {
        // ������ readln(x, y, ...)
        node->children.push_back(parseExpression());
        while (match(TokenType::Comma))
            node->children.push_back(parseExpression());
    }
    expect(TokenType::RParen, "��������� ')' ����� ���������� 'readln'");
    return node;
}

// ������ ��������� � ���������� ���������� � ������������� ��������
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

// ������ �������� ���������: ��������, ���������, or
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

// ������ �����: ���������, �������, and
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

// ������ ���������: �����, ��������������, ��������� ��������, ������� ���������, ������
shared_ptr<ASTNode> Parser::parseFactor() {
    if (match(TokenType::LParen)) {
        auto expr = parseExpression();
        expect(TokenType::RParen, "��������� ')'");
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
        // ���������������� �������� �������
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
    throw runtime_error("��������� �����, ������������� ��� ��������� � ������� � " + to_string(errLine) + " �������, " + to_string(errCol) + " �������");
}