#ifndef LEXER_H
#define LEXER_H

#include <string>
#include <vector>
#include <unordered_map>
#include <cctype>
#include <stdexcept>
#include <memory>

using namespace std;

// Переносим определение TokenType в отдельное пространство имен
namespace PascalToken {
    // Перечисление всех возможных типов токенов для лексического анализа
    enum TokenType {
        // Ключевые слова
        Program, Var, Const, Begin, End,
        If, Then, Else, While, Do,
        Read, Write, Readln, Writeln,
        Integer, Real, Boolean, StringType,
        For, To, Downto,
        True, False,
        Array, Of, DotDot,
        Procedure, Function, Return,
        In, // Добавляем токен для оператора in

        // Операторы и разделители
        Plus, Minus, Multiply, Divide,
        DivKeyword, Mod,
        Assign, Equal, NotEqual, Less, LessEqual, Greater, GreaterEqual,
        Semicolon, Colon, Comma, Dot, LParen, RParen, LBracket, RBracket,
        And, Or, Not,

        // Литералы и идентификаторы
        Identifier, Number, String,
        RealLiteral, StringLiteral,

        // Конец файла
        EndOfFile
    };
}

// Используем TokenType из пространства имен PascalToken
using PascalToken::TokenType;

// Структура токена, возвращаемого лексером
struct Token {
    TokenType type;   // Тип токена (ключевое слово, оператор, идентификатор и т.д.)
    string value;     // Значение токена (текст)
    int line;         // Номер строки в исходном коде
    int column;       // Позиция (столбец) в строке

    Token();
    Token(TokenType t, const string& v, int l, int c);
};

// Класс лексического анализатора (лексера)
class Lexer {
private:
    string source;        // Исходный текст программы
    size_t position;      // Текущая позиция в строке
    int line;             // Текущая строка
    int column;           // Текущий столбец

    // Вспомогательные методы для анализа текста
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
    explicit Lexer(const string& source); // Конструктор принимает исходный текст
    vector<Token> tokenize();             // Основной метод: разбить текст на токены
};

#endif // LEXER_H