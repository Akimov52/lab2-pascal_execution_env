#ifndef LEXER_H
#define LEXER_H

#include <string>
#include <vector>
#include <unordered_map>
#include <cctype>
#include <stdexcept>
#include <memory>
#include "interfaces.h"
#include "error_reporter.h"

using namespace std;

// ��������� ����������� TokenType � ��������� ������������ ����
namespace PascalToken {
    // ������������ ���� ��������� ����� ������� ��� ������������ �������
    enum TokenType {
        // �������� �����
        Program, Var, Const, Begin, End,
        If, Then, Else, While, Do,
        Read, Write, Readln, Writeln,
        Integer, Real, Boolean, StringType,
        For, To, Downto,
        True, False,

        // ��������� � �����������
        Plus, Minus, Multiply, Divide,
        DivKeyword, Mod,
        Assign, Equal, NotEqual, Less, LessEqual, Greater, GreaterEqual,
        Semicolon, Colon, Comma, Dot, LParen, RParen, 
        // ������� LBracket, RBracket (��������� � ���������)
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
class Lexer : public ILexer {
private:
    string source;                // �������� ����� ���������
    size_t position;              // ������� ������� � ������
    int line;                     // ������� ������
    int column;                   // ������� �������
    std::shared_ptr<IErrorReporter> errorReporter; // ���������� ������

    // ��������������� ������ ��� ������� ������
    char current() const;
    char peek() const;
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
    Lexer(const string& source, std::shared_ptr<IErrorReporter> reporter); // ����������� � ������������ ������
    vector<Token> tokenize() override;    // �������� �����: ������� ����� �� ������
    
    // �������������� ������
    int getLine() const { return line; }
    int getColumn() const { return column; }
    std::string getSourceFragment(int line, int column, int length = 10) const; // �������� �������� ��������� ����
};

#endif // LEXER_H