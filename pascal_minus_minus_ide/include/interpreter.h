#pragma once

#include "ast.h"
#include <map>

enum class ValueType { Integer, Real, Boolean, String };

// ������������� ��������� ��� ��������
struct Value {
    ValueType type;
    int intValue;
    double realValue;
    bool boolValue;
    string strValue;

    Value();
    explicit Value(int v);
    explicit Value(double v);
    explicit Value(bool v);
    explicit Value(const string& v);
};

class Interpreter {
public:
    Interpreter();

    void run(const shared_ptr<ASTNode>& root);
    int getVarValue(const string& name) const;
    const map<string, Value>& getAllSymbols() const { return symbols; }

private:
    map<string, Value> symbols;

    void executeAssignment(const shared_ptr<ASTNode>& node);
    void executeIf(const shared_ptr<ASTNode>& node);
    void executeWhile(const shared_ptr<ASTNode>& node);
    void executeWrite(const shared_ptr<ASTNode>& node);
    void executeRead(const shared_ptr<ASTNode>& node);

    Value evaluateExpression(const shared_ptr<ASTNode>& node);
    ValueType getValueType(const string& name) const; // ��������: ������� ��� ���������� ��� �������, ���� ��� � �������
    double toReal(const Value& v) const;
};