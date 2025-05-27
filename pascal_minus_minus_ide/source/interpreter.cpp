
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
        // children[0] - нижняя граница (обычно целое число)
        // children[1] - верхняя граница (обычно целое число)
        // children[2] - тип элементов

        // Получаем нижнюю и верхнюю границы
        int lowerBound = 0;  // По умолчанию от 0
        int upperBound = 9;  // По умолчанию до 9 (10 элементов)

        // Пробуем оценить границы, если не получается - используем значения по умолчанию
        try {
            if (root->children[0]->type == ASTNodeType::Number) {
                lowerBound = std::stoi(root->children[0]->value);
            } else {
                Value lowerValue = evaluateExpression(root->children[0]);
                if (lowerValue.type == ValueType::Integer) {
                    lowerBound = lowerValue.intValue;
                }
            }

            if (root->children[1]->type == ASTNodeType::Number) {
                upperBound = std::stoi(root->children[1]->value);
            } else {
                Value upperValue = evaluateExpression(root->children[1]);
                if (upperValue.type == ValueType::Integer) {
                    upperBound = upperValue.intValue;
                }
            }
        } catch (...) {
            // В случае ошибки используем значения по умолчанию
            lowerBound = 0;
            upperBound = 9;
        }

        // Получаем тип элементов
        std::string elemType = "integer";  // По умолчанию целочисленный тип
        if (root->children.size() > 2 && root->children[2]->type == ASTNodeType::Identifier) {
            elemType = root->children[2]->value;
        }

        // Вычисляем размер массива
        int size = upperBound - lowerBound + 1;
        if (size <= 0) {
            size = 10;  // Если неверный размер, используем 10 элементов
        }
        if (size > 100) {
            size = 100;  // Ограничиваем максимальный размер 100 элементами
        }

        // Создаем массив и инициализируем его значениями по умолчанию
        std::vector<Value> array(size);
        if (elemType == "real" || elemType == "double") {
            for (int i = 0; i < size; i++)
                array[i] = Value(0.0);
        } else if (elemType == "boolean") {
            for (int i = 0; i < size; i++)
                array[i] = Value(false);
        } else if (elemType == "string") {
            for (int i = 0; i < size; i++)
                array[i] = Value("");
        } else {
            // По умолчанию целочисленный тип
            for (int i = 0; i < size; i++)
                array[i] = Value(0);
        }

        // Сохраняем массив в таблице символов
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
            throw std::runtime_error("Неизвестный массив или строка: " + arrayName);
        
        // Преобразуем индекс в целое число, даже если это float
        int idx = 0;
        if (index.type == ValueType::Integer) {
            idx = index.intValue;
        } else if (index.type == ValueType::Real) {
            idx = static_cast<int>(index.realValue);
        } else {
            throw std::runtime_error("Индекс должен быть числом");
        }
        
        // Обрабатываем доступ к элементам массива или символам строки
        if (it->second.type == ValueType::Array) {
            // В Pascal индексы массивов начинаются с 1, но в C++ с 0
            idx = idx - 1;  // Преобразуем индекс Pascal в индекс C++
            
            // Проверяем границы массива
            if (idx < 0 || idx >= it->second.arrayValue.size())
                throw std::runtime_error("Индекс массива вне диапазона: " + std::to_string(idx+1));
            
            return it->second.arrayValue[idx];
        }
        else if (it->second.type == ValueType::String) {
            // Доступ к символу строки
            // В Pascal индексы строк начинаются с 1, но в C++ с 0
            idx = idx - 1;  // Преобразуем индекс Pascal в индекс C++
            
            if (idx < 0 || idx >= it->second.strValue.length())
                throw std::runtime_error("Индекс строки вне диапазона: " + std::to_string(idx+1));
            
            // Возвращаем один символ как строку
            std::string charStr = std::string(1, it->second.strValue[idx]);
            return Value(charStr);
        }
        else {
            throw std::runtime_error("Ожидался массив или строка: " + arrayName);
        }
    }
    case ASTNodeType::Call: {
        // Обработка вызова встроенных функций
        const std::string& funcName = node->value;
        
        if (funcName == "length") {
            // Функция length для определения длины строки
            if (node->children.size() != 1)
                throw std::runtime_error("Функция length принимает только один аргумент");
            
            Value arg = evaluateExpression(node->children[0]);
            if (arg.type != ValueType::String)
                throw std::runtime_error("Функция length принимает только строковый аргумент");
            
            return Value(static_cast<int>(arg.strValue.length()));
        }
        else {
            throw std::runtime_error("Неизвестная функция: " + funcName);
        }
    }
    case ASTNodeType::BinOp: {
        Value left = evaluateExpression(node->children[0]);
        Value right = evaluateExpression(node->children[1]);
        const std::string& op = node->value;
        
        // Добавляем поддержку оператора 'in' для проверки наличия символа в множестве
        if (op == "in") {
            // Проверяем, что левый операнд - символ (строка длины 1)
            if (left.type != ValueType::String)
                throw std::runtime_error("Оператор 'in' ожидает символ слева");
            
            // Проверяем, есть ли символ в множестве
            std::string ch = left.strValue;
            if (ch.length() != 1)
                throw std::runtime_error("Ожидался один символ для оператора 'in'");
            
            // Создаем временный массив из правого операнда (Expression node)
            std::vector<Value> elements;
            for (const auto& child : node->children[1]->children) {
                elements.push_back(evaluateExpression(child));
            }
            
            bool found = false;
            for (const auto& item : elements) {
                if (item.type == ValueType::String && item.strValue.length() == 1 && item.strValue[0] == ch[0]) {
                    found = true;
                    break;
                }
            }
            
            return Value(found);
        }

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
    case ASTNodeType::Expression: {
        // Обработка узла Expression (используется для множеств в операторе in)
        if (node->value == "set") {
            // Создаем массив значений для множества оператора in
            std::vector<Value> elements;
            for (const auto& elem : node->children) {
                elements.push_back(evaluateExpression(elem));
            }
            return Value(elements);
        }
        // Другие типы выражений
        if (node->children.size() > 0) {
            return evaluateExpression(node->children[0]);
        }
        throw std::runtime_error("Пустое выражение");
    }
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
        
        // Преобразуем индекс в целое число, даже если это float
        int idx = 0;
        if (index.type == ValueType::Integer) {
            idx = index.intValue;
        } else if (index.type == ValueType::Real) {
            idx = static_cast<int>(index.realValue);
        } else {
            throw std::runtime_error("Индекс массива должен быть числом");
        }
        
        // В Pascal индексы массивов начинаются с 1, но в C++ с 0
        idx = idx - 1;  // Преобразуем индекс Pascal в индекс C++
        
        // Проверяем границы
        if (idx < 0 || idx >= it->second.arrayValue.size())
            throw std::runtime_error("Индекс массива вне диапазона: " + std::to_string(idx+1));
        
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
