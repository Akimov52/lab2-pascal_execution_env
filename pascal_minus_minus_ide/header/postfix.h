#ifndef POSTFIX_H
#define POSTFIX_H

/**
 * @file postfix.h
 * @brief Калькулятор выражений на основе обратной польской записи (ОПЗ)
 * 
 * Реализует вычисление значений выражений с использованием алгоритма обратной польской записи.
 * Обеспечивает обработку всех типов операций языка Pascal--
 */

#include <string>
#include <vector>
#include <map>
#include <functional>
#include <stack>
#include <stdexcept>
#include "interfaces.h"
#include "ast.h"

using namespace std;

/**
 * Типы операторов для вычисления выражений в обратной польской записи (ОПЗ, postfix notation)
 * Перечисление всех поддерживаемых операторов в языке Pascal--
 */
enum class OperatorType {
    Plus,
    Minus,
    Multiply,
    Divide,
    IntegerDivide,   // div
    Modulus,         // mod
    Equal,           // =
    NotEqual,        // <>
    Less,            // <
    LessEqual,       // <=
    Greater,         // >
    GreaterEqual,    // >=
    And,             // and
    Or,              // or
    Not              // not
};

/**
 * Структура для представления информации об операторах
 * Содержит данные о типе оператора, его приоритете, арности и ассоциативности
 */
struct OperatorInfo {
    OperatorType type;   // Тип оператора
    int precedence;     // Приоритет (чем выше, тем раньше выполняется)
    bool isUnary;       // Является ли оператор унарным
    bool rightAssoc;    // Ассоциативность справа (для унарных операторов)
};

/**
 * Класс для вычисления выражений в обратной польской записи (ОПЗ, postfix notation)
 * Реализует алгоритм вычисления выражений с использованием стека
 * и преобразование инфиксных выражений в постфиксные
 */
class PostfixCalculator : public IPostfixCalculator {
public:
    PostfixCalculator();
    
    // Реализация интерфейса IPostfixCalculator
    Value evaluate(const std::shared_ptr<ASTNode>& node, const std::map<std::string, Value>& variables) override;
    Value performOperation(const std::string& op, const std::vector<Value>& operands) override;
    
    // Дополнительные методы для работы с постфиксными выражениями
    // Вычислить значение выражения, представленного в виде вектора токенов (postfix)
    Value evaluatePostfix(const std::vector<std::string>& tokens, const std::map<std::string, Value>& variables);
    
    // Преобразовать инфиксное выражение (как строку) в вектор токенов в постфиксной форме
    std::vector<std::string> infixToPostfix(const std::vector<std::string>& infix);
    
    // Преобразовать АСТ в постфиксную форму
    std::vector<std::string> astToPostfix(const std::shared_ptr<ASTNode>& node);

private:
    std::map<std::string, OperatorInfo> operatorMap;
    
    // Инициализация карты операторов
    void initOperatorMap();
    
    // Проверка, является ли токен оператором
    bool isOperator(const std::string& token) const;
    
    // Получение информации об операторе
    OperatorInfo getOperatorInfo(const std::string& token) const;
    
    // Вспомогательный метод для выполнения операции с двумя операндами
    Value performBinaryOperation(const std::string& op, const Value& a, const Value& b);
    
    // Вспомогательный метод для выполнения унарной операции
    Value performUnaryOperation(const std::string& op, const Value& a);
    
    // Рекурсивный метод для преобразования АСТ в постфиксную форму
    void processASTNode(const std::shared_ptr<ASTNode>& node, std::vector<std::string>& output);
};

#endif // POSTFIX_H