#include "interpreter.h" // ������������ ���� � ��������� ������ Interpreter � ��������� Value
#include <iostream>      // ��� �����/������
#include <limits>        // ��� numeric_limits, ������������� � Readln
#include <cmath>
#include <algorithm>
#include <sstream>
#include "postfix.h"  // ��������� ��������� postfix.h � �������� ����
#include "logger.h"     // ��� �����������

// ���������� ������������� Value (�������������� ���������� ��������)
// ========================

// ����������� �� ���������: �������� ���� Integer, ������ 0
Value::Value() : type(ValueType::Integer), intValue(0), realValue(0.0), boolValue(false), stringValue("") {}

// ����������� ��� ������ ��������
Value::Value(int v) : type(ValueType::Integer), intValue(v), realValue(0.0), boolValue(false), stringValue("") {}

// ����������� ��� ������������� ��������
Value::Value(double v) : type(ValueType::Real), intValue(0), realValue(v), boolValue(false), stringValue("") {}

// ����������� ��� �������� ��������
Value::Value(bool v) : type(ValueType::Boolean), intValue(0), realValue(0.0), boolValue(v), stringValue("") {}

// ����������� ��� ���������� ��������
Value::Value(const std::string& v) : type(ValueType::String), intValue(0), realValue(0.0), boolValue(false), stringValue(v) {}

// ������ �������������� �����
int Value::toInt() const {
    switch (type) {
        case ValueType::Integer:
            return intValue;
        case ValueType::Real:
            return static_cast<int>(realValue);
        case ValueType::Boolean:
            return boolValue ? 1 : 0;
        case ValueType::String:
            try {
                return std::stoi(stringValue);
            } catch (const std::exception&) {
                throw std::runtime_error("���������� ������������� ������ \"" + stringValue + "\" � ����� �����");
            }
        default:
            throw std::runtime_error("���������������� ��� ��� �������������� � �����");
    }
}

double Value::toReal() const {
    switch (type) {
        case ValueType::Integer:
            return static_cast<double>(intValue);
        case ValueType::Real:
            return realValue;
        case ValueType::Boolean:
            return boolValue ? 1.0 : 0.0;
        case ValueType::String:
            try {
                return std::stod(stringValue);
            } catch (const std::exception&) {
                throw std::runtime_error("���������� ������������� ������ \"" + stringValue + "\" � ������������ �����");
            }
        default:
            throw std::runtime_error("���������������� ��� ��� �������������� � ������������");
    }
}

bool Value::toBool() const {
    switch (type) {
        case ValueType::Boolean:
            return boolValue;
        case ValueType::Integer:
            return intValue != 0;
        case ValueType::Real:
            return realValue != 0.0;
        case ValueType::String:
            return !stringValue.empty() && stringValue != "0" && stringValue != "false";
        default:
            throw std::runtime_error("���������������� ��� ��� �������������� � ����������");
    }
}

std::string Value::toString() const {
    std::ostringstream oss;
    switch (type) {
        case ValueType::String:
            return stringValue;
        case ValueType::Integer:
            oss << intValue;
            return oss.str();
        case ValueType::Real:
            oss << realValue;
            return oss.str();
        case ValueType::Boolean:
            return boolValue ? "true" : "false";
        default:
            throw std::runtime_error("���������������� ��� ��� �������������� � ������");
    }
}

// ========================
// �������������
// ========================

// ����������� �� ���������
Interpreter::Interpreter() : errorReporter(std::make_shared<ErrorReporter>()) {
    // ������ ����������� ����������� ��� ���������� ���������
    postfixCalculator = std::make_unique<PostfixCalculator>();
}

// ���������� ����������� ������� ��� ���������� ������
void Interpreter::reportError(const string& message, int line, int column) const {
    if (errorReporter) {
        errorReporter->reportError(message, line, column);
    }
}

void Interpreter::reportWarning(const string& message, int line, int column) const {
    if (errorReporter) {
        errorReporter->reportWarning(message, line, column);
    }
}

// ����������� � ��������� ����������� ������
Interpreter::Interpreter(std::shared_ptr<IErrorReporter> reporter) 
    : errorReporter(reporter ? reporter : std::make_shared<ErrorReporter>()), 
      postfixCalculator(std::make_unique<PostfixCalculator>()) {}
      
// �������� ������������� ����������
bool Interpreter::isDeclared(const std::string& name) const {
    return symbols.find(name) != symbols.end();
}

// ��������� �������� ����������
Value Interpreter::getVariable(const std::string& name) const {
    auto it = symbols.find(name);
    if (it == symbols.end()) {
        throw std::runtime_error("����������� ����������: " + name);
    }
    return it->second;
}

// ��������� �������� ����������
void Interpreter::setVariable(const std::string& name, const Value& value) {
    symbols[name] = value;
}

// ������� ���� ��������
void Interpreter::clearSymbols() {
    symbols.clear();
}

// ������ ��������� �� ������ (������������ �����)
Value Interpreter::evaluate(const std::string& expression) {
    LOG_INFO("Evaluating expression: " + expression);
    // ����� ������ ���� ��� �������� ������ � AST
    // ��� �������� ������ ��������
    Value result;
    result.type = ValueType::String;
    result.stringValue = "Not implemented: " + expression;
    return result;
}

/**
 * ��������� �������� ���������, ��������������� ����� AST
 * @param expression ���� AST, �������������� ���������
 * @return ��������� ���������� ���������
 */
Value Interpreter::evaluateExpression(const std::shared_ptr<ASTNode>& expression) {
    // ���������� ���������� ������ evaluateUsingPostfix, ������� ���������� PostfixCalculator
    return evaluateUsingPostfix(expression);
}


/**
 * ������� ����� ������������� ��������� Pascal--
 * ���������� ������������ ���� AST, ������� � ��������� ����
 * @param root �������� ���� AST ���������
 */
void Interpreter::run(const std::shared_ptr<ASTNode>& root) {
    if (!root) {
        reportWarning("������ ���������");
        return; // ���� ���� ������ � ������ �� ������
    }
    
#ifdef ENABLE_LOGGING
    LOG_INFO("������ ���������� ���������");
#endif
    switch (root->type) {
    case ASTNodeType::ConstDecl: {
        const std::string& name = root->value;
        const std::string& typeName = root->children[0]->value;
        // ���������� ����������� ����������� ��� ���������� ���������
        Value val = evaluateUsingPostfix(root->children[1]);
        
        LOG_DEBUG("���������� ��������� " + name + " ���� " + typeName);
        
        if (typeName == "real" || typeName == "double")
            symbols[name] = Value(val.realValue);
        else if (typeName == "integer")
            symbols[name] = Value(val.intValue);
        else if (typeName == "boolean")
            symbols[name] = Value(val.boolValue);
        else if (typeName == "string")
            symbols[name] = Value(val.stringValue);
        else
            throw std::runtime_error("����������� ��� ���������: " + typeName);
        break;
    }
    case ASTNodeType::VarDecl: {
        try {
            const std::string& name = root->value;
            const std::string& typeName = root->children[0]->value;
            
            LOG_DEBUG("���������� ���������� " + name + " ���� " + typeName);
            
            // ������� ���������� � ������� ��������� ���������������� ����
            if (typeName == "real" || typeName == "double") {
                symbols[name] = Value(0.0);
            } else if (typeName == "integer") {
                symbols[name] = Value(0);
            } else if (typeName == "boolean") {
                symbols[name] = Value(false);
            } else if (typeName == "string") {
                symbols[name] = Value("");
            } else {
                reportError("����������� ��� ����������: " + typeName);
                throw std::runtime_error("����������� ��� ����������: " + typeName);
            }
        } catch (const std::exception& e) {
            reportError(std::string("������ ��� ���������� ����������: ") + e.what());
            throw; // ������������� ���������� ������
        }
        break;
    }
    case ASTNodeType::Program:
    case ASTNodeType::Block:
    case ASTNodeType::ConstSection:
    case ASTNodeType::VarSection:
        for (const auto& stmt : root->children)
            run(stmt);
        break;
    case ASTNodeType::Assignment:
        executeAssignment(root);
        break;
    case ASTNodeType::If:
        executeIf(root);
        break;
    case ASTNodeType::While:
        executeWhile(root);
        break;
    case ASTNodeType::ForLoop:
        executeFor(root);
        break;
    case ASTNodeType::Write:
    case ASTNodeType::Writeln:
        executeWrite(root);
        break;
    case ASTNodeType::Read:
    case ASTNodeType::Readln:
        executeRead(root);
        break;
    case ASTNodeType::Number:
    case ASTNodeType::Real:
    case ASTNodeType::Boolean:
    case ASTNodeType::String:
    case ASTNodeType::Identifier:
    case ASTNodeType::BinOp:
    case ASTNodeType::UnOp:
        // ��� ���� ������ ������������ ������ � ����������, �� ���� ����������� ����� � ������ �� ������
        break;
    // ������� ���������� �������� � ������� (Call, Return)
    case ASTNodeType::Expression:
    default:
        std::cerr << "����������� �������� ����: " << static_cast<int>(root->type) << std::endl;
        throw std::runtime_error("����������� ��������");
    }
}


// ===== ���������� executeFor =====
// ���������� executeFor ��� ������ ������ Interpreter
void Interpreter::executeFor(const std::shared_ptr<ASTNode>& node) {
    try {
        LOG_DEBUG("���������� ����� for");
        
        // node->value �������� ��� ���������� �, ��������, "|downto"
        std::string varName = node->value;
        bool isDownto = false;
        
        // ���������, ���� ��� ���� � ��������� (downto)
        size_t pipePos = varName.find('|');
        if (pipePos != std::string::npos) {
            isDownto = (varName.substr(pipePos + 1) == "downto");
            varName = varName.substr(0, pipePos);
        }
        
        // ���������� ����������� ����������� ��� ���������� ���������� � ��������� ��������
        Value fromVal = evaluateUsingPostfix(node->children[0]);
        Value toVal = evaluateUsingPostfix(node->children[1]);
        auto& body = node->children[2];
        
        LOG_DEBUG("���� for " + varName + (isDownto ? " downto" : " to") + 
        " �� " + std::to_string(fromVal.toInt()) + " �� " + std::to_string(toVal.toInt()));

        // ��������, ��� �������� from � to ����� ������������� � ����� �����
        if (fromVal.type != ValueType::Integer) {
            if (fromVal.type == ValueType::Real) {
                reportWarning("�������� ���� Real ����� ������������� � ����� ��� ����� for");
                fromVal = Value(static_cast<int>(fromVal.realValue));
            } else {
                reportError("��������� �������� ����� for ������ ���� ��������");
                throw std::runtime_error("��������� �������� ����� for ������ ���� ��������");
            }
        }
        
        if (toVal.type != ValueType::Integer) {
            if (toVal.type == ValueType::Real) {
                reportWarning("�������� �������� ���� Real ����� ������������� � ����� ��� ����� for");
                toVal = Value(static_cast<int>(toVal.realValue));
            } else {
                reportError("�������� �������� ����� for ������ ���� ��������");
                throw std::runtime_error("�������� �������� ����� for ������ ���� ��������");
            }
        }

        // ��������� ������� �������� ���������� �����, ���� ��� ����������
        Value oldValue;
        bool varExists = isDeclared(varName);
        if (varExists) {
            oldValue = getVariable(varName);
        }

        // ������� �������� ��� ������ �� ����������� ������
        int iterations = 0;
        const int MAX_ITERATIONS = 10000; // ������������ ����� ��������
        
        // ���������� ����� ��� ��������������� �������������� ���������� ��� ������ �� �������
        class VariableRestorer {
        public:
            VariableRestorer(std::map<std::string, Value>& symbols, const std::string& name, bool exists, const Value& oldVal) 
                : symbols_(symbols), name_(name), exists_(exists), oldVal_(oldVal) {}
            
            ~VariableRestorer() {
                if (exists_) {
                    symbols_[name_] = oldVal_;
                } else {
                    symbols_.erase(name_);
                }
            }
        private:
            std::map<std::string, Value>& symbols_;
            std::string name_;
            bool exists_;
            Value oldVal_;
        };
        
        // ������� ������ ��� ��������������� �������������� ����������
        VariableRestorer restorer(symbols, varName, varExists, oldValue);
        
        try {
            // ��������� ���� � ����������� �� ����������� (to ��� downto)
            if (isDownto) {
                for (int i = fromVal.intValue; i >= toVal.intValue; --i) {
                    symbols[varName] = Value(i);
                    run(body);
                    
                    // �������� �� ����������� ����
                    iterations++;
                    if (iterations > MAX_ITERATIONS) {
                        reportWarning("��������� ����������� ���� for downto (��������� ������������ ����� ��������)");
                        break;
                    }
                }
            } else {
                for (int i = fromVal.intValue; i <= toVal.intValue; ++i) {
                    symbols[varName] = Value(i);
                    run(body);
                    
                    // �������� �� ����������� ����
                    iterations++;
                    if (iterations > MAX_ITERATIONS) {
                        reportWarning("��������� ����������� ���� for to (��������� ������������ ����� ��������)");
                        break;
                    }
                }
            }
            
            LOG_DEBUG("���������� ����� for ����� " + std::to_string(iterations) + " ��������");
        } 
        catch (const std::exception& e) {
            reportError(std::string("������ ��� ���������� ����� for: ") + e.what());
            throw; // ������������� ���������� ������
        }
        // ��� ������������� ���� ��������������� ����������, ��� ������� ���������� VariableRestorer
    } 
    catch (const std::exception& e) {
        reportError(std::string("������ � ����� for: ") + e.what());
        throw; // ������������� ���������� ������
    }
}

// ����� evaluateExpression ���������� � ������ ����� �����

void Interpreter::executeAssignment(const shared_ptr<ASTNode>& node) {
    try {
        // ������� ������������ ����������
        std::string varName = node->children[0]->value;
        
        // ���������� ����������� ����� ��� ���������� ���������
        Value value = evaluateUsingPostfix(node->children[1]);
        
        // �������� �������� ������������
        LOG_DEBUG("������������ ���������� " + varName);
        
        // ��������� ����� ��������
        symbols[varName] = value;
    } catch (const std::exception& e) {
        reportError(std::string("������ ��� ���������� ������������: ") + e.what());
        throw; // ������������� ���������� ��� ��������� �� ������� ������
    }
}

void Interpreter::executeIf(const shared_ptr<ASTNode>& node) {
    try {
        // ��������� ������� � �������������� ����������� �����
        Value cond = evaluateUsingPostfix(node->children[0]);
        
        // ���������, ��� ������� ����� ������� ���
        if (cond.type != ValueType::Boolean) {
            reportWarning("������� � ��������� if ������ ���� ����������� ����");
            cond = Value(cond.toBool()); // ����������� � �������� ����
        }
        
        LOG_DEBUG("���������� ��������� ��������� if, �������: " + std::string(cond.boolValue ? "true" : "false"));
        
        // ��������� ��������������� �����
        if (cond.boolValue)
            run(node->children[1]); // then ����
        else if (node->children.size() > 2)
            run(node->children[2]); // else ���� (���� ����)
    } catch (const std::exception& e) {
        reportError(std::string("������ ��� ���������� ��������� ���������: ") + e.what());
    }
}

void Interpreter::executeWhile(const shared_ptr<ASTNode>& node) {
    try {
        LOG_DEBUG("������ ���������� ����� while");
        
        // ������� �������� ��� ������ �� ����������� ������
        int iterations = 0;
        const int MAX_ITERATIONS = 10000; // ������������ ����� ��������
        
        while (true) {
            // ��������� ������� � �������������� ����������� �����
            Value cond = evaluateUsingPostfix(node->children[0]);
            
            // ���������, ��� ������� ����� ������� ���
            if (cond.type != ValueType::Boolean) {
                reportWarning("������� � ��������� while ������ ���� ����������� ����");
                cond = Value(cond.toBool()); // ����������� � �������� ����
            }
            
            // ���� ������� �� ���������, ������� �� �����
            if (!cond.boolValue)
                break;
                
            // ��������� ���� �����
            run(node->children[1]);
            
            // �������� �� ����������� ����
            iterations++;
            if (iterations > MAX_ITERATIONS) {
                reportWarning("��������� ����������� ���� while (��������� ������������ ����� ��������)");
                break;
            }
        }
        
        LOG_DEBUG("���������� ����� while ����� " + std::to_string(iterations) + " ��������");
    } catch (const std::exception& e) {
        reportError(std::string("������ ��� ���������� ����� while: ") + e.what());
    }
}

void Interpreter::executeWrite(const shared_ptr<ASTNode>& node) {
    try {
        LOG_DEBUG("���������� ��������� write/writeln");
        
        for (size_t i = 0; i < node->children.size(); ++i) {
            // ���������� ����������� ����������� ��� ���������� ���������
            Value val = evaluateUsingPostfix(node->children[i]);
            
            // ������� �������� � ����������� �� ��� ����
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
                cout << val.stringValue; 
                break;
            default:
                reportWarning("���������������� ��� ������ ��� ������");
                cout << "[����������� ���]";
                break;
            }
            
            // ��������� ������ ����� ����������
            if (i + 1 < node->children.size()) 
                cout << " ";
        }
    } catch (const std::exception& e) {
        reportError(std::string("������ ��� ���������� write/writeln: ") + e.what());
    }
    if (node->type == ASTNodeType::Writeln) cout << endl;
}

void Interpreter::executeRead(const shared_ptr<ASTNode>& node) {
    try {
        LOG_DEBUG("���������� ��������� read/readln");
        
        for (const auto& child : node->children) {
            // �������� ��� ����������
            string varName = child->value;
            
            // ���������, ��� ���������� ����������
            if (!isDeclared(varName)) {
                reportError("������� ������ � ������������� ����������: " + varName);
                continue;
            }
            
            // ���������� ��� ����������
            ValueType varType = getValueType(varName);
            
            // ������ �������� � ����������� �� ���� ����������
            switch (varType) {
                case ValueType::Integer: {
                    int v;
                    if (cin >> v) {
                        symbols[varName] = Value(v);
                    } else {
                        reportError("������ ��� ������ ������ �����");
                        cin.clear(); // ���������� ��������� ������
                    }
                    break;
                }
                case ValueType::Real: {
                    double v;
                    if (cin >> v) {
                        symbols[varName] = Value(v);
                    } else {
                        reportError("������ ��� ������ ������������� �����");
                        cin.clear(); // ���������� ��������� ������
                    }
                    break;
                }
                case ValueType::Boolean: {
                    string input;
                    if (cin >> input) {
                        // ��������������� ��������� ����� � ������ ��������
                        transform(input.begin(), input.end(), input.begin(), ::tolower);
                        bool value = (input == "true" || input == "1" || input == "yes");
                        symbols[varName] = Value(value);
                    } else {
                        reportError("������ ��� ������ ����������� ��������");
                        cin.clear(); // ���������� ��������� ������
                    }
                    break;
                }
                case ValueType::String: {
                    string v;
                    if (cin >> v) {
                        symbols[varName] = Value(v);
                    } else {
                        reportError("������ ��� ������ ������");
                        cin.clear(); // ���������� ��������� ������
                    }
                    break;
                }
                default: {
                    reportError("���������������� ��� ���������� ��� �����: " + varName);
                    break;
                }
            }
        }
        
        // ��� readln ���������� ������� ������
        if (node->type == ASTNodeType::Readln) {
            LOG_DEBUG("������ �� ����� ������ (readln)");
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
    } catch (const std::exception& e) {
        reportError(std::string("������ ��� ���������� read/readln: ") + e.what());
        cin.clear(); // ���������� ��������� ������
        cin.ignore(numeric_limits<streamsize>::max(), '\n'); // ���������� ������� ������
    }
}

// ������������ ������ ������ getVarValue ��� ������� �������� � ������� const ������
int Interpreter::getVarValue(const string& name) const {
    try {
        // ��������� ������������� ����������
        auto it = symbols.find(name);
        if (it == symbols.end()) {
            reportError("���������� �� �������: " + name, 0, 0);  // ��������� ��������� line � column
            throw std::runtime_error("���������� �� �������: " + name);
        }
        
        // ���� ���������� ������ ����, ���������� � ��������
        if (it->second.type == ValueType::Integer) {
            return it->second.intValue;
        } else {
            // ��� ������ ����� �������� ������������� � ������
            return it->second.toInt();
        }
    } catch (const std::exception& e) {
        LOG_ERROR(std::string("������ ��� ��������� �������� ����������: ") + e.what());
        throw; // ������������� ���������� ������
    }
}


// ������������ ������ ������ getValueType ��� ������� �������� � ������� const ������
ValueType Interpreter::getValueType(const string& name) const {
    try {
        // ��������� ������������� ���������� � ������� ��������
        auto it = symbols.find(name);
        if (it == symbols.end()) {
            // ���� ���������� �� �������, ���������� ������ � ��������� ������� ���������
            reportError("���������� �� �������: " + name, 0, 0);
            throw std::runtime_error("���������� �� �������: " + name);
        }
        
        // ���������� ��� ���������� �� ������� ��������
        return it->second.type;
    } catch (const std::exception& e) {
        // �������� ������ � ������������� ���������� ������
        LOG_ERROR(std::string("������ ��� ����������� ���� ����������: ") + e.what());
        throw; // ������������� ���������� ������
    }
}


// ������������ ������ ������ evaluateUsingPostfix ��� ���������� ���������
/**
 * ��������� �������� ���������, ��������������� ����� AST, ��������� ����������� �����������
 * @param node ���� AST, �������������� ���������
 * @return �������� ���������
 */
Value Interpreter::evaluateUsingPostfix(const std::shared_ptr<ASTNode>& node) {
    try {
        // ���������� ����� evaluate �� ���������� IPostfixCalculator
        // �������� ���� � ����������� � �����������
        if (postfixCalculator) {
            // �������� ������� �������� � ����������� ��� ������� � ����������
            const std::map<std::string, Value>& vars = symbols;
            return postfixCalculator->evaluate(node, vars);
        } else {
            // ������, ���� ����������� �� ���������������
            throw std::runtime_error("PostfixCalculator not initialized");
        }
    } catch (const std::exception& e) {
        // ������������ � �������� ������, ���������� �������� �� ���������
        reportError(std::string("������ ���������� ���������: ") + e.what(), 0, 0);
        return Value(); // ���������� �������� �� ��������� (0)
    }
}

