#ifndef INTERFACES_H
#define INTERFACES_H

/**
 * @file interfaces.h
 * @brief Интерфейсы компонентов компилятора/интерпретатора Pascal--
 *
 * Определяет общие интерфейсы для всех основных компонентов системы,
 * что обеспечивает модульность и возможность замены конкретных реализаций.
 */

#include <string>
#include <memory>
#include <vector>
#include <map>

// Предварительные объявления типов
struct Token;
class ASTNode;
struct Value;
enum class ValueType;

/**
 * Базовый интерфейс для всех компонентов компилятора
 * Обеспечивает единый способ идентификации компонентов
 */
class ICompilerComponent {
public:
    virtual ~ICompilerComponent() = default;
    virtual std::string getComponentName() const = 0;
};

/**
 * Интерфейс для системы отчетов об ошибках
 * Отвечает за регистрацию и форматированный вывод ошибок и предупреждений
 */
class IErrorReporter {
public:
    virtual ~IErrorReporter() = default;
    virtual void reportError(const std::string& message, int line = -1, int column = -1) = 0;
    virtual void reportWarning(const std::string& message, int line = -1, int column = -1) = 0;
    virtual bool hasErrors() const = 0;
};

/**
 * Интерфейс для лексического анализатора
 * Отвечает за преобразование исходного кода в последовательность токенов
 */
class ILexer : public ICompilerComponent {
public:
    virtual std::vector<Token> tokenize() = 0;
    virtual std::string getComponentName() const override { return "Lexer"; }
};

/**
 * Интерфейс для синтаксического анализатора
 * Отвечает за построение абстрактного синтаксического дерева из токенов
 */
class IParser : public ICompilerComponent {
public:
    virtual std::shared_ptr<ASTNode> parse() = 0;
    virtual std::string getComponentName() const override { return "Parser"; }
};

/**
 * Интерфейс для постфиксного калькулятора
 * Отвечает за вычисление выражений с использованием обратной польской записи
 */
class IPostfixCalculator {
public:
    virtual ~IPostfixCalculator() = default;
    virtual Value evaluate(const std::shared_ptr<ASTNode>& node, const std::map<std::string, Value>& variables) = 0;
    virtual Value performOperation(const std::string& op, const std::vector<Value>& operands) = 0;
};

/**
 * Интерфейс для интерпретатора
 * Отвечает за выполнение программы на основе абстрактного синтаксического дерева
 * и управление переменными, функциями и процедурами
 */
class IInterpreter : public ICompilerComponent {
public:
    virtual void run(const std::shared_ptr<ASTNode>& ast) = 0;
    virtual Value evaluate(const std::string& expression) = 0;
    virtual bool isDeclared(const std::string& name) const = 0;
    virtual Value getVariable(const std::string& name) const = 0;
    virtual void setVariable(const std::string& name, const Value& value) = 0;
    virtual void clearSymbols() = 0;
    virtual const std::map<std::string, Value>& getAllSymbols() const = 0;
    virtual std::string getComponentName() const override { return "Interpreter"; }
};

#endif // INTERFACES_H
