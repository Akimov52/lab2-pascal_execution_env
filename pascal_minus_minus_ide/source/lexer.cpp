#include "lexer.h"
#include <stdexcept>
#include <cctype>
#include <unordered_map>
#include <string>

// ����������� �� ��������� ��� ������: ������������� ��� EndOfFile � ������ ��������
Token::Token() : type(TokenType::EndOfFile), value(""), line(0), column(0) {}

// ����������� ������ � �����������: ���, ��������, ������ � �������
Token::Token(TokenType t, const string& v, int l, int c)
    : type(t), value(v), line(l), column(c) {}

// ��������: �������� �� ������ ��������� ��� ������������� ������ (Windows-1251)
bool isAlphaCyrillic(unsigned char c) {
    // ��������
    if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z'))
        return true;
    // ��������� (Windows-1251: �-� 0xC0-0xDF, �-� 0xE0-0xFF, � 0xA8, � 0xB8)
    if ((c >= 0xC0 && c <= 0xDF) || (c >= 0xE0 && c <= 0xFF) || c == 0xA8 || c == 0xB8)
        return true;
    return false;
}

using namespace std;

// ����������� �������: ��������� �������� ����� ���������
Lexer::Lexer(const string& source)
    : source(source), position(0), line(1), column(1) {}

// �������� ������� ������
char Lexer::current() const {
    if (position >= source.length()) return '\0';
    return source[position];
}

// ���������� ��������� ������ (��� ����������� �������)
char Lexer::peek() const {
    if (position + 1 >= source.length()) return '\0';
    return source[position + 1];
}

// ���������� ������� �� ���� ������, �������� ������ � �������
void Lexer::advance() {
    if (position < source.length()) {
        if (source[position] == '\n') {
            line++;
            column = 1;
        } else {
            column++;
        }
        position++;
    }
}

// ���������� ��� ���������� �������
void Lexer::skipWhitespace() {
    while (isspace((unsigned char)current())) {
        advance();
    }
}

// ���������� ����������� (Pascal: { ... } ��� (* ... *))
void Lexer::skipComment() {
    if (current() == '{') {
        advance();
        while (current() != '}' && current() != '\0') advance();
        if (current() == '}') advance();
    } else if (current() == '(' && peek() == '*') {
        advance(); advance();
        int level = 1;
        while (level > 0 && current() != '\0') {
            if (current() == '(' && peek() == '*') {
                advance(); advance();
                level++;
            } else if (current() == '*' && peek() == ')') {
                advance(); advance();
                level--;
            } else {
                advance();
            }
        }
    }
}

// ���������, ��������� �� ������� ������ � ���������, � ������������, ���� ��
bool Lexer::match(char expected) {
    if (current() == expected) {
        advance();
        return true;
    }
    return false;
}

// ������� ����� � ��������� ����� � ���������
Token Lexer::makeToken(TokenType type, const string& value) {
    return Token(type, value, line, column);
}

// �������� ������� �������� ���� Pascal
const unordered_map<string, TokenType>& Lexer::getKeywords() const {
    static const unordered_map<string, TokenType> keywords = {
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
        {"and", TokenType::And},
        {"or", TokenType::Or},
        {"not", TokenType::Not}
    };
    return keywords;
}


// ��������� ����� (����� ��� ������������)
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

// ��������� ������������� ��� �������� �����
Token Lexer::readIdentifierOrKeyword() {
    string text;
    int startCol = column;

    while (isAlphaCyrillic((unsigned char)current()) || isdigit((unsigned char)current()) || current() == '_') {
        text += current();
        advance();
    }

    const auto& keywords = getKeywords();
    auto it = keywords.find(text);
    if (it != keywords.end()) {
        return makeToken(it->second, text);
    }

    return makeToken(TokenType::Identifier, text);
}

// ��������� ��������� ������� (� ��������� ��� ������� ��������)
Token Lexer::readString() {
    string str;
    int startCol = column;
    char quote = current(); // ����������� ������� (' ��� ")
    advance(); // ���������� ����������� �������

    while (current() != quote && current() != '\0') {
        if (current() == '\\') {
            advance(); // ���������� �������� ����
            if (current() == '\0') break;
            // ��������� escape-�������������������
            if (current() == 'n') {
                str += '\n';
            } else if (current() == 't') {
                str += '\t';
            } else {
                str += current(); // ������ �������� ������ ��� ����
            }
            advance();
        } else {
            str += current();
            advance();
        }
    }

    if (current() == quote) {
        advance(); // ���������� ����������� �������
        return makeToken(TokenType::StringLiteral, str);
    }

    throw runtime_error("Unterminated string literal");
}

// �������� �����: ������� �������� ����� �� ������
vector<Token> Lexer::tokenize() {
    vector<Token> tokens;

    while (current() != '\0') {
        skipWhitespace();
        skipComment();

        char c = current();
        if (c == '\0') break;

        if (isAlphaCyrillic((unsigned char)c) || c == '_') {
            tokens.push_back(readIdentifierOrKeyword());
        } else if (isdigit((unsigned char)c)) {
            tokens.push_back(readNumber());
        } else if (c == '"' || c == '\'') {
            tokens.push_back(readString());
        } else {
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
                    } else if (current() == '>') {
                        tokens.push_back(makeToken(TokenType::NotEqual, "<>"));
                        advance();
                    } else {
                        tokens.push_back(makeToken(TokenType::Less, "<"));
                    }
                    break;
                case '>':
                    advance();
                    if (current() == '=') {
                        tokens.push_back(makeToken(TokenType::GreaterEqual, ">="));
                        advance();
                    } else {
                        tokens.push_back(makeToken(TokenType::Greater, ">"));
                    }
                    break;
                case ':':
                    advance();
                    if (current() == '=') {
                        tokens.push_back(makeToken(TokenType::Assign, ":="));
                        advance();
                    } else {
                        tokens.push_back(makeToken(TokenType::Colon, ":"));
                    }
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