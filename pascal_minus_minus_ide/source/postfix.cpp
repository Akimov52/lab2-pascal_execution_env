#include "postfix.h"
#include <stack>
#include <sstream>
#include <stdexcept>
#include <cmath>
#include <algorithm>
#include "interpreter.h" // Для доступа к типу Value
#include "logger.h"

// Вспомогательная функция: возвращает true, если строка — число (целое или вещественное)
static bool is_number(const string& s) {
    if (s.empty())
        return false;
    istringstream iss(s);
    double d;
    iss >> d;
    return iss.eof() && !iss.fail();
}

// Вычисляет значение выражения в постфиксной записи (Reverse Polish Notation)
// Использует стек для хранения промежуточных результатов
// Конструктор для PostfixCalculator
PostfixCalculator::PostfixCalculator() {
    initOperatorMap();
}

// Инициализация таблицы операторов с приоритетами
void PostfixCalculator::initOperatorMap() {
    // Арифметические операторы
    operatorMap["+"] = {OperatorType::Plus, 2, false, false};
    operatorMap["-"] = {OperatorType::Minus, 2, false, false};
    operatorMap["*"] = {OperatorType::Multiply, 3, false, false};
    operatorMap["/"] = {OperatorType::Divide, 3, false, false};
    operatorMap["div"] = {OperatorType::IntegerDivide, 3, false, false};
    operatorMap["mod"] = {OperatorType::Modulus, 3, false, false};
    
    // Операторы сравнения
    operatorMap["="] = {OperatorType::Equal, 1, false, false};
    operatorMap["<>"] = {OperatorType::NotEqual, 1, false, false};
    operatorMap["<"] = {OperatorType::Less, 1, false, false};
    operatorMap["<="] = {OperatorType::LessEqual, 1, false, false};
    operatorMap[">"] = {OperatorType::Greater, 1, false, false};
    operatorMap[">="] = {OperatorType::GreaterEqual, 1, false, false};
    
    // Логические операторы
    operatorMap["and"] = {OperatorType::And, 0, false, false};
    operatorMap["or"] = {OperatorType::Or, 0, false, false};
    operatorMap["not"] = {OperatorType::Not, 4, true, true};
    
    // Унарные операторы
    operatorMap["u-"] = {OperatorType::Minus, 4, true, true}; // Унарный минус
}

// Проверка, является ли токен оператором
bool PostfixCalculator::isOperator(const std::string& token) const {
    return operatorMap.find(token) != operatorMap.end();
}

// Получение информации об операторе
OperatorInfo PostfixCalculator::getOperatorInfo(const std::string& token) const {
    auto it = operatorMap.find(token);
    if (it != operatorMap.end()) {
        return it->second;
    }
    throw std::runtime_error("Неизвестный оператор: " + token);
}

// Реализация метода из интерфейса IPostfixCalculator
Value PostfixCalculator::performOperation(const std::string& op, const std::vector<Value>& operands) {
    // Проверка количества операндов
    if (operands.empty()) {
        throw std::runtime_error("Нет операндов для операции: " + op);
    }
    
    // Проверка, унарная ли это операция
    bool isUnary = operands.size() == 1 || op == "not" || op == "u-";
    
    if (isUnary) {
        if (operands.size() != 1) {
            throw std::runtime_error("Унарная операция " + op + " требует один операнд");
        }
        return performUnaryOperation(op, operands[0]);
    } else {
        if (operands.size() != 2) {
            throw std::runtime_error("Бинарная операция " + op + " требует два операнда");
        }
        return performBinaryOperation(op, operands[0], operands[1]);
    }
}

// Выполнение бинарной операции над значениями
Value PostfixCalculator::performBinaryOperation(const std::string& op, const Value& a, const Value& b) {
    Value result;
    
    // Проверяем, что операнды подходящего типа для данной операции
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
        // Деление всегда даёт вещественный результат
        result.type = ValueType::Real;
        result.realValue = (a.type == ValueType::Integer ? a.intValue : a.realValue) / 
                          (b.type == ValueType::Integer ? b.intValue : b.realValue);
    } else if (op == "div") {
        // Целочисленное деление требует целочисленных операндов
        if (a.type != ValueType::Integer || b.type != ValueType::Integer) {
            throw std::runtime_error("Оператор 'div' требует целочисленных операндов");
        }
        if (b.intValue == 0) {
            throw std::runtime_error("Деление на ноль");
        }
        result.type = ValueType::Integer;
        result.intValue = a.intValue / b.intValue;
    } else if (op == "mod") {
        // Модуль требует целочисленных операндов
        if (a.type != ValueType::Integer || b.type != ValueType::Integer) {
            throw std::runtime_error("Оператор 'mod' требует целочисленных операндов");
        }
        if (b.intValue == 0) {
            throw std::runtime_error("Деление на ноль в операции mod");
        }
        result.type = ValueType::Integer;
        result.intValue = a.intValue % b.intValue;
    } 
    // Операторы сравнения
    else if (op == "=" || op == "<>" || op == "<" || op == "<=" || op == ">" || op == ">=") {
        result.type = ValueType::Boolean;
        // Сравнение чисел
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
        // Сравнение булевых значений
        else if (a.type == ValueType::Boolean && b.type == ValueType::Boolean) {
            if (op == "=") result.boolValue = (a.boolValue == b.boolValue);
            else if (op == "<>") result.boolValue = (a.boolValue != b.boolValue);
            else {
                throw std::runtime_error("Операторы <, <=, >, >= не применимы к логическим значениям");
            }
        }
        // Сравнение строк
        else if (a.type == ValueType::String && b.type == ValueType::String) {
            if (op == "=") result.boolValue = (a.stringValue == b.stringValue);
            else if (op == "<>") result.boolValue = (a.stringValue != b.stringValue);
            else if (op == "<") result.boolValue = (a.stringValue < b.stringValue);
            else if (op == "<=") result.boolValue = (a.stringValue <= b.stringValue);
            else if (op == ">") result.boolValue = (a.stringValue > b.stringValue);
            else if (op == ">=") result.boolValue = (a.stringValue >= b.stringValue);
        }
        else {
            throw std::runtime_error("Несовместимые типы для сравнения");
        }
    }
    // Логические операторы
    else if (op == "and" || op == "or") {
        if (a.type != ValueType::Boolean || b.type != ValueType::Boolean) {
            throw std::runtime_error("Логические операторы требуют логических операндов");
        }
        result.type = ValueType::Boolean;
        if (op == "and") result.boolValue = a.boolValue && b.boolValue;
        else if (op == "or") result.boolValue = a.boolValue || b.boolValue;
    }

    // Если неизвестная операция
    else {
        throw std::runtime_error("Неизвестная операция: " + op);
    }
    
    return result;
}

// Выполнение унарной операции над значением
Value PostfixCalculator::performUnaryOperation(const std::string& op, const Value& a) {
    Value result;
    
    if (op == "u-") { // Унарный минус
        if (a.type == ValueType::Integer) {
            result.type = ValueType::Integer;
            result.intValue = -a.intValue;
        } else if (a.type == ValueType::Real) {
            result.type = ValueType::Real;
            result.realValue = -a.realValue;
        } else {
            throw std::runtime_error("Унарный минус применим только к числам");
        }
    } else if (op == "not") { // Логическое отрицание
        if (a.type != ValueType::Boolean) {
            throw std::runtime_error("Оператор 'not' требует логического операнда");
        }
        result.type = ValueType::Boolean;
        result.boolValue = !a.boolValue;
    } else {
        throw std::runtime_error("Неизвестная унарная операция: " + op);
    }
    
    return result;
}

// Реализация метода из интерфейса IPostfixCalculator
Value PostfixCalculator::evaluate(const std::shared_ptr<ASTNode>& node, const std::map<std::string, Value>& variables) {
    // Преобразуем AST в постфиксную запись
    std::vector<std::string> postfix = astToPostfix(node);
    // Вычисляем значение постфиксного выражения
    return evaluatePostfix(postfix, variables);
}

// Вычисление выражения в постфиксной форме с учетом переменных
Value PostfixCalculator::evaluatePostfix(const std::vector<std::string>& tokens, const std::map<std::string, Value>& variables) {
    std::stack<Value> valueStack;
    
    for (const auto& token : tokens) {
        // Если токен - число
        if (is_number(token)) {
            Value val;
            // Определяем тип числа (целое или вещественное)
            if (token.find('.') != std::string::npos) {
                val.type = ValueType::Real;
                val.realValue = std::stod(token);
            } else {
                val.type = ValueType::Integer;
                val.intValue = std::stoi(token);
            }
            valueStack.push(val);
        }
        // Если токен - строковый литерал
        else if (token.size() >= 2 && token[0] == '\'' && token[token.size()-1] == '\'') {
            Value val;
            val.type = ValueType::String;
            val.stringValue = token.substr(1, token.size() - 2);
            valueStack.push(val);
        }
        // Если токен - булево значение
        else if (token == "true" || token == "false") {
            Value val;
            val.type = ValueType::Boolean;
            val.boolValue = (token == "true");
            valueStack.push(val);
        }
        // Если токен - переменная
        else if (variables.find(token) != variables.end()) {
            valueStack.push(variables.at(token));
        }
        // Если токен - оператор
        else if (isOperator(token)) {
            // Проверяем, достаточно ли операндов в стеке
            const auto& opInfo = getOperatorInfo(token);
            
            if (opInfo.isUnary) {
                if (valueStack.empty()) {
                    throw std::runtime_error("Недостаточно операндов для унарного оператора " + token);
                }
                Value a = valueStack.top();
                valueStack.pop();
                std::vector<Value> operands = {a};
                valueStack.push(performOperation(token, operands));
            } else {
                if (valueStack.size() < 2) {
                    throw std::runtime_error("Недостаточно операндов для бинарного оператора " + token);
                }
                Value b = valueStack.top();
                valueStack.pop();
                Value a = valueStack.top();
                valueStack.pop();
                std::vector<Value> operands = {a, b};
                valueStack.push(performOperation(token, operands));
            }
        }
        // Неизвестный токен
        else {
            throw std::runtime_error("Неизвестный токен в постфиксном выражении: " + token);
        }
    }
    
    // После вычисления в стеке должен остаться ровно один результат
    if (valueStack.size() != 1) {
        throw std::runtime_error("Ошибка вычисления постфиксного выражения: неверное количество результатов");
    }
    
    return valueStack.top();
}

// Возвращает приоритет оператора (чем выше число — тем выше приоритет)
static int precedence(const string& op) {
    if (op == "+" || op == "-")
        return 1;
    if (op == "*" || op == "/")
        return 2;
    if (op == "^")
        return 3;
    return 0;
}

// Проверяет, является ли строка оператором (+, -, *, /, ^)
static bool is_operator(const string& op) {
    return op == "+" || op == "-" || op == "*" || op == "/" || op == "^";
}

// Разделяет строку на токены (числа, операторы, скобки)
// Пример: "2+3*(4-1)" -> ["2", "+", "3", "*", "(", "4", "-", "1", ")"]
static vector<string> tokenize(const string& expr) {
    vector<string> tokens;
    string num;
    for (size_t i = 0; i < expr.size(); ++i) {
        char c = expr[i];
        if (isspace(c))
            continue; // пропускаем пробелы
        if (isdigit(c) || c == '.')
            // Если цифра или точка — продолжаем накапливать число
            num += c;
        else {
            // Если встретили оператор или скобку — сначала добавляем накопленное число
            if (!num.empty()) {
                tokens.push_back(num);
                num.clear();
            }
            // Добавляем оператор или скобку как отдельный токен
            if (is_operator(string(1, c)) || c == '(' || c == ')')
                tokens.push_back(string(1, c));
        }
    }
    // Добавляем последнее число, если оно есть
    if (!num.empty())
        tokens.push_back(num);
    return tokens;
}

// Переводит инфиксное выражение в постфиксное (алгоритм сортировочной станции Дейкстры)
// Например, "2+3*4" -> ["2", "3", "4", "*", "+"]
std::vector<std::string> PostfixCalculator::infixToPostfix(const std::vector<std::string>& infix) {
    std::vector<std::string> output;     // Выходная очередь (постфиксная запись)
    std::stack<std::string> ops;         // Стек операторов

    for (const auto& token : infix) {
        // Если токен - число, идентификатор, строка или булево значение - сразу в выходную очередь
        if (is_number(token) || 
            (token.size() >= 2 && token[0] == '\'' && token[token.size()-1] == '\'') ||
            token == "true" || token == "false" ||
            (token[0] >= 'a' && token[0] <= 'z') || (token[0] >= 'A' && token[0] <= 'Z')) {
            output.push_back(token);
        }
        // Если токен - оператор
        else if (isOperator(token)) {
            const auto& currentOp = getOperatorInfo(token);
            
            // Выталкиваем операторы с большим или равным приоритетом (с учетом ассоциативности)
            while (!ops.empty() && ops.top() != "(" && isOperator(ops.top())) {
                const auto& topOp = getOperatorInfo(ops.top());
                
                // Для левоассоциативных операторов выталкиваем если приоритет >= 
                // Для правоассоциативных операторов выталкиваем если приоритет >
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
        // Открывающая скобка - в стек
        else if (token == "(") {
            ops.push(token);
        }
        // Закрывающая скобка - выталкиваем операторы до открывающей скобки
        else if (token == ")") {
            while (!ops.empty() && ops.top() != "(") {
                output.push_back(ops.top());
                ops.pop();
            }
            
            if (!ops.empty() && ops.top() == "(") {
                ops.pop(); // Удаляем открывающую скобку
            } else {
                throw std::runtime_error("Несогласованные скобки в выражении");
            }
        }
        // Неизвестный токен
        else {
            throw std::runtime_error("Неизвестный токен в выражении: " + token);
        }
    }
    
    // Выталкиваем оставшиеся операторы
    while (!ops.empty()) {
        if (ops.top() == "(" || ops.top() == ")") {
            throw std::runtime_error("Несогласованные скобки в выражении");
        }
        output.push_back(ops.top());
        ops.pop();
    }
    
    return output;
}

// Преобразует АСТ в последовательность токенов в постфиксной форме
std::vector<std::string> PostfixCalculator::astToPostfix(const std::shared_ptr<ASTNode>& node) {
    std::vector<std::string> output;
    if (!node) return output;
    
    // Рекурсивно обрабатываем дерево
    processASTNode(node, output);
    
    LOG_DEBUG("Постфиксная форма: " + [&output]() {
        std::string result;
        for (const auto& token : output) {
            result += token + " ";
        }
        return result;
    }());
    
    return output;
}

// Рекурсивный метод для преобразования АСТ в постфиксную форму
void PostfixCalculator::processASTNode(const std::shared_ptr<ASTNode>& node, std::vector<std::string>& output) {
    if (!node) return;
    
    switch (node->type) {
        // Числовые литералы
        case ASTNodeType::Number:
        case ASTNodeType::Real:
            output.push_back(node->value);
            break;
            
        // Строковые литералы
        case ASTNodeType::String:
            output.push_back("'" + node->value + "'");
            break;
            
        // Булевы литералы
        case ASTNodeType::Boolean:
            output.push_back(node->value); // true или false
            break;
            
        // Идентификаторы (переменные)
        case ASTNodeType::Identifier:
            output.push_back(node->value);
            break;
            
        // Унарные операторы
        case ASTNodeType::UnOp:
            if (!node->children.empty()) {
                processASTNode(node->children[0], output);
                
                // Унарный минус
                if (node->value == "-") {
                    output.push_back("u-"); // Помечаем как унарный минус
                }
                // Отрицание
                else if (node->value == "not") {
                    output.push_back("not");
                }
            }
            break;
            
        // Бинарные операторы
        case ASTNodeType::BinOp:
            if (node->children.size() >= 2) {
                processASTNode(node->children[0], output);
                processASTNode(node->children[1], output);
                output.push_back(node->value); // Оператор
            }
            break;
            
        // Выражения
        case ASTNodeType::Expression:
            for (const auto& child : node->children) {
                processASTNode(child, output);
            }
            break;
            
        default:
            // Другие типы узлов, которые не являются частью выражений
            break;
    }
}