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

        // Операторы и разделители
        Plus, Minus, Multiply, Divide,
        DivKeyword, Mod,
        Assign, Equal, NotEqual, Less, LessEqual, Greater, GreaterEqual,
        Semicolon, Colon, Comma, Dot, LParen, RParen, 
        // Удалены LBracket, RBracket (связанные с массивами)
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
class Lexer : public ILexer {
private:
    string source;                // Исходный текст программы
    size_t position;              // Текущая позиция в строке
    int line;                     // Текущая строка
    int column;                   // Текущий столбец
    std::shared_ptr<IErrorReporter> errorReporter; // Обработчик ошибок

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
    Lexer(const string& source, std::shared_ptr<IErrorReporter> reporter); // Конструктор с обработчиком ошибок
    vector<Token> tokenize() override;    // Основной метод: разбить текст на токены
    
    // Дополнительные методы
    int getLine() const { return line; }
    int getColumn() const { return column; }
    std::string getSourceFragment(int line, int column, int length = 10) const; // Получить фрагмент исходного кода
};

#endif // LEXER_H