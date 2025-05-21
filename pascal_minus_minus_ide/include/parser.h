#ifndef PARSER_H
#define PARSER_H

#include "ast.h"

// Класс синтаксического анализатора (парсера)
class Parser {
public:
    // Конструктор принимает вектор токенов, полученных из лексера
    explicit Parser(const vector<Token>& tokens);

    // Основной метод: построить AST из токенов
    shared_ptr<ASTNode> parse();

private:
    const vector<Token>& tokens; // Список токенов для разбора
    size_t pos;                  // Текущая позиция в списке токенов

    // Получить текущий токен
    const Token& current() const;
    // Если текущий токен совпадает с ожидаемым типом — перейти к следующему
    bool match(TokenType type);
    // Проверить, что текущий токен нужного типа, иначе выбросить исключение с сообщением
    void expect(TokenType type, const string& errorMsg);

    // Методы разбора различных конструкций языка
    shared_ptr<ASTNode> parseProgram();          // program ... end.
    shared_ptr<ASTNode> parseBlock();            // begin ... end
    shared_ptr<ASTNode> parseStatement();        // Оператор (присваивание, if, while и т.д.)
    shared_ptr<ASTNode> parseAssignment();       // Присваивание
    shared_ptr<ASTNode> parseIf();               // if ... then ... else
    shared_ptr<ASTNode> parseWhile();            // while ... do ...
    shared_ptr<ASTNode> parseWrite();            // Write(...)
    shared_ptr<ASTNode> parseRead();             // Read(...)
    shared_ptr<ASTNode> parseReadln();           // Readln(...)
    shared_ptr<ASTNode> parseWriteln();          // Write(...)
    shared_ptr<ASTNode> parseExpression();       // Общее выражение
    shared_ptr<ASTNode> parseSimpleExpression(); // Простое выражение (без логики)
    shared_ptr<ASTNode> parseTerm();             // Термы (умножение, деление и т.д.)
    shared_ptr<ASTNode> parseFactor();           // Факторы (числа, идентификаторы, скобки)
    shared_ptr<ASTNode> parseVarSection();       // var ... ;
    shared_ptr<ASTNode> parseConstSection();     // const ... ;
    shared_ptr<ASTNode> parseArrayDecl();        // Объявление массива
    shared_ptr<ASTNode> parseProcedureDecl();    // Объявление процедуры
    shared_ptr<ASTNode> parseFunctionDecl();     // Объявление функции
    shared_ptr<ASTNode> parseCall();             // Вызов функции/процедуры
    shared_ptr<ASTNode> parseReturn();           // Оператор return
    shared_ptr<ASTNode> parseProcedureCall();    // Вызов процедуры
};

#endif // PARSER_H