#ifndef LEXER_H
#define LEXER_H

#include <string>
#include <vector>
#include <unordered_map>
#include <cctype>
#include <stdexcept>
#include <memory>

using namespace std;

// ��������� ����������� TokenType � ��������� ������������ ����
namespace PascalToken {
    // ������������ ���� ��������� ����� ������� ��� ������������ �������
    enum TokenType {
        // �������� �����
        Program, Var, Const, Begin, End,
        If, Then, Else, While, Do,
        Read, Write,
        Readln, Writeln,
        Integer, Real, Boolean, StringType,
        True, False,
        Array, Of,
        Procedure, Function, Return,

        // ��������� � �����������
        Plus, Minus, Multiply, Divide,
        DivKeyword, Mod,
        Assign, Equal, NotEqual, Less, LessEqual, Greater, GreaterEqual,
        Semicolon, Colon, Comma, Dot, LParen, RParen, LBracket, RBracket,
        And, Or, Not,

        // �������� � ��������������
        Identifier, Number, String,
        RealLiteral, StringLiteral,

        // ����� �����
        EndOfFile
    };
}

// ���������� TokenType �� ������������ ���� PascalToken
using PascalToken::TokenType;

// ��������� ������, ������������� ��������
struct Token {
    TokenType type;   // ��� ������ (�������� �����, ��������, ������������� � �.�.)
    string value;     // �������� ������ (�����)
    int line;         // ����� ������ � �������� ����
    int column;       // ������� (�������) � ������

    Token();
    Token(TokenType t, const string& v, int l, int c);
};

// ����� ������������ ����������� (�������)
class Lexer {
private:
    string source;        // �������� ����� ���������
    size_t position;      // ������� ������� � ������
    int line;             // ������� ������
    int column;           // ������� �������

    // ��������������� ������ ��� ������� ������
    char current() const;
    char peek() const;
    char peekNext() const;
    void advance();
    void skipWhitespace();
    void skipComment();
    bool match(char expected);
    Token readNumber();
    Token readIdentifierOrKeyword();
    Token readString();
    Token makeToken(TokenType type, const string& value);
    const unordered_map<string, TokenType>& getKeywords() const;

public:
    explicit Lexer(const string& source); // ����������� ��������� �������� �����
    vector<Token> tokenize();             // �������� �����: ������� ����� �� ������
};

#endif // LEXER_H