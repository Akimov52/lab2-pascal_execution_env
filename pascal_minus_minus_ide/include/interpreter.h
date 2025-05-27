#pragma once

#include "ast.h"
#include <map>
#include <vector>
#include <string>

enum class ValueType { Integer, Real, Boolean, String, Array };

// Универсальная структура для значений
struct Value {
    ValueType type;
    int intValue;
    double realValue;
    bool boolValue;
    std::string strValue;
    std::vector<Value> arrayValue;  // Поле для массива

    Value();
    explicit Value(int v);
    explicit Value(double v);
    explicit Value(bool v);
    explicit Value(const std::string& v);
    explicit Value(const std::vector<Value>& arr);  // Конструктор для массива
};

class Interpreter {
public:
    Interpreter();
    void run(const std::shared_ptr<ASTNode>& root);
    void executeFor(const std::shared_ptr<ASTNode>& node);
    int getVarValue(const std::string& name) const;
    const std::map<std::string, Value>& getAllSymbols() const { return symbols; }

private:
    std::map<std::string, Value> symbols;
    void executeAssignment(const std::shared_ptr<ASTNode>& node);
    void executeIf(const std::shared_ptr<ASTNode>& node);
    void executeWhile(const std::shared_ptr<ASTNode>& node);
    void executeWrite(const std::shared_ptr<ASTNode>& node);
    void executeRead(const std::shared_ptr<ASTNode>& node);
    Value evaluateExpression(const std::shared_ptr<ASTNode>& node);
    ValueType getValueType(const std::string& name) const;
    double toReal(const Value& v) const;
};