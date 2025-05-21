#include "interpreter.h" // ������������ ���� � ��������� ������ Interpreter � ��������� Value
#include <iostream>      // ��� �����/������
#include <limits>        // ��� numeric_limits, ������������� � Readln
#include <cmath>
#include <algorithm>

// ========================
// ���������� ������������� Value (�������������� ���������� ��������)
// ========================

// ����������� �� ���������: �������� ���� Integer, ������ 0
Value::Value() : type(ValueType::Integer), intValue(0), realValue(0.0), boolValue(false), strValue("") {}

// ����������� ��� ������ ��������
Value::Value(int v) : type(ValueType::Integer), intValue(v), realValue(0.0), boolValue(false), strValue("") {}

// ����������� ��� ������������� ��������
Value::Value(double v) : type(ValueType::Real), intValue(0), realValue(v), boolValue(false), strValue("") {}

// ����������� ��� �������� ��������
Value::Value(bool v) : type(ValueType::Boolean), intValue(0), realValue(0.0), boolValue(v), strValue("") {}

// ����������� ��� ���������� ��������
Value::Value(const string& v) : type(ValueType::String), intValue(0), realValue(0.0), boolValue(false), strValue(v) {}

// ========================
// �������������
// ========================

Interpreter::Interpreter() = default; // ����������� �� ���������

// ������ ������������� ���������
void Interpreter::run(const shared_ptr<ASTNode>& root) {
    if (!root)
        return; // ���� ���� ������ � ������ �� ������
    switch (root->type) {
    case ASTNodeType::Program:
    case ASTNodeType::Block:
    case ASTNodeType::ConstSection:
    case ASTNodeType::VarSection:
        // ��������������� ��������� ��� ������� (���������)
        for (const auto& stmt : root->children)
            run(stmt);
        break;
    case ASTNodeType::ConstDecl:
    case ASTNodeType::VarDecl:
        break;
    case ASTNodeType::Assignment:
        executeAssignment(root); // ������������ ����������
        break;
    case ASTNodeType::If:
        executeIf(root); // �������� ��������
        break;
    case ASTNodeType::While:
        executeWhile(root); // ���� while
        break;
    case ASTNodeType::Write:
    case ASTNodeType::Writeln:
        executeWrite(root); // �����
        break;
    case ASTNodeType::Read:
    case ASTNodeType::Readln:
        executeRead(root); // ����
        break;

    case ASTNodeType::ProcCall: {
        // ����������� �����: �������� write/writeln/read/readln
        const string& name = root->value;
        if (name == "Writeln") {
            // ��������� ������ executeWrite + ������� �� ����� ������
            for (size_t i = 0; i < root->children.size(); ++i) {
                Value val = evaluateExpression(root->children[i]);
                switch (val.type) {
                case ValueType::Integer:
                    cout << val.intValue;
                    break;
                case ValueType::Real:
                    cout << val.realValue;
                    break;
                case ValueType::Boolean:
                    cout << (val.boolValue ? "true" : "false");
                    break;
                case ValueType::String:
                    cout << val.strValue;
                    break;
                }
                if (i + 1 < root->children.size())
                    cout << " ";
            }
            cout << endl;
            break;
        }
        else if (name == "Write") {
            // ������ executeWrite ��� �������� ������
            for (size_t i = 0; i < root->children.size(); ++i) {
                Value val = evaluateExpression(root->children[i]);
                switch (val.type) {
                case ValueType::Integer:
                    cout << val.intValue;
                    break;
                case ValueType::Real:
                    cout << val.realValue;
                    break;
                case ValueType::Boolean:
                    cout << (val.boolValue ? "true" : "false");
                    break;
                case ValueType::String:
                    cout << val.strValue;
                    break;
                }
                if (i + 1 < root->children.size())
                    cout << " ";
            }
            break;
        }
        else if (name == "Readln") {
            // ������ executeRead + ����� ������� ������
            for (const auto& child : root->children) {
                string varName = child->value;
                int v;
                cin >> v;
                symbols[varName] = Value(v);
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
            }
            break;
        }
        else if (name == "Read") {
            // executeRead ��� �������������
            for (const auto& child : root->children) {
                string varName = child->value;
                int v;
                cin >> v;
                symbols[varName] = Value(v);
            }
            break;
        }
        else
            throw runtime_error("����������� ���������: " + name);
    }
    default:
        throw runtime_error("����������� ��������");
    }
}

// ���������� ���������
Value Interpreter::evaluateExpression(const shared_ptr<ASTNode>& node) {
    switch (node->type) {
    case ASTNodeType::Number:
        return Value(stoi(node->value)); // ����������� ������ � �����
    case ASTNodeType::Real:
        return Value(stod(node->value));
    case ASTNodeType::Boolean:
        return Value(node->value == "true");
    case ASTNodeType::String:
        return Value(node->value); // ������ ������
    case ASTNodeType::Identifier: {
        // ����� ���������� �� �����
        auto it = symbols.find(node->value);
        if (it == symbols.end())
            throw runtime_error("����������� ����������: " + node->value);
        return it->second;
    }
    case ASTNodeType::BinOp: {
        // �������� ��������: ��������� ����� � ������ ��������
        Value left = evaluateExpression(node->children[0]);
        Value right = evaluateExpression(node->children[1]);

        // ��������� ��������� ����������
        if (node->value == "div") {
            if (left.type != ValueType::Integer || right.type != ValueType::Integer)
                throw runtime_error("�������� div �������� ������ � �����");
            return Value(left.intValue / right.intValue);
        }
        if (node->value == "mod") {
            if (left.type != ValueType::Integer || right.type != ValueType::Integer)
                throw runtime_error("�������� mod �������� ������ � �����");
            return Value(left.intValue % right.intValue);
        }
        if (node->value == "+") {
            if (left.type == ValueType::String || right.type == ValueType::String)
                return Value(left.strValue + right.strValue); // ������������ �����
            return Value(left.intValue + right.intValue);
        }
        if (node->value == "-")
            return Value(left.intValue - right.intValue);
        if (node->value == "*")
            return Value(left.intValue * right.intValue);
        if (node->value == "/") {
            if (right.intValue == 0)
                throw runtime_error("������� �� ����");
            return Value(left.intValue / right.intValue);
        }

        // ���������� ��������
        if (node->value == "and")
            return Value(left.boolValue && right.boolValue);
        if (node->value == "or")
            return Value(left.boolValue || right.boolValue);

        // ��������� ���������
        if (node->value == "=")
            return Value(left.intValue == right.intValue);
        if (node->value == "<>")
            return Value(left.intValue != right.intValue);
        if (node->value == "<")
            return Value(left.intValue < right.intValue);
        if (node->value == "<=")
            return Value(left.intValue <= right.intValue);
        if (node->value == ">")
            return Value(left.intValue > right.intValue);
        if (node->value == ">=")
            return Value(left.intValue >= right.intValue);

        throw runtime_error("����������� �������� ��������: " + node->value);
    }
    case ASTNodeType::UnOp:
        // ������� ��������
        if (node->value == "-")
            return Value(-evaluateExpression(node->children[0]).intValue);
        if (node->value == "not")
            return Value(!evaluateExpression(node->children[0]).boolValue);
        throw runtime_error("����������� ������� ��������: " + node->value);
    default:
        throw runtime_error("������ ���������� ���������");
    }
}

// ��������� ������������: ��� ���������� � ��������
void Interpreter::executeAssignment(const shared_ptr<ASTNode>& node) {
    string varName = node->children[0]->value;              // ��� ����������
    Value value = evaluateExpression(node->children[1]);    // ��������� ���������
    symbols[varName] = value;                               // ��������� � ������� ��������
}

// �������� ��������: if-then-else
void Interpreter::executeIf(const shared_ptr<ASTNode>& node) {
    Value cond = evaluateExpression(node->children[0]); // �������
    if (cond.boolValue)
        run(node->children[1]);                         // then
    else if (node->children.size() > 2)
        run(node->children[2]);                         // else
}

// ���� while
void Interpreter::executeWhile(const shared_ptr<ASTNode>& node) {
    while (evaluateExpression(node->children[0]).boolValue) // ���� ������� �������
        run(node->children[1]);                             // ��������� ���� �����
}

// ����� �������� (write/writeln)
void Interpreter::executeWrite(const shared_ptr<ASTNode>& node) {
    for (size_t i = 0; i < node->children.size(); ++i) {
        const auto& child = node->children[i];
        Value val = evaluateExpression(child); // ��������� ��������
        switch (val.type) {
        case ValueType::Integer:
            cout << val.intValue;
            break;
        case ValueType::Real:
            cout << val.realValue;
            break;
        case ValueType::Boolean:
            cout << (val.boolValue ? "true" : "false");
            break;
        case ValueType::String:
            cout << val.strValue;
            break;
        }
        if (i + 1 < node->children.size())
            cout << " "; // ������ ����� ����������
    }
    if (node->type == ASTNodeType::Writeln)
        cout << endl; // ������� ������ ��� writeln
}

// ���� �������� (read/readln)
void Interpreter::executeRead(const shared_ptr<ASTNode>& node) {
    for (const auto& child : node->children) {
        string varName = child->value;   // ��� ����������
        int value;
        cin >> value;                    // ������ �������� �� ������
        symbols[varName] = Value(value); // ��������� � ������� ��������
        if (node->type == ASTNodeType::Readln)
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); // ���������� ������� ������
    }
}

// ��������� �������� ���������� �� ����� (������������ ��� ������ ��� �������)
int Interpreter::getVarValue(const string& name) const {
    auto it = symbols.find(name);
    if (it == symbols.end())
        throw runtime_error("���������� �� �������: " + name);
    return it->second.intValue;
}