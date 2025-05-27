
#include "interpreter.h" // Заголовочный файл с описанием класса Interpreter и структуры Value
#include <iostream>      // Для ввода/вывода
#include <limits>        // Для numeric_limits, используемого в Readln
#include <cmath>
#include <algorithm>

// Реализация конструкторов Value (универсального контейнера значений)
// ========================

// Конструктор по умолчанию: значение типа Integer, равное 0
Value::Value() : type(ValueType::Integer), intValue(0), realValue(0.0), boolValue(false), strValue("") {}

// Конструктор для целого значения
Value::Value(int v) : type(ValueType::Integer), intValue(v), realValue(0.0), boolValue(false), strValue("") {}

// Конструктор для вещественного значения
Value::Value(double v) : type(ValueType::Real), intValue(0), realValue(v), boolValue(false), strValue("") {}

// Конструктор для булевого значения
Value::Value(bool v) : type(ValueType::Boolean), intValue(0), realValue(0.0), boolValue(v), strValue("") {}

// Конструктор для строкового значения
Value::Value(const std::string& v) : type(ValueType::String), intValue(0), realValue(0.0), boolValue(false), strValue(v) {}

// Конструктор для массива
Value::Value(const std::vector<Value>& arr) : type(ValueType::Array), intValue(0), realValue(0.0), boolValue(false), strValue(""), arrayValue(arr) {}

// ========================
// Интерпретатор
// ========================

Interpreter::Interpreter() = default; // Конструктор по умолчанию

// Запуск интерпретации программы
void Interpreter::run(const std::shared_ptr<ASTNode>& root) {
    if (!root)
        return; // Если узел пустой — ничего не делаем
    switch (root->type) {
    case ASTNodeType::ConstDecl: {
        const std::string& name = root->value;
        const std::string& typeName = root->children[0]->value;
        Value val = evaluateExpression(root->children[1]);
        if (typeName == "real" || typeName == "double")
            symbols[name] = Value(val.realValue);
        else if (typeName == "integer")
            symbols[name] = Value(val.intValue);
        else if (typeName == "boolean")
            symbols[name] = Value(val.boolValue);
        else if (typeName == "string")
            symbols[name] = Value(val.strValue);
        else
            throw std::runtime_error("Неизвестный тип константы: " + typeName);
        break;
    }
    case ASTNodeType::VarDecl: {
        const std::string& name = root->value;
        const std::string& typeName = root->children[0]->value;
        if (typeName == "real" || typeName == "double")
            symbols[name] = Value(0.0);
        else
            symbols[name] = Value(0);
        break;
    }
    case ASTNodeType::ArrayDecl: {
        const std::string& name = root->value;
        // В парсере структура узла ArrayDecl:
        // children[0] - нижняя граница
        // children[1] - верхняя граница
        // children[2] - тип элементов
        Value lowerValue = evaluateExpression(root->children[0]);
        Value upperValue = evaluateExpression(root->children[1]);
        
        int lowerBound, upperBound;
        if (lowerValue.type == ValueType::Integer) {
            lowerBound = lowerValue.intValue;
        } else {
            throw std::runtime_error("Нижняя граница массива должна быть целым числом");
        }
        
        if (upperValue.type == ValueType::Integer) {
            upperBound = upperValue.intValue;
        } else {
            throw std::runtime_error("Верхняя граница массива должна быть целым числом");
        }
        const std::string& elemType = root->children[2]->value;
        
        int size = upperBound - lowerBound + 1;
        if (size <= 0)
            throw std::runtime_error("Размер массива должен быть положительным");
        
        std::vector<Value> array(size);
        if (elemType == "real" || elemType == "double") {
            for (int i = 0; i < size; i++)
                array[i] = Value(0.0);
        } else if (elemType == "integer") {
            for (int i = 0; i < size; i++)
                array[i] = Value(0);
        } else if (elemType == "boolean") {
            for (int i = 0; i < size; i++)
                array[i] = Value(false);
        } else if (elemType == "string") {
            for (int i = 0; i < size; i++)
                array[i] = Value("");
        } else {
            throw std::runtime_error("Неизвестный тип массива: " + elemType);
        }
        
        symbols[name] = Value(array);
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
    case ASTNodeType::ProcCall: {
        const std::string& name = root->value;
        if (name == "Writeln" || name == "Write") {
            for (size_t i = 0; i < root->children.size(); ++i) {
                Value val = evaluateExpression(root->children[i]);
                switch (val.type) {
                case ValueType::Integer: std::cout << val.intValue; break;
                case ValueType::Real: std::cout << val.realValue; break;
                case ValueType::Boolean: std::cout << (val.boolValue ? "true" : "false"); break;
                case ValueType::String: std::cout << val.strValue; break;
                }
                if (name == "Writeln")
                    std::cout << std::endl;
            }
        }
        else
            throw std::runtime_error("Неизвестная процедура: " + name);
        break;
    }
    case ASTNodeType::Number:
    case ASTNodeType::Real:
    case ASTNodeType::Boolean:
    case ASTNodeType::String:
    case ASTNodeType::Identifier:
    case ASTNodeType::ArrayAccess:
    case ASTNodeType::BinOp:
    case ASTNodeType::UnOp:
        // Эти узлы обычно используются только в выражениях, но если встретились здесь — ничего не делаем
        break;
    case ASTNodeType::ProcedureDecl:
    case ASTNodeType::FunctionDecl:
    case ASTNodeType::Call:
    case ASTNodeType::Return:
    case ASTNodeType::Expression:
    case ASTNodeType::DotDot:
        // Если требуется, добавьте обработку этих узлов здесь
        break;
    default:
        std::cerr << "Неизвестный оператор типа: " << static_cast<int>(root->type) << std::endl;
        throw std::runtime_error("Неизвестный оператор");
    }
}


// ===== Реализация executeFor =====
// Реализация executeFor как метода класса Interpreter
void Interpreter::executeFor(const std::shared_ptr<ASTNode>& node) {
    // node->value содержит имя переменной и, возможно, "|downto"
    std::string varName = node->value;
    bool isDownto = false;
    size_t pipePos = varName.find('|');
    if (pipePos != std::string::npos) {
        isDownto = (varName.substr(pipePos + 1) == "downto");
        varName = varName.substr(0, pipePos);
    }

        Value fromVal = this->evaluateExpression(node->children[0]);
    Value toVal = this->evaluateExpression(node->children[1]);
    auto& body = node->children[2];

    Value oldValue;
    bool varExists = (this->symbols.find(varName) != this->symbols.end());
    if (varExists) {
        oldValue = this->symbols[varName];
    }

    try {
        this->symbols[varName] = fromVal;

        if (isDownto) {
            for (int i = fromVal.intValue; i >= toVal.intValue; --i) {
                this->symbols[varName] = Value(i);
                this->run(body);
            }
        } else {
            for (int i = fromVal.intValue; i <= toVal.intValue; ++i) {
                this->symbols[varName] = Value(i);
                this->run(body);
            }
        }

        if (varExists) {
            this->symbols[varName] = oldValue;
        } else {
            this->symbols.erase(varName);
        }
    } catch (...) {
        if (varExists) {
            this->symbols[varName] = oldValue;
        } else {
            this->symbols.erase(varName);
        }
        throw;
    }
}

Value Interpreter::evaluateExpression(const shared_ptr<ASTNode>& node) {
    switch (node->type) {
    case ASTNodeType::Number:
        return Value(stoi(node->value));
    case ASTNodeType::Real:
        return Value(stod(node->value));
    case ASTNodeType::Boolean:
        return Value(node->value == "true");
    case ASTNodeType::String:
        return Value(node->value);
    case ASTNodeType::Identifier: {
        auto it = symbols.find(node->value);
        if (it == symbols.end())
            throw std::runtime_error("Неизвестная переменная: " + node->value);
        return it->second;
    }
    case ASTNodeType::ArrayAccess: {
        std::string arrayName = node->value;
        Value index = evaluateExpression(node->children[0]);
        auto it = symbols.find(arrayName);
        if (it == symbols.end())
            throw std::runtime_error("Неизвестный массив: " + arrayName);
        if (it->second.type != ValueType::Array)
            throw std::runtime_error("Ожидался массив: " + arrayName);
        if (index.type != ValueType::Integer)
            throw std::runtime_error("Индекс массива должен быть целым числом");
        int idx = index.intValue;
        if (idx < 0 || idx >= it->second.arrayValue.size())
            throw std::runtime_error("Индекс массива вне диапазона: " + std::to_string(idx));
        return it->second.arrayValue[idx];
    }
    case ASTNodeType::BinOp: {
        Value left = evaluateExpression(node->children[0]);
        Value right = evaluateExpression(node->children[1]);
        const std::string& op = node->value;

        if (op == "div") {
            if (left.type != ValueType::Integer || right.type != ValueType::Integer)
                throw std::runtime_error("Оператор div применим только к целым");
            if (right.intValue == 0)
                throw std::runtime_error("Деление на ноль (div)");
            return Value(left.intValue / right.intValue);
        }
        if (op == "mod") {
            if (left.type != ValueType::Integer || right.type != ValueType::Integer)
                throw std::runtime_error("Оператор mod применим только к целым");
            if (right.intValue == 0)
                throw std::runtime_error("Деление на ноль (mod)");
            return Value(left.intValue % right.intValue);
        }
        if (op == "+") {
            if (left.type == ValueType::String || right.type == ValueType::String) {
                std::string l = (left.type == ValueType::String) ? left.strValue : std::to_string(left.intValue);
                std::string r = (right.type == ValueType::String) ? right.strValue : std::to_string(right.intValue);
                return Value(l + r);
            }
            if (left.type == ValueType::Real || right.type == ValueType::Real)
                return Value(toReal(left) + toReal(right));
            return Value(left.intValue + right.intValue);
        }
        if (op == "-") {
            if (left.type == ValueType::Real || right.type == ValueType::Real)
                return Value(toReal(left) - toReal(right));
            return Value(left.intValue - right.intValue);
        }
        if (op == "*") {
            if (left.type == ValueType::Real || right.type == ValueType::Real)
                return Value(toReal(left) * toReal(right));
            return Value(left.intValue * right.intValue);
        }
        if (op == "/") {
            double l = toReal(left), r = toReal(right);
            if (r == 0.0)
                throw std::runtime_error("Деление на ноль");
            return Value(l / r);
        }
        if (op == "and") {
            if (left.type != ValueType::Boolean || right.type != ValueType::Boolean)
                throw std::runtime_error("Оператор and применим только к булевым значениям");
            return Value(left.boolValue && right.boolValue);
        }
        if (op == "or") {
            if (left.type != ValueType::Boolean || right.type != ValueType::Boolean)
                throw std::runtime_error("Оператор or применим только к булевым значениям");
            return Value(left.boolValue || right.boolValue);
        }
        if (op == "=") return Value(toReal(left) == toReal(right));
        if (op == "<>") return Value(toReal(left) != toReal(right));
        if (op == "<") return Value(toReal(left) < toReal(right));
        if (op == "<=") return Value(toReal(left) <= toReal(right));
        if (op == ">") return Value(toReal(left) > toReal(right));
        if (op == ">=") return Value(toReal(left) >= toReal(right));

        throw std::runtime_error("Неизвестный бинарный оператор: " + op);
    }
    case ASTNodeType::UnOp:
        if (node->value == "-") {
            Value v = evaluateExpression(node->children[0]);
            if (v.type == ValueType::Real) return Value(-v.realValue);
            if (v.type == ValueType::Integer) return Value(-v.intValue);
            throw std::runtime_error("Унарный минус применим только к числам");
        }
        if (node->value == "not") {
            Value v = evaluateExpression(node->children[0]);
            if (v.type != ValueType::Boolean)
                throw std::runtime_error("Оператор not применим только к булевым значениям");
            return Value(!v.boolValue);
        }
        throw std::runtime_error("Неизвестный унарный оператор: " + node->value);
    default:
        throw std::runtime_error(
            "Ошибка вычисления выражения. Тип узла: " +
            std::to_string(static_cast<int>(node->type)) +
            ", значение: " + node->value
        );
    }
}

void Interpreter::executeAssignment(const shared_ptr<ASTNode>& node) {
    if (node->children[0]->type == ASTNodeType::ArrayAccess) {
        // Присваивание элементу массива: arr[i] := value
        std::string arrayName = node->children[0]->value;
        Value index = evaluateExpression(node->children[0]->children[0]);
        Value value = evaluateExpression(node->children[1]);
        
        auto it = symbols.find(arrayName);
        if (it == symbols.end())
            throw std::runtime_error("Неизвестный массив: " + arrayName);
        if (it->second.type != ValueType::Array)
            throw std::runtime_error("Ожидался массив: " + arrayName);
        if (index.type != ValueType::Integer)
            throw std::runtime_error("Индекс массива должен быть целым числом");
        int idx = index.intValue;
        if (idx < 0 || idx >= it->second.arrayValue.size())
            throw std::runtime_error("Индекс массива вне диапазона: " + std::to_string(idx));
        it->second.arrayValue[idx] = value;
    } else {
        // Обычное присваивание переменной
        std::string varName = node->children[0]->value;
        Value value = evaluateExpression(node->children[1]);
        symbols[varName] = value;
    }
}

void Interpreter::executeIf(const shared_ptr<ASTNode>& node) {
    Value cond = evaluateExpression(node->children[0]);
    if (cond.boolValue)
        run(node->children[1]);
    else if (node->children.size() > 2)
        run(node->children[2]);
}

void Interpreter::executeWhile(const shared_ptr<ASTNode>& node) {
    while (evaluateExpression(node->children[0]).boolValue)
        run(node->children[1]);
}

void Interpreter::executeWrite(const shared_ptr<ASTNode>& node) {
    for (size_t i = 0; i < node->children.size(); ++i) {
        Value val = evaluateExpression(node->children[i]);
        switch (val.type) {
        case ValueType::Integer: cout << val.intValue; break;
        case ValueType::Real: cout << val.realValue; break;
        case ValueType::Boolean: cout << (val.boolValue ? "true" : "false"); break;
        case ValueType::String: cout << val.strValue; break;
        case ValueType::Array:
            cout << "[";
            for (size_t j = 0; j < val.arrayValue.size(); ++j) {
                switch (val.arrayValue[j].type) {
                case ValueType::Integer: cout << val.arrayValue[j].intValue; break;
                case ValueType::Real: cout << val.arrayValue[j].realValue; break;
                case ValueType::Boolean: cout << (val.arrayValue[j].boolValue ? "true" : "false"); break;
                case ValueType::String: cout << val.arrayValue[j].strValue; break;
                case ValueType::Array: cout << "[Array]"; break;
                }
                if (j + 1 < val.arrayValue.size()) cout << ", ";
            }
            cout << "]";
            break;
        }
        if (i + 1 < node->children.size()) cout << " ";
    }
    if (node->type == ASTNodeType::Writeln) cout << endl;
}

void Interpreter::executeRead(const shared_ptr<ASTNode>& node) {
    for (const auto& child : node->children) {
        string varName = child->value;
        if (getValueType(varName) == ValueType::Real) {
            double v; cin >> v;
            symbols[varName] = Value(v);
        }
        else {
            int v; cin >> v;
            symbols[varName] = Value(v);
        }
    }
    if (node->type == ASTNodeType::Readln)
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

int Interpreter::getVarValue(const string& name) const {
    auto it = symbols.find(name);
    if (it == symbols.end())
        throw runtime_error("Переменная не найдена: " + name);
    return it->second.intValue;
}

ValueType Interpreter::getValueType(const string& name) const {
    auto it = symbols.find(name);
    if (it == symbols.end())
        throw runtime_error("Переменная не найдена: " + name);
    return it->second.type;
}

double Interpreter::toReal(const Value& v) const {
    if (v.type == ValueType::Real) return v.realValue;
    if (v.type == ValueType::Integer) return static_cast<double>(v.intValue);
    throw runtime_error("Ожидалось числовое значение");
}
