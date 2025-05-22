#include "interpreter.h" // Заголовочный файл с описанием класса Interpreter и структуры Value
#include <iostream>      // Для ввода/вывода
#include <limits>        // Для numeric_limits, используемого в Readln
#include <cmath>
#include <algorithm>

// ========================
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
Value::Value(const string& v) : type(ValueType::String), intValue(0), realValue(0.0), boolValue(false), strValue(v) {}

// ========================
// Интерпретатор
// ========================

Interpreter::Interpreter() = default; // Конструктор по умолчанию

// Запуск интерпретации программы
void Interpreter::run(const shared_ptr<ASTNode>& root) {
    if (!root)
        return; // Если узел пустой — ничего не делаем
    switch (root->type) {
    case ASTNodeType::ConstDecl: {
        // root->value — имя константы; child[0] — её выражение
        const string& name = root->value;
        const string& typeName = root->children[0]->value;
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
            throw runtime_error("Неизвестный тип константы: " + typeName);
        break;
    }
    case ASTNodeType::VarDecl: {
        const string& name = root->value;                  // root->value — имя переменной
        const string& typeName = root->children[0]->value; // Тип лежит в первом дочернем узле
        if (typeName == "real" || typeName == "double")
            symbols[name] = Value(0.0);
        else
            symbols[name] = Value(0);
        break;
    }
    case ASTNodeType::Program:
    case ASTNodeType::Block:
    case ASTNodeType::ConstSection:
    case ASTNodeType::VarSection:
        // Последовательно выполняем все подузлы (операторы)
        for (const auto& stmt : root->children)
            run(stmt);
        break;
    case ASTNodeType::Assignment:
        executeAssignment(root); // Присваивание переменной
        break;
    case ASTNodeType::If:
        executeIf(root); // Условный оператор
        break;
    case ASTNodeType::While:
        executeWhile(root); // Цикл while
        break;
    case ASTNodeType::Write:
    case ASTNodeType::Writeln:
        executeWrite(root); // Вывод
        break;
    case ASTNodeType::Read:
    case ASTNodeType::Readln:
        executeRead(root); // Ввод
        break;

    case ASTNodeType::ProcCall: {
        // Процедурный вызов: возможно write/writeln/read/readln
        const string& name = root->value;
        if (name == "Writeln") {
            // повторяем логику executeWrite + переход на новую строку
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
            // логика executeWrite без переноса строки
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
        else if (name == "Read" || name == "Readln") {
            // аналог executeRead/execureReadln
            for (const auto& child : root->children) {
                string varName = child->value;
                if (getValueType(varName) == ValueType::Real) {
                    double v;
                    cin >> v;
                    symbols[varName] = Value(v);
                }
                else {
                    int v;
                    cin >> v;
                    symbols[varName] = Value(v);
                }
            }
            if (name == "Readln")
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
            break;
        }
        else
            throw runtime_error("Неизвестная процедура: " + name);
    }
    default:
        throw runtime_error("Неизвестный оператор");
    }
}

// Вычисление выражения
Value Interpreter::evaluateExpression(const shared_ptr<ASTNode>& node) {
    switch (node->type) {
    case ASTNodeType::Number:
        return Value(stoi(node->value)); // Преобразуем строку в число
    case ASTNodeType::Real: {
        double val = stod(node->value);
    cout << "[DEBUG evaluateExpression::Real] node->value=" << node->value << " parsed=" << val << endl;
    return Value(val);
    }
    case ASTNodeType::Boolean:
        return Value(node->value == "true");
    case ASTNodeType::String:
        return Value(node->value); // Просто строка
    case ASTNodeType::Identifier: {
        // Поиск переменной по имени
        auto it = symbols.find(node->value);
        if (it == symbols.end())
            throw runtime_error("Неизвестная переменная: " + node->value);
        return it->second;
    }
    case ASTNodeType::BinOp: {
        // Бинарная операция: вычисляем левый и правый операнды
        Value left = evaluateExpression(node->children[0]);
        Value right = evaluateExpression(node->children[1]);

        // Обработка различных операторов
        if (node->value == "div") {
            if (left.type != ValueType::Integer || right.type != ValueType::Integer)
                throw runtime_error("Оператор div применим только к целым");
            return Value(left.intValue / right.intValue);
        }
        if (node->value == "mod") {
            if (left.type != ValueType::Integer || right.type != ValueType::Integer)
                throw runtime_error("Оператор mod применим только к целым");
            return Value(left.intValue % right.intValue);
        }
        if (node->value == "+") {
            if (left.type == ValueType::String || right.type == ValueType::String)
                return Value(left.strValue + right.strValue); // Конкатенация строк
            if (left.type == ValueType::Real || right.type == ValueType::Real)
                return Value(toReal(left) + toReal(right));
            return Value(left.intValue + right.intValue);
        }
        if (node->value == "-") {
            if (left.type == ValueType::Real || right.type == ValueType::Real)
                return Value(toReal(left) - toReal(right));
            return Value(left.intValue - right.intValue);
        }
        if (node->value == "*") {
            if (left.type == ValueType::Real || right.type == ValueType::Real)
                return Value(toReal(left) * toReal(right));
            return Value(left.intValue * right.intValue);
        }
        if (node->value == "/") {
            double l = (left.type == ValueType::Real) ? left.realValue : left.intValue;
            double r = (right.type == ValueType::Real) ? right.realValue : right.intValue;
            if (r == 0.0)
                throw runtime_error("Деление на ноль");
            return Value(l / r);
        }

        // Логические операции
        if (node->value == "and")
            return Value(left.boolValue && right.boolValue);
        if (node->value == "or")
            return Value(left.boolValue || right.boolValue);

        // Операторы сравнения
        if (node->value == "=")
            return Value(toReal(left) == toReal(right));
        if (node->value == "<>")
            return Value(toReal(left) != toReal(right));
        if (node->value == "<")
            return Value(toReal(left) < toReal(right));
        if (node->value == "<=")
            return Value(toReal(left) <= toReal(right));
        if (node->value == ">")
            return Value(toReal(left) > toReal(right));
        if (node->value == ">=")
            return Value(toReal(left) >= toReal(right));

        throw runtime_error("Неизвестный бинарный оператор: " + node->value);
    }
    case ASTNodeType::UnOp:
        // Унарные операции
        if (node->value == "-") {
            if (evaluateExpression(node->children[0]).type == ValueType::Real)
                return Value(-evaluateExpression(node->children[0]).realValue);
            else
                return Value(-evaluateExpression(node->children[0]).intValue);
        }
        if (node->value == "not")
            return Value(!evaluateExpression(node->children[0]).boolValue);
        throw runtime_error("Неизвестный унарный оператор: " + node->value);
    default:
        throw runtime_error("Ошибка вычисления выражения"); 
    }
}

// Обработка присваивания: имя переменной и значение
void Interpreter::executeAssignment(const shared_ptr<ASTNode>& node) {
    string varName = node->children[0]->value;              // Имя переменной
    Value value = evaluateExpression(node->children[1]);    // Вычисляем выражение
    symbols[varName] = value;                               // Сохраняем в таблицу символов
}

// Условный оператор: if-then-else
void Interpreter::executeIf(const shared_ptr<ASTNode>& node) {
    Value cond = evaluateExpression(node->children[0]); // Условие
    if (cond.boolValue)
        run(node->children[1]);                         // then
    else if (node->children.size() > 2)
        run(node->children[2]);                         // else
}

// Цикл while
void Interpreter::executeWhile(const shared_ptr<ASTNode>& node) {
    while (evaluateExpression(node->children[0]).boolValue) // Пока условие истинно
        run(node->children[1]);                             // Выполняем тело цикла
}

// Вывод значений (write/writeln)
void Interpreter::executeWrite(const shared_ptr<ASTNode>& node) {
    for (size_t i = 0; i < node->children.size(); ++i) {
        const auto& child = node->children[i];
        Value val = evaluateExpression(child); // Вычисляем значение
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
            cout << " "; // Пробел между значениями
    }
    if (node->type == ASTNodeType::Writeln)
        cout << endl; // Перевод строки для writeln
}

// Ввод значений (read/readln)
void Interpreter::executeRead(const shared_ptr<ASTNode>& node) {
    for (const auto& child : node->children) {
        string varName = child->value;   // Имя переменной
        // Определяем, целая или вещественная переменная
        if (getValueType(varName) == ValueType::Real) {
            double v;
            cin >> v;
            symbols[varName] = Value(v);
        }
        else {
            int v;
            cin >> v;
            symbols[varName] = Value(v);
        }
        if (node->type == ASTNodeType::Readln)
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Игнорируем остаток строки
    }
}

// Получение значения переменной по имени (используется для тестов или отладки)
int Interpreter::getVarValue(const string& name) const {
    auto it = symbols.find(name);
    if (it == symbols.end())
        throw runtime_error("Переменная не найдена: " + name);
    return it->second.intValue;
}

// Получаем тип переменной из symbols
ValueType Interpreter::getValueType(const string& name) const {
    auto it = symbols.find(name);
    if (it == symbols.end())
        throw runtime_error("Переменная не найдена: " + name);
    return it->second.type;
}

double Interpreter::toReal(const Value& v) const {
    if (v.type == ValueType::Real)
        return v.realValue;
    if (v.type == ValueType::Integer)
        return static_cast<double>(v.intValue);
    throw runtime_error("Ожидалось числовое значение");
}