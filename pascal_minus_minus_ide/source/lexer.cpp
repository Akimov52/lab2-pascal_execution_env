#include "lexer.h"
#include "iostream"
using namespace std;

// Конструктор по умолчанию для токена: инициализируем как EndOfFile с пустым значением
Token::Token() : type(TokenType::EndOfFile), value(""), line(0), column(0) {}

// Конструктор токена с параметрами: тип, значение, строка и позиция
Token::Token(TokenType t, const string& v, int l, int c) : type(t), value(v), line(l), column(c) {}

// Проверка: является ли символ буквенным или кириллическим (Windows-1251)
bool isAlphaCyrillic(unsigned char c) {
    // Латиница
    if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z'))
        return true;
    // Кириллица (Windows-1251: А-Я 0xC0-0xDF, а-я 0xE0-0xFF, Ё 0xA8, ё 0xB8)
    if ((c >= 0xC0 && c <= 0xDF) || (c >= 0xE0 && c <= 0xFF) || c == 0xA8 || c == 0xB8)
        return true;
    return false;
}

// Конструктор лексера: сохраняем исходный текст программы
Lexer::Lexer(const string& source) : source(source), position(0), line(1), column(1) {}

// Получение текущего символа
char Lexer::current() const {
    if (position >= source.length())
        return '\0';
    return source[position];
}

// Посмотреть следующий символ (без перемещения курсора)
char Lexer::peek() const {
    if (position + 1 >= source.length())
        return '\0';
    return source[position + 1];
}

// Перемещение курсора на один символ, обновляя строку и позицию
void Lexer::advance() {
    if (position < source.length()) {
        if (source[position] == '\n') {
            line++;
            column = 1;
        }
        else
            column++;
        position++;
    }
}

// Пропускаем все пробельные символы
void Lexer::skipWhitespace() {
    while (isspace((unsigned char)current()))
        advance();
}

// Пропускаем комментарии (Pascal: { ... } или (* ... *))
void Lexer::skipComment() {
    if (current() == '{') {
        advance();
        while (current() != '}' && current() != '\0')
            advance();
        if (current() == '}')
            advance();
    }
    else if (current() == '(' && peek() == '*') {
        advance();
        advance();
        int level = 1;
        while (level > 0 && current() != '\0') {
            if (current() == '(' && peek() == '*') {
                advance(); advance();
                level++;
            }
            else if (current() == '*' && peek() == ')') {
                advance(); advance();
                level--;
            }
            else
                advance();
        }
    }
}

// Проверяет, совпадает ли текущий символ с ожидаемым, и перемещается, если да
bool Lexer::match(char expected) {
    if (current() == expected) {
        advance();
        return true;
    }
    return false;
}

// Создаем токен с указанным типом и значением
Token Lexer::makeToken(TokenType type, const string& value) {
    return Token(type, value, line, column);
}

// Получение словаря ключевых слов Pascal
const unordered_map<string, TokenType>& Lexer::getKeywords() const {
    static unordered_map<string, TokenType> keywords = {
        {"program", TokenType::Program},
        {"var", TokenType::Var},
        {"const", TokenType::Const},
        {"begin", TokenType::Begin},
        {"end", TokenType::End},
        {"if", TokenType::If},
        {"then", TokenType::Then},
        {"else", TokenType::Else},
        {"while", TokenType::While},
        {"do", TokenType::Do},
        {"read", TokenType::Read},
        {"write", TokenType::Write},
        {"readln", TokenType::Readln},
        {"writeln", TokenType::Writeln},
        {"integer", TokenType::Integer},
        {"real", TokenType::Real},
        {"boolean", TokenType::Boolean},
        {"string", TokenType::StringType},
        {"true", TokenType::True},
        {"false", TokenType::False},
        {"array", TokenType::Array},
        {"of", TokenType::Of},
        {"procedure", TokenType::Procedure},
        {"function", TokenType::Function},
        {"return", TokenType::Return},
        {"div", TokenType::DivKeyword},
        {"mod", TokenType::Mod},
        {"and", TokenType::And},
        {"or", TokenType::Or},
        {"not", TokenType::Not},
        {"for", TokenType::For},
        {"in", TokenType::Return}, // Временно используем Return вместо In
        {"to", TokenType::To},
        {"downto", TokenType::Downto},
    };
    return keywords;
}


// Считываем число (целое или вещественное)
Token Lexer::readNumber() {
    string num;
    bool isReal = false;
    int startCol = column;

    while (isdigit((unsigned char)current())) {
        num += current();
        advance();
    }

    if (current() == '.') {
        isReal = true;
        num += current();
        advance();

        while (isdigit((unsigned char)current())) {
            num += current();
            advance();
        }
    }

    return isReal
        ? makeToken(TokenType::RealLiteral, num)
        : makeToken(TokenType::Number, num);
}

// Считываем идентификатор или ключевое слово
Token Lexer::readIdentifierOrKeyword() {
    string text;
    int startCol = column;

    while (isAlphaCyrillic((unsigned char)current()) || isdigit((unsigned char)current()) || current() == '_') {
        text += current();
        advance();
    }

    const auto& keywords = getKeywords();
    auto it = keywords.find(text);
    if (it != keywords.end())
        return makeToken(it->second, text);

    return makeToken(TokenType::Identifier, text);
}

// Считываем строковый литерал (в одинарных или двойных кавычках)
Token Lexer::readString() {
    string str;
    int startCol = column;
    char quote = current(); // Запоминаем кавычку (' или ")
    advance(); // Пропускаем открывающую кавычку

    while (current() != quote && current() != '\0') {
        if (current() == '\\') {
            advance(); // Пропускаем обратный слеш
            if (current() == '\0')
                break;
            // Обработка escape-последовательностей
            if (current() == 'n')
                str += '\n';
            else if (current() == 't')
                str += '\t';
            else
                str += current(); // Прочие символы берём как есть
            advance();
        }
        else {
            str += current();
            advance();
        }
    }

    if (current() == quote) {
        advance(); // Пропускаем закрывающую кавычку
        return makeToken(TokenType::StringLiteral, str);
    }

    throw runtime_error("Unterminated string literal");
}

// Основной метод: разбить исходный текст на токены
vector<Token> Lexer::tokenize() {
    vector<Token> tokens;

    while (current() != '\0') {
        skipWhitespace();
        skipComment();

        if (current() == '.' && peek() == '.') {
            std::cout << "Found DotDot (..) at position " << position << std::endl;
            advance(); advance();
            tokens.push_back(makeToken(TokenType::DotDot, ".."));
            std::cout << "Added DotDot token to tokens list" << std::endl;
            continue;
        }


        char c = current();
        if (c == '\0')
            break;

        if (isAlphaCyrillic((unsigned char)c) || c == '_')
            tokens.push_back(readIdentifierOrKeyword());
        else if (isdigit((unsigned char)c))
            tokens.push_back(readNumber());
        else if (c == '"' || c == '\'')
            tokens.push_back(readString());
        else {
            switch (c) {
            case '+':
                tokens.push_back(makeToken(TokenType::Plus, "+"));
                advance();
                break;
            case '-':
                tokens.push_back(makeToken(TokenType::Minus, "-"));
                advance();
                break;
            case '*':
                tokens.push_back(makeToken(TokenType::Multiply, "*"));
                advance();
                break;
            case '/':
                tokens.push_back(makeToken(TokenType::Divide, "/"));
                advance();
                break;
            case '=':
                tokens.push_back(makeToken(TokenType::Equal, "="));
                advance();
                break;
            case '<':
                advance();
                if (current() == '=') {
                    tokens.push_back(makeToken(TokenType::LessEqual, "<="));
                    advance();
                }
                else if (current() == '>') {
                    tokens.push_back(makeToken(TokenType::NotEqual, "<>"));
                    advance();
                }
                else
                    tokens.push_back(makeToken(TokenType::Less, "<"));
                break;
            case '>':
                advance();
                if (current() == '=') {
                    tokens.push_back(makeToken(TokenType::GreaterEqual, ">="));
                    advance();
                }
                else
                    tokens.push_back(makeToken(TokenType::Greater, ">"));
                break;
            case ':':
                advance();
                if (current() == '=') {
                    tokens.push_back(makeToken(TokenType::Assign, ":="));
                    advance();
                }
                else
                    tokens.push_back(makeToken(TokenType::Colon, ":"));
                break;
            case ';':
                tokens.push_back(makeToken(TokenType::Semicolon, ";"));
                advance();
                break;
            case ',':
                tokens.push_back(makeToken(TokenType::Comma, ","));
                advance();
                break;
            case '.':
                tokens.push_back(makeToken(TokenType::Dot, "."));
                advance();
                break;
            case '(':
                tokens.push_back(makeToken(TokenType::LParen, "("));
                advance();
                break;
            case ')':
                tokens.push_back(makeToken(TokenType::RParen, ")"));
                advance();
                break;
            case '[':
                tokens.push_back(makeToken(TokenType::LBracket, "["));
                advance();
                break;
            case ']':
                tokens.push_back(makeToken(TokenType::RBracket, "]"));
                advance();
                break;
            default:
                string error = "Unexpected character: ";
                error += c;
                throw runtime_error(error);
            }
        }
    }

    tokens.push_back(makeToken(TokenType::EndOfFile, ""));
    return tokens;
}
