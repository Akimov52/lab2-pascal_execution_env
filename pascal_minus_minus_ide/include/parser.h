#ifndef PARSER_H
#define PARSER_H

/**
 * @file parser.h
 * @brief Синтаксический анализатор (парсер) для языка Pascal--
 * 
 * Отвечает за построение абстрактного синтаксического дерева (AST)
 * из последовательности токенов, полученных от лексера.
 */

#include "ast.h"
#include "lexer.h"
#include "interfaces.h"
#include "error_reporter.h"

/**
 * Класс синтаксического анализатора (парсера)
 * Реализует нисходящий рекурсивный анализ с предпросмотром для построения AST.
 * Предоставляет методы для разбора всех конструкций языка Pascal--
 */
class Parser : public IParser {
public:
    /**
     * Конструктор по умолчанию
     * Создает парсер без начальных токенов и обработчика ошибок
     */
    Parser();
    
    /**
     * Конструктор с обработчиком ошибок
     * @param errorReporter Указатель на обработчик ошибок
     */
    explicit Parser(std::shared_ptr<IErrorReporter> errorReporter);

    /**
     * Конструктор принимает вектор токенов, полученных из лексера
     * @param tokens Список токенов для разбора
     * @param errorReporter Опциональный обработчик ошибок
     */
    explicit Parser(const vector<Token>& tokens, std::shared_ptr<IErrorReporter> errorReporter = nullptr);

    /**
     * Реализация метода интерфейса IParser для синтаксического анализа
     * @return Указатель на корневой узел построенного абстрактного синтаксического дерева
     */
    shared_ptr<ASTNode> parse() override;
    
    /**
     * Возвращает имя компонента
     * @return Строка с именем компонента ("Parser")
     */
    std::string getComponentName() const override { return "Parser"; }

private:
    std::vector<Token> tokens;   // Список токенов для разбора
    size_t pos;                  // Текущая позиция в списке токенов
    std::shared_ptr<IErrorReporter> errorReporter; // Обработчик ошибок

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
    shared_ptr<ASTNode> parseFor();              // for ... to ... do ...

};

#endif // PARSER_H