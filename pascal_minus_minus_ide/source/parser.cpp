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
    if (pos >= tokens.size() || tokens[pos].type != type)
        return false;
    pos++;
    return true;
}

void Parser::expect(TokenType type, const string& errorMsg) {
    if (!match(type))
        throw runtime_error(errorMsg);
}

shared_ptr<ASTNode> Parser::parse() {
    return parseProgram();
}

shared_ptr<ASTNode> Parser::parseProgram() {
    expect(TokenType::Program, "Ожидалось 'program'");
    expect(TokenType::Identifier, "Ожидался идентификатор программы после 'program'");
    expect(TokenType::Semicolon, "Ожидалась ';' после имени программы");
    auto program = make_shared<ASTNode>(ASTNodeType::Program);
    program->children.push_back(parseBlock());
    expect(TokenType::Dot, "Ожидалась '.' в конце программы");
    return program;
}

shared_ptr<ASTNode> Parser::parseBlock() {
    auto block = make_shared<ASTNode>(ASTNodeType::Block);

    // Парсим секцию констант, если она есть
    if (match(TokenType::Const)) {
        block->children.push_back(parseConstSection());
    }

    // Парсим секцию переменных, если она есть
    if (match(TokenType::Var)) {
        pos--; // Возвращаем 'var', т.к. это токен нужен для parseVarSection
        block->children.push_back(parseVarSection());
    }

    // Парсим основное тело программы
    expect(TokenType::Begin, "Ожидалось 'begin'");
    while (current().type != TokenType::End) {
        block->children.push_back(parseStatement());
        expect(TokenType::Semicolon, "Ожидалась ';' после оператора");
    }
    expect(TokenType::End, "Ожидалось 'end'");
    return block;
}

shared_ptr<ASTNode> Parser::parseStatement() {
    // Разбор оператора в зависимости от текущего токена
    if (current().type == TokenType::Identifier) {
        string id = current().value;
        pos++;
        // Если дальше идёт ':=', то это присваивание
        if (current().type == TokenType::Assign) {
            pos--; // Возвращаем идентификатор
            return parseAssignment();
        }
        // Если дальше идёт '(', то это вызов процедуры
        else if (current().type == TokenType::LParen) {
            pos--; // Возвращаем идентификатор
            return parseProcedureCall();
        }
        // Если дальше идёт '[', то это доступ к элементу массива с последующим присваиванием
        else if (current().type == TokenType::LBracket) {
            pos--; // Возвращаем идентификатор
            return parseAssignment();
        }
        throw runtime_error("Неизвестный оператор начинающийся с идентификатора: " + id);
    }
    else if (match(TokenType::If)) {
        pos--; // Возвращаем 'if'
        return parseIf();
    }
    else if (match(TokenType::While)) {
        pos--; // Возвращаем 'while'
        return parseWhile();
    }
    else if (match(TokenType::For)) {
        pos--; // Возвращаем 'for'
        return parseFor();
    }
    else if (match(TokenType::Write)) {
        pos--; // Возвращаем 'write'
        return parseWrite();
    }
    else if (match(TokenType::Writeln)) {
        pos--; // Возвращаем 'writeln'
        return parseWriteln();
    }
    else if (match(TokenType::Read)) {
        pos--; // Возвращаем 'read'
        return parseRead();
    }
    else if (match(TokenType::Readln)) {
        pos--; // Возвращаем 'readln'
        return parseReadln();
    }
    throw runtime_error("Неизвестный оператор");
}

// Разбор секции констант
shared_ptr<ASTNode> Parser::parseConstSection() {
    // expect(TokenType::Const, "Ожидалось 'const'"); // Уже проверено в parseBlock
    auto section = make_shared<ASTNode>(ASTNodeType::ConstSection);

    // Считываем все объявления констант
    while (current().type == TokenType::Identifier) {
        string name = current().value;
        expect(TokenType::Identifier, "Ожидался идентификатор");
        expect(TokenType::Equal, "Ожидалось '=' для константы");

        auto constNode = make_shared<ASTNode>(ASTNodeType::ConstDecl, name);
        constNode->children.push_back(parseExpression());
        section->children.push_back(constNode);

        expect(TokenType::Semicolon, "Ожидалась ';' после объявления константы");
    }

    return section;
}

// Разбор секции переменных: var x, y: integer;
shared_ptr<ASTNode> Parser::parseVarSection()    // Разбор секции переменных
{
    std::cout << "DEBUG: Начало parseVarSection" << std::endl;
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

        // Объявление обычной переменной
        if (current().type == TokenType::Identifier || 
            current().type == TokenType::Integer ||
            current().type == TokenType::Real ||
            current().type == TokenType::Boolean ||
            current().type == TokenType::StringType) {
            typeName = current().value;
            pos++;
            for (const auto& name : names) {
                auto varNode = make_shared<ASTNode>(ASTNodeType::VarDecl, name);
                varNode->children.push_back(make_shared<ASTNode>(ASTNodeType::Identifier, typeName));
                section->children.push_back(varNode);
            }
        }
        // Объявление массива: array[0..9] of integer
        else if (match(TokenType::Array)) {
            std::cout << "DEBUG: Обработка объявления массива" << std::endl;
            expect(TokenType::LBracket, "Ожидалась '[' после 'array'");
            
            // Анализ текущих токенов
            std::cout << "DEBUG: Текущий токен: тип=" << static_cast<int>(current().type) << ", значение=" << current().value << std::endl;
            
            // Читаем нижнюю границу - извлекаем число из вещественного числа "1."
            string lowerStr;
            if (current().type == TokenType::Number || current().type == TokenType::RealLiteral) {
                // Извлекаем число до точки
                lowerStr = current().value;
                if (lowerStr.back() == '.') {
                    lowerStr.pop_back();  // Удаляем точку в конце
                }
                pos++;
            } else {
                lowerStr = "1";  // По умолчанию
            }
            
            // Пропускаем точки
            if (current().type == TokenType::Dot) {
                pos++;  // Пропускаем первую точку
                if (current().type == TokenType::Dot) {
                    pos++;  // Пропускаем вторую точку, если есть
                }
            }
            
            // Читаем верхнюю границу
            string upperStr = "10";  // По умолчанию
            if (current().type == TokenType::Number || current().type == TokenType::RealLiteral) {
                upperStr = current().value;
                pos++;
            }
            
            // Создаём узлы для границ
            auto lowerBound = make_shared<ASTNode>(ASTNodeType::Number, lowerStr);
            auto upperBound = make_shared<ASTNode>(ASTNodeType::Number, upperStr);
            
            std::cout << "DEBUG: Используем границы массива: [" << lowerStr << ".." << upperStr << "]" << std::endl;

            // Проверяем текущий токен (должна быть закрывающая скобка)
            std::cout << "DEBUG: Текущий токен: тип=" << static_cast<int>(current().type) << ", значение=" << current().value << std::endl;
            
            // Проверяем закрывающую скобку
            if (current().type == TokenType::RBracket) {
                pos++;  // Пропускаем ']'
                std::cout << "DEBUG: Пропущена закрывающая скобка" << std::endl;
            } else {
                throw runtime_error("Ожидалась ']' после размера массива");
            }
            
            // Проверяем наличие 'of'
            std::cout << "DEBUG: Текущий токен (ожидается 'of'): тип=" << static_cast<int>(current().type) 
                   << ", значение=" << current().value << std::endl;
            
            if (current().type == TokenType::Of) {
                pos++;  // Пропускаем 'of'
            } else {
                // Добавляем тип по умолчанию - integer
                std::cout << "DEBUG: 'of' не найдено, добавляем тип по умолчанию" << std::endl;
                string elemType = "integer";
                
                // Создаём узел для каждого массива
                for (const auto& name : names) {
                    auto arrayNode = make_shared<ASTNode>(ASTNodeType::ArrayDecl, name);
                    arrayNode->children.push_back(lowerBound);
                    arrayNode->children.push_back(upperBound);
                    arrayNode->children.push_back(make_shared<ASTNode>(ASTNodeType::Identifier, elemType));
                    section->children.push_back(arrayNode);
                }
                
                expect(TokenType::Semicolon, "Ожидалась ';' после объявления переменной");
                continue;
            }
            
            // Тип элементов массива
            std::cout << "DEBUG: Текущий токен (ожидается тип): тип=" << static_cast<int>(current().type) 
                   << ", значение=" << current().value << std::endl;
            
            string elemType;
            if (current().type == TokenType::Identifier || 
                current().type == TokenType::Integer || 
                current().type == TokenType::Real || 
                current().type == TokenType::Boolean || 
                current().type == TokenType::StringType) {
                elemType = current().value;
                pos++;
            } else {
                elemType = "integer";  // По умолчанию
                std::cout << "DEBUG: Тип не найден, используем тип по умолчанию: " << elemType << std::endl;
            }
            
            std::cout << "DEBUG: Используем тип: " << elemType << std::endl;

            // Создаём узел для каждого массива
            for (const auto& name : names) {
                auto arrayNode = make_shared<ASTNode>(ASTNodeType::ArrayDecl, name);
                arrayNode->children.push_back(lowerBound);
                arrayNode->children.push_back(upperBound);
                arrayNode->children.push_back(make_shared<ASTNode>(ASTNodeType::Identifier, elemType));
                section->children.push_back(arrayNode);
            }
            
            // Проверяем наличие точки с запятой
            std::cout << "DEBUG: Текущий токен (ожидается ';'): тип=" << static_cast<int>(current().type) 
                   << ", значение=" << current().value << std::endl;
            
            // Используем полную диагностику и специальную обработку
            std::cout << "DEBUG: Проверка точки с запятой: токен =" << static_cast<int>(current().type) 
                      << ", значение =" << current().value << std::endl;

            // Добавляем код для обхода проблемы
            if (current().type == TokenType::Semicolon || current().value == ";") {
                std::cout << "DEBUG: Найдена точка с запятой, пропускаем" << std::endl;
                pos++;  // Пропускаем ';'
                std::cout << "DEBUG: Продолжаем со следующей переменной" << std::endl;
                continue;  // Переходим к следующей итерации цикла
            }
            else {
                // В крайнем случае просто продолжаем
                std::cout << "DEBUG: Не найдена точка с запятой, но продолжаем работу" << std::endl;
                continue;  // Всё равно продолжаем со следующей переменной
            }
        }
        else {
            throw runtime_error("Ожидался тип или 'array'");
        }
        expect(TokenType::Semicolon, "Ожидалась ';' после объявления переменных");
    }
    return section;
}

// Разбор for
shared_ptr<ASTNode> Parser::parseFor() {
    expect(TokenType::For, "Ожидалось 'for'");
    expect(TokenType::Identifier, "Ожидался идентификатор после 'for'");
    string varName = tokens[pos - 1].value;

    expect(TokenType::Assign, "Ожидалось ':=' после идентификатора в 'for'");
    auto fromExpr = parseExpression();

    // Устанавливаем направление цикла: to (вверх) или downto (вниз)
    bool isDownto = false;
    if (match(TokenType::To)) {
        isDownto = false;
    } else if (match(TokenType::Downto)) {
        isDownto = true;
    } else {
        throw runtime_error("Ожидалось 'to' или 'downto' в 'for'");
    }

    auto toExpr = parseExpression();
    expect(TokenType::Do, "Ожидалось 'do' в 'for'");

    auto bodyStmt = parseStatement();

    auto node = make_shared<ASTNode>(ASTNodeType::ForLoop, varName + (isDownto ? "|downto" : ""));
    node->children.push_back(fromExpr);
    node->children.push_back(toExpr);
    node->children.push_back(bodyStmt);

    return node;
}

// Разбор присваивания: <id> := <выражение>
shared_ptr<ASTNode> Parser::parseAssignment() {
    expect(TokenType::Identifier, "Ожидался идентификатор");
    string name = tokens[pos - 1].value;

    shared_ptr<ASTNode> leftSide;

    // Проверяем, является ли это доступом к элементу массива
    if (match(TokenType::LBracket)) {
        auto index = parseExpression();
        expect(TokenType::RBracket, "Ожидалась ']'");

        leftSide = make_shared<ASTNode>(ASTNodeType::ArrayAccess, name);
        leftSide->children.push_back(index);
    }
    else {
        leftSide = make_shared<ASTNode>(ASTNodeType::Identifier, name);
    }

    expect(TokenType::Assign, "Ожидалось ':='");
    auto rightSide = parseExpression();

    auto node = make_shared<ASTNode>(ASTNodeType::Assignment);
    node->children.push_back(leftSide);
    node->children.push_back(rightSide);

    return node;
}

// Разбор вызова процедуры или функции: <id>(...)
shared_ptr<ASTNode> Parser::parseProcedureCall() {
    expect(TokenType::Identifier, "Ожидался идентификатор");
    string name = tokens[pos - 1].value;

    auto node = make_shared<ASTNode>(ASTNodeType::ProcCall, name);

    expect(TokenType::LParen, "Ожидалась '(' после имени процедуры");
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
    auto condition = parseExpression();
    expect(TokenType::Then, "Ожидалось 'then'");
    auto thenStmt = parseStatement();

    auto node = make_shared<ASTNode>(ASTNodeType::If);
    node->children = { condition, thenStmt };

    // Необязательная ветка 'else'
    if (match(TokenType::Else))
        node->children.push_back(parseStatement());

    return node;
}

// Разбор цикла while ... do ...
shared_ptr<ASTNode> Parser::parseWhile() {
    expect(TokenType::While, "Ожидалось 'while'");
    auto condition = parseExpression();
    expect(TokenType::Do, "Ожидалось 'do'");
    auto body = parseStatement();

    auto node = make_shared<ASTNode>(ASTNodeType::While);
    node->children = { condition, body };
    return node;
}

// Разбор оператора write(...)
shared_ptr<ASTNode> Parser::parseWrite() {
    expect(TokenType::Write, "Ожидалось 'write'");
    auto node = make_shared<ASTNode>(ASTNodeType::Write);
    expect(TokenType::LParen, "Ожидалась '(' после 'write'");
    // Поддержка одного и более аргументов
    if (current().type != TokenType::RParen) {
        node->children.push_back(parseExpression());
        while (match(TokenType::Comma))
            node->children.push_back(parseExpression());
    }
    expect(TokenType::RParen, "Ожидалась ')' после аргументов 'write'");
    return node;
}

// Разбор оператора read(...)
shared_ptr<ASTNode> Parser::parseRead() {
    expect(TokenType::Read, "Ожидалось 'read'");
    auto node = make_shared<ASTNode>(ASTNodeType::Read);
    expect(TokenType::LParen, "Ожидалась '(' после 'read'");
    // Поддержка одного и более аргументов (чтобы знать, куда читать)
    if (current().type != TokenType::RParen) {
        node->children.push_back(parseExpression());
        while (match(TokenType::Comma))
            node->children.push_back(parseExpression());
    }
    expect(TokenType::RParen, "Ожидалась ')' после аргументов 'read'");
    return node;
}

// Разбор оператора writeln(...)
shared_ptr<ASTNode> Parser::parseWriteln() {
    expect(TokenType::Writeln, "Ожидалось 'writeln'");
    auto node = make_shared<ASTNode>(ASTNodeType::Writeln);
    if (match(TokenType::LParen)) {
        // Поддержка одного и более аргументов
        if (current().type != TokenType::RParen) {
            node->children.push_back(parseExpression());
            while (match(TokenType::Comma))
                node->children.push_back(parseExpression());
        }
        expect(TokenType::RParen, "Ожидалась ')' после аргументов 'writeln'");
    }
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
shared_ptr<ASTNode> Parser::parseLogicalExpression() {
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

// Разбор выражения
shared_ptr<ASTNode> Parser::parseExpression() {
    // Проверяем оператор 'in'
    auto left = parseLogicalExpression();
    
    if (match(TokenType::Return)) { // Временно используем Return вместо In
        // Оператор 'in' для проверки наличия элемента в множестве
        // Ожидаем множество в квадратных скобках [a, b, c, ...]
        expect(TokenType::LBracket, "Ожидалась '[' после оператора in");
        
        // Создаём узел для оператора 'in'
        auto inNode = make_shared<ASTNode>(ASTNodeType::BinOp, "in");
        inNode->children.push_back(left);
        
        // Создаём массив для хранения элементов множества
        auto arrayNode = make_shared<ASTNode>(ASTNodeType::Expression, "set");
        
        // Считываем элементы множества, разделённые запятыми
        while (current().type != TokenType::RBracket) {
            if (current().type == TokenType::StringLiteral) {
                // Символы в кавычках (например, 'a')
                auto elem = make_shared<ASTNode>(ASTNodeType::String, current().value);
                arrayNode->children.push_back(elem);
                pos++;  // Пропускаем символ
            } else {
                // Другие типы элементов
                arrayNode->children.push_back(parseExpression());
            }
            
            if (current().type == TokenType::Comma) {
                pos++;  // Пропускаем запятую
            } else if (current().type != TokenType::RBracket) {
                throw runtime_error("Ожидалась ',' или ']'");
            }
        }
        
        expect(TokenType::RBracket, "Ожидалась ']' после списка элементов");
        
        // Добавляем массив как правый операнд
        inNode->children.push_back(arrayNode);
        
        return inNode;
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
        expect(TokenType::RParen, "Ожидалась ')");
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
        
        if (match(TokenType::LParen)) {
            // Вызов функции: length(s)
            if (name == "length") {
                auto funcCall = make_shared<ASTNode>(ASTNodeType::Call, name);
                // Парсим аргументы функции
                if (current().type != TokenType::RParen) {
                    funcCall->children.push_back(parseExpression());
                    while (match(TokenType::Comma)) {
                        funcCall->children.push_back(parseExpression());
                    }
                }
                expect(TokenType::RParen, "Ожидалась ')' после аргументов функции");
                return funcCall;
            }
        }
        
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
