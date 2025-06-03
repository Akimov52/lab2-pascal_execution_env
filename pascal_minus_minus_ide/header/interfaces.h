#ifndef INTERFACES_H
#define INTERFACES_H

/**
 * @file interfaces.h
 * @brief ���������� ����������� �����������/�������������� Pascal--
 *
 * ���������� ����� ���������� ��� ���� �������� ����������� �������,
 * ��� ������������ ����������� � ����������� ������ ���������� ����������.
 */

#include <string>
#include <memory>
#include <vector>
#include <map>

// ��������������� ���������� �����
struct Token;
class ASTNode;
struct Value;
enum class ValueType;

/**
 * ������� ��������� ��� ���� ����������� �����������
 * ������������ ������ ������ ������������� �����������
 */
class ICompilerComponent {
public:
    virtual ~ICompilerComponent() = default;
    virtual std::string getComponentName() const = 0;
};

/**
 * ��������� ��� ������� ������� �� �������
 * �������� �� ����������� � ��������������� ����� ������ � ��������������
 */
class IErrorReporter {
public:
    virtual ~IErrorReporter() = default;
    virtual void reportError(const std::string& message, int line = -1, int column = -1) = 0;
    virtual void reportWarning(const std::string& message, int line = -1, int column = -1) = 0;
    virtual bool hasErrors() const = 0;
};

/**
 * ��������� ��� ������������ �����������
 * �������� �� �������������� ��������� ���� � ������������������ �������
 */
class ILexer : public ICompilerComponent {
public:
    virtual std::vector<Token> tokenize() = 0;
    virtual std::string getComponentName() const override { return "Lexer"; }
};

/**
 * ��������� ��� ��������������� �����������
 * �������� �� ���������� ������������ ��������������� ������ �� �������
 */
class IParser : public ICompilerComponent {
public:
    virtual std::shared_ptr<ASTNode> parse() = 0;
    virtual std::string getComponentName() const override { return "Parser"; }
};

/**
 * ��������� ��� ������������ ������������
 * �������� �� ���������� ��������� � �������������� �������� �������� ������
 */
class IPostfixCalculator {
public:
    virtual ~IPostfixCalculator() = default;
    virtual Value evaluate(const std::shared_ptr<ASTNode>& node, const std::map<std::string, Value>& variables) = 0;
    virtual Value performOperation(const std::string& op, const std::vector<Value>& operands) = 0;
};

/**
 * ��������� ��� ��������������
 * �������� �� ���������� ��������� �� ������ ������������ ��������������� ������
 * � ���������� �����������, ��������� � �����������
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
