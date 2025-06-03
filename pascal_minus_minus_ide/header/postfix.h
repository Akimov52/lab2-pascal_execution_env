#ifndef POSTFIX_H
#define POSTFIX_H

/**
 * @file postfix.h
 * @brief ����������� ��������� �� ������ �������� �������� ������ (���)
 * 
 * ��������� ���������� �������� ��������� � �������������� ��������� �������� �������� ������.
 * ������������ ��������� ���� ����� �������� ����� Pascal--
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
 * ���� ���������� ��� ���������� ��������� � �������� �������� ������ (���, postfix notation)
 * ������������ ���� �������������� ���������� � ����� Pascal--
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
 * ��������� ��� ������������� ���������� �� ����������
 * �������� ������ � ���� ���������, ��� ����������, ������� � ���������������
 */
struct OperatorInfo {
    OperatorType type;   // ��� ���������
    int precedence;     // ��������� (��� ����, ��� ������ �����������)
    bool isUnary;       // �������� �� �������� �������
    bool rightAssoc;    // ��������������� ������ (��� ������� ����������)
};

/**
 * ����� ��� ���������� ��������� � �������� �������� ������ (���, postfix notation)
 * ��������� �������� ���������� ��������� � �������������� �����
 * � �������������� ��������� ��������� � �����������
 */
class PostfixCalculator : public IPostfixCalculator {
public:
    PostfixCalculator();
    
    // ���������� ���������� IPostfixCalculator
    Value evaluate(const std::shared_ptr<ASTNode>& node, const std::map<std::string, Value>& variables) override;
    Value performOperation(const std::string& op, const std::vector<Value>& operands) override;
    
    // �������������� ������ ��� ������ � ������������ �����������
    // ��������� �������� ���������, ��������������� � ���� ������� ������� (postfix)
    Value evaluatePostfix(const std::vector<std::string>& tokens, const std::map<std::string, Value>& variables);
    
    // ������������� ��������� ��������� (��� ������) � ������ ������� � ����������� �����
    std::vector<std::string> infixToPostfix(const std::vector<std::string>& infix);
    
    // ������������� ��� � ����������� �����
    std::vector<std::string> astToPostfix(const std::shared_ptr<ASTNode>& node);

private:
    std::map<std::string, OperatorInfo> operatorMap;
    
    // ������������� ����� ����������
    void initOperatorMap();
    
    // ��������, �������� �� ����� ����������
    bool isOperator(const std::string& token) const;
    
    // ��������� ���������� �� ���������
    OperatorInfo getOperatorInfo(const std::string& token) const;
    
    // ��������������� ����� ��� ���������� �������� � ����� ����������
    Value performBinaryOperation(const std::string& op, const Value& a, const Value& b);
    
    // ��������������� ����� ��� ���������� ������� ��������
    Value performUnaryOperation(const std::string& op, const Value& a);
    
    // ����������� ����� ��� �������������� ��� � ����������� �����
    void processASTNode(const std::shared_ptr<ASTNode>& node, std::vector<std::string>& output);
};

#endif // POSTFIX_H