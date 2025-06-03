#pragma once

#include "ast.h"
#include "interfaces.h"
#include "error_reporter.h"
#include "postfix.h"  // �������� ������ ����������� PostfixCalculator
#include <map>
#include <vector>
#include <string>
#include <memory>

enum class ValueType { Integer, Real, Boolean, String };

/**
 * ������������� ��������� ��� �������� �������� ������ ����� � Pascal--
 * ������������ ������ �������� ���� ������: Integer, Real, Boolean � String
 * � ������������� ������ ��� �������������� ����� ����
 */
struct Value {
    ValueType type;        // ��� �������� (Integer, Real, Boolean, String)
    int intValue;          // ������������� �������� (��� ���� Integer)
    double realValue;      // ������������ �������� (��� ���� Real)
    bool boolValue;        // ���������� �������� (��� ���� Boolean)
    string stringValue;    // ��������� �������� (��� ���� String)
    
    /**
     * ������ ��� ����������� �������������� ����� ������
     * ���������� ���������� ��� ������������� ��������������
     */
    int toInt() const;      // �������������� � ������ �����
    double toReal() const;  // �������������� � ������������� �����
    bool toBool() const;    // �������������� � ����������� ��������
    string toString() const; // �������������� � ������
    
    // ������������ ��� ��������� ����� ������
    Value();                  // ����������� �� ��������� (������� ������� ��������)
    explicit Value(int v);    // �������� �� ������ �����
    explicit Value(double v); // �������� �� ������������� �����
    explicit Value(bool v);   // �������� �� ����������� ��������
    explicit Value(const string& v); // �������� �� ������
};

/**
 * ����� �������������� ����� Pascal--
 * �������� �� ���������� ���������, �������������� � ���� ������������ ��������������� ������ (AST)
 * �������� ������� ��� ���������� �����������, ���������� ��������� � ���������� ����������
 * ���������� PostfixCalculator ��� ���������� ��������� � ����������� �����
 */
class Interpreter : public IInterpreter {
public:
    /**
     * ����������� �� ���������
     * ������� ������������� ��� ����������� ������
     */
    Interpreter();
    
    /**
     * ����������� � ������������ ������
     * @param errorReporter ���������� ������ ��� ������ ��������� �� ������� � ���������������
     */
    explicit Interpreter(shared_ptr<IErrorReporter> errorReporter);
    
    /**
     * ���������� ������� ���������� IInterpreter
     */

    /**
     * ��������� ������������� ���������, �������������� ������� AST
     * @param ast �������� ���� AST ���������
     */
    void run(const shared_ptr<ASTNode>& ast) override;
    
    /**
     * ��������� �������� ���������� ���������
     * @param expression ��������� ������������� ���������
     * @return ��������� ���������� ���������
     */
    Value evaluate(const string& expression) override;
    
    /**
     * ���������, ��������� �� ���������� � ��������� ������
     * @param name ��� ����������
     * @return true, ���� ���������� ���������, ����� false
     */
    bool isDeclared(const string& name) const override;
    
    /**
     * �������� �������� ���������� �� �����
     * @param name ��� ����������
     * @return �������� ���������� ��� ���������� ����������, ���� ���������� �� �������
     */
    Value getVariable(const string& name) const override;
    
    /**
     * ������������� �������� ����������
     * @param name ��� ����������
     * @param value ����� �������� ����������
     */
    void setVariable(const string& name, const Value& value) override;
    
    /**
     * ������� ������� �������� (������� ��� ����������)
     */
    void clearSymbols() override;
    
    /**
     * ���������� ��� ����������
     * @return ������ "Interpreter"
     */
    string getComponentName() const override { return "Interpreter"; }
    
    /**
     * �������������� ������ ��������������
     */

    /**
     * ��������� �������� ����� For � Pascal--
     * @param node ���� AST, �������������� �������� For
     */
    void executeFor(const shared_ptr<ASTNode>& node);

    /**
     * ���������� ������������� �������� ����������
     * @param name ��� ����������
     * @return ������������� �������� ���������� ��� ��������������� �������� ������� ����
     * @throws ����������, ���� ���������� �� ������� ��� �� ����� ���� ������������� � ������ ����
     */
    int getVarValue(const string& name) const;

    /**
     * ���������� ��� ����������
     * @param name ��� ����������
     * @return ��� ���������� (Integer, Real, Boolean, String)
     * @throws ����������, ���� ���������� �� �������
     */
    ValueType getValueType(const string& name) const;

    /**
     * ���������� ������ �� ������� �������� (��� ����������)
     * @return ����������� ������ �� ����� ��������
     */
    const map<string, Value>& getAllSymbols() const { return symbols; }
    
    /**
     * ������ ��� ���������� ������ � ��������������
     */

    /**
     * �������� �� ������ � ��������������
     * @param message ��������� �� ������
     * @param line ����� ������ (�� ��������� 0)
     * @param column ����� ������� (�� ��������� 0)
     */
    void reportError(const string& message, int line = 0, int column = 0) const;

    /**
     * �������� � �������������� � ��������������
     * @param message ��������� � ��������������
     * @param line ����� ������ (�� ��������� 0)
     * @param column ����� ������� (�� ��������� 0)
     */
    void reportWarning(const string& message, int line = 0, int column = 0) const;

private:
    map<string, Value> symbols;
    shared_ptr<IErrorReporter> errorReporter;
    unique_ptr<PostfixCalculator> postfixCalculator;
    
    // ������ ���������� ����������
    void executeAssignment(const std::shared_ptr<ASTNode>& node);
    void executeIf(const std::shared_ptr<ASTNode>& node);
    void executeWhile(const std::shared_ptr<ASTNode>& node);
    void executeWrite(const std::shared_ptr<ASTNode>& node);
    void executeRead(const std::shared_ptr<ASTNode>& node);
    
    // ������ ��� ���������� ���������
    Value evaluateExpression(const std::shared_ptr<ASTNode>& node);
    Value evaluateUsingPostfix(const std::shared_ptr<ASTNode>& node);
};
