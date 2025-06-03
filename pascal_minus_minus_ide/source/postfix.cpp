#include "postfix.h"
#include <stack>
#include <sstream>
#include <stdexcept>
#include <cmath>
#include <algorithm>
#include "interpreter.h" // ��� ������� � ���� Value
#include "logger.h"

// ��������������� �������: ���������� true, ���� ������ � ����� (����� ��� ������������)
static bool is_number(const string& s) {
    if (s.empty())
        return false;
    istringstream iss(s);
    double d;
    iss >> d;
    return iss.eof() && !iss.fail();
}

// ��������� �������� ��������� � ����������� ������ (Reverse Polish Notation)
// ���������� ���� ��� �������� ������������� �����������
// ����������� ��� PostfixCalculator
PostfixCalculator::PostfixCalculator() {
    initOperatorMap();
}

// ������������� ������� ���������� � ������������
void PostfixCalculator::initOperatorMap() {
    // �������������� ���������
    operatorMap["+"] = {OperatorType::Plus, 2, false, false};
    operatorMap["-"] = {OperatorType::Minus, 2, false, false};
    operatorMap["*"] = {OperatorType::Multiply, 3, false, false};
    operatorMap["/"] = {OperatorType::Divide, 3, false, false};
    operatorMap["div"] = {OperatorType::IntegerDivide, 3, false, false};
    operatorMap["mod"] = {OperatorType::Modulus, 3, false, false};
    
    // ��������� ���������
    operatorMap["="] = {OperatorType::Equal, 1, false, false};
    operatorMap["<>"] = {OperatorType::NotEqual, 1, false, false};
    operatorMap["<"] = {OperatorType::Less, 1, false, false};
    operatorMap["<="] = {OperatorType::LessEqual, 1, false, false};
    operatorMap[">"] = {OperatorType::Greater, 1, false, false};
    operatorMap[">="] = {OperatorType::GreaterEqual, 1, false, false};
    
    // ���������� ���������
    operatorMap["and"] = {OperatorType::And, 0, false, false};
    operatorMap["or"] = {OperatorType::Or, 0, false, false};
    operatorMap["not"] = {OperatorType::Not, 4, true, true};
    
    // ������� ���������
    operatorMap["u-"] = {OperatorType::Minus, 4, true, true}; // ������� �����
}

// ��������, �������� �� ����� ����������
bool PostfixCalculator::isOperator(const std::string& token) const {
    return operatorMap.find(token) != operatorMap.end();
}

// ��������� ���������� �� ���������
OperatorInfo PostfixCalculator::getOperatorInfo(const std::string& token) const {
    auto it = operatorMap.find(token);
    if (it != operatorMap.end()) {
        return it->second;
    }
    throw std::runtime_error("����������� ��������: " + token);
}

// ���������� ������ �� ���������� IPostfixCalculator
Value PostfixCalculator::performOperation(const std::string& op, const std::vector<Value>& operands) {
    // �������� ���������� ���������
    if (operands.empty()) {
        throw std::runtime_error("��� ��������� ��� ��������: " + op);
    }
    
    // ��������, ������� �� ��� ��������
    bool isUnary = operands.size() == 1 || op == "not" || op == "u-";
    
    if (isUnary) {
        if (operands.size() != 1) {
            throw std::runtime_error("������� �������� " + op + " ������� ���� �������");
        }
        return performUnaryOperation(op, operands[0]);
    } else {
        if (operands.size() != 2) {
            throw std::runtime_error("�������� �������� " + op + " ������� ��� ��������");
        }
        return performBinaryOperation(op, operands[0], operands[1]);
    }
}

// ���������� �������� �������� ��� ����������
Value PostfixCalculator::performBinaryOperation(const std::string& op, const Value& a, const Value& b) {
    Value result;
    
    // ���������, ��� �������� ����������� ���� ��� ������ ��������
    if (op == "+") {
        if (a.type == ValueType::Integer && b.type == ValueType::Integer) {
            result.type = ValueType::Integer;
            result.intValue = a.intValue + b.intValue;
        } else {
            result.type = ValueType::Real;
            result.realValue = (a.type == ValueType::Integer ? a.intValue : a.realValue) + 
                              (b.type == ValueType::Integer ? b.intValue : b.realValue);
        }
    } else if (op == "-") {
        if (a.type == ValueType::Integer && b.type == ValueType::Integer) {
            result.type = ValueType::Integer;
            result.intValue = a.intValue - b.intValue;
        } else {
            result.type = ValueType::Real;
            result.realValue = (a.type == ValueType::Integer ? a.intValue : a.realValue) - 
                              (b.type == ValueType::Integer ? b.intValue : b.realValue);
        }
    } else if (op == "*") {
        if (a.type == ValueType::Integer && b.type == ValueType::Integer) {
            result.type = ValueType::Integer;
            result.intValue = a.intValue * b.intValue;
        } else {
            result.type = ValueType::Real;
            result.realValue = (a.type == ValueType::Integer ? a.intValue : a.realValue) * 
                              (b.type == ValueType::Integer ? b.intValue : b.realValue);
        }
    } else if (op == "/") {
        // ������� ������ ��� ������������ ���������
        result.type = ValueType::Real;
        result.realValue = (a.type == ValueType::Integer ? a.intValue : a.realValue) / 
                          (b.type == ValueType::Integer ? b.intValue : b.realValue);
    } else if (op == "div") {
        // ������������� ������� ������� ������������� ���������
        if (a.type != ValueType::Integer || b.type != ValueType::Integer) {
            throw std::runtime_error("�������� 'div' ������� ������������� ���������");
        }
        if (b.intValue == 0) {
            throw std::runtime_error("������� �� ����");
        }
        result.type = ValueType::Integer;
        result.intValue = a.intValue / b.intValue;
    } else if (op == "mod") {
        // ������ ������� ������������� ���������
        if (a.type != ValueType::Integer || b.type != ValueType::Integer) {
            throw std::runtime_error("�������� 'mod' ������� ������������� ���������");
        }
        if (b.intValue == 0) {
            throw std::runtime_error("������� �� ���� � �������� mod");
        }
        result.type = ValueType::Integer;
        result.intValue = a.intValue % b.intValue;
    } 
    // ��������� ���������
    else if (op == "=" || op == "<>" || op == "<" || op == "<=" || op == ">" || op == ">=") {
        result.type = ValueType::Boolean;
        // ��������� �����
        if ((a.type == ValueType::Integer || a.type == ValueType::Real) && 
            (b.type == ValueType::Integer || b.type == ValueType::Real)) {
            double aVal = (a.type == ValueType::Integer) ? a.intValue : a.realValue;
            double bVal = (b.type == ValueType::Integer) ? b.intValue : b.realValue;
            
            if (op == "=") result.boolValue = (aVal == bVal);
            else if (op == "<>") result.boolValue = (aVal != bVal);
            else if (op == "<") result.boolValue = (aVal < bVal);
            else if (op == "<=") result.boolValue = (aVal <= bVal);
            else if (op == ">") result.boolValue = (aVal > bVal);
            else if (op == ">=") result.boolValue = (aVal >= bVal);
        }
        // ��������� ������� ��������
        else if (a.type == ValueType::Boolean && b.type == ValueType::Boolean) {
            if (op == "=") result.boolValue = (a.boolValue == b.boolValue);
            else if (op == "<>") result.boolValue = (a.boolValue != b.boolValue);
            else {
                throw std::runtime_error("��������� <, <=, >, >= �� ��������� � ���������� ���������");
            }
        }
        // ��������� �����
        else if (a.type == ValueType::String && b.type == ValueType::String) {
            if (op == "=") result.boolValue = (a.stringValue == b.stringValue);
            else if (op == "<>") result.boolValue = (a.stringValue != b.stringValue);
            else if (op == "<") result.boolValue = (a.stringValue < b.stringValue);
            else if (op == "<=") result.boolValue = (a.stringValue <= b.stringValue);
            else if (op == ">") result.boolValue = (a.stringValue > b.stringValue);
            else if (op == ">=") result.boolValue = (a.stringValue >= b.stringValue);
        }
        else {
            throw std::runtime_error("������������� ���� ��� ���������");
        }
    }
    // ���������� ���������
    else if (op == "and" || op == "or") {
        if (a.type != ValueType::Boolean || b.type != ValueType::Boolean) {
            throw std::runtime_error("���������� ��������� ������� ���������� ���������");
        }
        result.type = ValueType::Boolean;
        if (op == "and") result.boolValue = a.boolValue && b.boolValue;
        else if (op == "or") result.boolValue = a.boolValue || b.boolValue;
    }

    // ���� ����������� ��������
    else {
        throw std::runtime_error("����������� ��������: " + op);
    }
    
    return result;
}

// ���������� ������� �������� ��� ���������
Value PostfixCalculator::performUnaryOperation(const std::string& op, const Value& a) {
    Value result;
    
    if (op == "u-") { // ������� �����
        if (a.type == ValueType::Integer) {
            result.type = ValueType::Integer;
            result.intValue = -a.intValue;
        } else if (a.type == ValueType::Real) {
            result.type = ValueType::Real;
            result.realValue = -a.realValue;
        } else {
            throw std::runtime_error("������� ����� �������� ������ � ������");
        }
    } else if (op == "not") { // ���������� ���������
        if (a.type != ValueType::Boolean) {
            throw std::runtime_error("�������� 'not' ������� ����������� ��������");
        }
        result.type = ValueType::Boolean;
        result.boolValue = !a.boolValue;
    } else {
        throw std::runtime_error("����������� ������� ��������: " + op);
    }
    
    return result;
}

// ���������� ������ �� ���������� IPostfixCalculator
Value PostfixCalculator::evaluate(const std::shared_ptr<ASTNode>& node, const std::map<std::string, Value>& variables) {
    // ����������� AST � ����������� ������
    std::vector<std::string> postfix = astToPostfix(node);
    // ��������� �������� ������������ ���������
    return evaluatePostfix(postfix, variables);
}

// ���������� ��������� � ����������� ����� � ������ ����������
Value PostfixCalculator::evaluatePostfix(const std::vector<std::string>& tokens, const std::map<std::string, Value>& variables) {
    std::stack<Value> valueStack;
    
    for (const auto& token : tokens) {
        // ���� ����� - �����
        if (is_number(token)) {
            Value val;
            // ���������� ��� ����� (����� ��� ������������)
            if (token.find('.') != std::string::npos) {
                val.type = ValueType::Real;
                val.realValue = std::stod(token);
            } else {
                val.type = ValueType::Integer;
                val.intValue = std::stoi(token);
            }
            valueStack.push(val);
        }
        // ���� ����� - ��������� �������
        else if (token.size() >= 2 && token[0] == '\'' && token[token.size()-1] == '\'') {
            Value val;
            val.type = ValueType::String;
            val.stringValue = token.substr(1, token.size() - 2);
            valueStack.push(val);
        }
        // ���� ����� - ������ ��������
        else if (token == "true" || token == "false") {
            Value val;
            val.type = ValueType::Boolean;
            val.boolValue = (token == "true");
            valueStack.push(val);
        }
        // ���� ����� - ����������
        else if (variables.find(token) != variables.end()) {
            valueStack.push(variables.at(token));
        }
        // ���� ����� - ��������
        else if (isOperator(token)) {
            // ���������, ���������� �� ��������� � �����
            const auto& opInfo = getOperatorInfo(token);
            
            if (opInfo.isUnary) {
                if (valueStack.empty()) {
                    throw std::runtime_error("������������ ��������� ��� �������� ��������� " + token);
                }
                Value a = valueStack.top();
                valueStack.pop();
                std::vector<Value> operands = {a};
                valueStack.push(performOperation(token, operands));
            } else {
                if (valueStack.size() < 2) {
                    throw std::runtime_error("������������ ��������� ��� ��������� ��������� " + token);
                }
                Value b = valueStack.top();
                valueStack.pop();
                Value a = valueStack.top();
                valueStack.pop();
                std::vector<Value> operands = {a, b};
                valueStack.push(performOperation(token, operands));
            }
        }
        // ����������� �����
        else {
            throw std::runtime_error("����������� ����� � ����������� ���������: " + token);
        }
    }
    
    // ����� ���������� � ����� ������ �������� ����� ���� ���������
    if (valueStack.size() != 1) {
        throw std::runtime_error("������ ���������� ������������ ���������: �������� ���������� �����������");
    }
    
    return valueStack.top();
}

// ���������� ��������� ��������� (��� ���� ����� � ��� ���� ���������)
static int precedence(const string& op) {
    if (op == "+" || op == "-")
        return 1;
    if (op == "*" || op == "/")
        return 2;
    if (op == "^")
        return 3;
    return 0;
}

// ���������, �������� �� ������ ���������� (+, -, *, /, ^)
static bool is_operator(const string& op) {
    return op == "+" || op == "-" || op == "*" || op == "/" || op == "^";
}

// ��������� ������ �� ������ (�����, ���������, ������)
// ������: "2+3*(4-1)" -> ["2", "+", "3", "*", "(", "4", "-", "1", ")"]
static vector<string> tokenize(const string& expr) {
    vector<string> tokens;
    string num;
    for (size_t i = 0; i < expr.size(); ++i) {
        char c = expr[i];
        if (isspace(c))
            continue; // ���������� �������
        if (isdigit(c) || c == '.')
            // ���� ����� ��� ����� � ���������� ����������� �����
            num += c;
        else {
            // ���� ��������� �������� ��� ������ � ������� ��������� ����������� �����
            if (!num.empty()) {
                tokens.push_back(num);
                num.clear();
            }
            // ��������� �������� ��� ������ ��� ��������� �����
            if (is_operator(string(1, c)) || c == '(' || c == ')')
                tokens.push_back(string(1, c));
        }
    }
    // ��������� ��������� �����, ���� ��� ����
    if (!num.empty())
        tokens.push_back(num);
    return tokens;
}

// ��������� ��������� ��������� � ����������� (�������� ������������� ������� ��������)
// ��������, "2+3*4" -> ["2", "3", "4", "*", "+"]
std::vector<std::string> PostfixCalculator::infixToPostfix(const std::vector<std::string>& infix) {
    std::vector<std::string> output;     // �������� ������� (����������� ������)
    std::stack<std::string> ops;         // ���� ����������

    for (const auto& token : infix) {
        // ���� ����� - �����, �������������, ������ ��� ������ �������� - ����� � �������� �������
        if (is_number(token) || 
            (token.size() >= 2 && token[0] == '\'' && token[token.size()-1] == '\'') ||
            token == "true" || token == "false" ||
            (token[0] >= 'a' && token[0] <= 'z') || (token[0] >= 'A' && token[0] <= 'Z')) {
            output.push_back(token);
        }
        // ���� ����� - ��������
        else if (isOperator(token)) {
            const auto& currentOp = getOperatorInfo(token);
            
            // ����������� ��������� � ������� ��� ������ ����������� (� ������ ���������������)
            while (!ops.empty() && ops.top() != "(" && isOperator(ops.top())) {
                const auto& topOp = getOperatorInfo(ops.top());
                
                // ��� ����������������� ���������� ����������� ���� ��������� >= 
                // ��� ������������������ ���������� ����������� ���� ��������� >
                if ((!currentOp.rightAssoc && currentOp.precedence <= topOp.precedence) ||
                    (currentOp.rightAssoc && currentOp.precedence < topOp.precedence)) {
                    output.push_back(ops.top());
                    ops.pop();
                } else {
                    break;
                }
            }
            ops.push(token);
        }
        // ����������� ������ - � ����
        else if (token == "(") {
            ops.push(token);
        }
        // ����������� ������ - ����������� ��������� �� ����������� ������
        else if (token == ")") {
            while (!ops.empty() && ops.top() != "(") {
                output.push_back(ops.top());
                ops.pop();
            }
            
            if (!ops.empty() && ops.top() == "(") {
                ops.pop(); // ������� ����������� ������
            } else {
                throw std::runtime_error("��������������� ������ � ���������");
            }
        }
        // ����������� �����
        else {
            throw std::runtime_error("����������� ����� � ���������: " + token);
        }
    }
    
    // ����������� ���������� ���������
    while (!ops.empty()) {
        if (ops.top() == "(" || ops.top() == ")") {
            throw std::runtime_error("��������������� ������ � ���������");
        }
        output.push_back(ops.top());
        ops.pop();
    }
    
    return output;
}

// ����������� ��� � ������������������ ������� � ����������� �����
std::vector<std::string> PostfixCalculator::astToPostfix(const std::shared_ptr<ASTNode>& node) {
    std::vector<std::string> output;
    if (!node) return output;
    
    // ���������� ������������ ������
    processASTNode(node, output);
    
    LOG_DEBUG("����������� �����: " + [&output]() {
        std::string result;
        for (const auto& token : output) {
            result += token + " ";
        }
        return result;
    }());
    
    return output;
}

// ����������� ����� ��� �������������� ��� � ����������� �����
void PostfixCalculator::processASTNode(const std::shared_ptr<ASTNode>& node, std::vector<std::string>& output) {
    if (!node) return;
    
    switch (node->type) {
        // �������� ��������
        case ASTNodeType::Number:
        case ASTNodeType::Real:
            output.push_back(node->value);
            break;
            
        // ��������� ��������
        case ASTNodeType::String:
            output.push_back("'" + node->value + "'");
            break;
            
        // ������ ��������
        case ASTNodeType::Boolean:
            output.push_back(node->value); // true ��� false
            break;
            
        // �������������� (����������)
        case ASTNodeType::Identifier:
            output.push_back(node->value);
            break;
            
        // ������� ���������
        case ASTNodeType::UnOp:
            if (!node->children.empty()) {
                processASTNode(node->children[0], output);
                
                // ������� �����
                if (node->value == "-") {
                    output.push_back("u-"); // �������� ��� ������� �����
                }
                // ���������
                else if (node->value == "not") {
                    output.push_back("not");
                }
            }
            break;
            
        // �������� ���������
        case ASTNodeType::BinOp:
            if (node->children.size() >= 2) {
                processASTNode(node->children[0], output);
                processASTNode(node->children[1], output);
                output.push_back(node->value); // ��������
            }
            break;
            
        // ���������
        case ASTNodeType::Expression:
            for (const auto& child : node->children) {
                processASTNode(child, output);
            }
            break;
            
        default:
            // ������ ���� �����, ������� �� �������� ������ ���������
            break;
    }
}