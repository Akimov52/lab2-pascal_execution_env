#include "interpreter.h" // Заголовочный файл с описанием класса Interpreter и структуры Value
#include <iostream>      // Для ввода/вывода
#include <limits>        // Для numeric_limits, используемого в Readln
#include <cmath>
#include <algorithm>
#include <sstream>
#include "postfix.h"  // Добавляем включение postfix.h в исходный файл
#include "logger.h"     // Для логирования

// Реализация конструкторов Value (универсального контейнера значений)
// ========================

// Конструктор по умолчанию: значение типа Integer, равное 0
Value::Value() : type(ValueType::Integer), intValue(0), realValue(0.0), boolValue(false), stringValue("") {}

// Конструктор для целого значения
Value::Value(int v) : type(ValueType::Integer), intValue(v), realValue(0.0), boolValue(false), stringValue("") {}

// Конструктор для вещественного значения
Value::Value(double v) : type(ValueType::Real), intValue(0), realValue(v), boolValue(false), stringValue("") {}

// Конструктор для булевого значения
Value::Value(bool v) : type(ValueType::Boolean), intValue(0), realValue(0.0), boolValue(v), stringValue("") {}

// Конструктор для строкового значения
Value::Value(const std::string& v) : type(ValueType::String), intValue(0), realValue(0.0), boolValue(false), stringValue(v) {}

// Методы преобразования типов
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
                throw std::runtime_error("Невозможно преобразовать строку \"" + stringValue + "\" в целое число");
            }
        default:
            throw std::runtime_error("Неподдерживаемый тип для преобразования в целое");
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
                throw std::runtime_error("Невозможно преобразовать строку \"" + stringValue + "\" в вещественное число");
            }
        default:
            throw std::runtime_error("Неподдерживаемый тип для преобразования в вещественное");
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
            throw std::runtime_error("Неподдерживаемый тип для преобразования в логический");
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
            throw std::runtime_error("Неподдерживаемый тип для преобразования в строку");
    }
}

// ========================
// Интерпретатор
// ========================

// Конструктор по умолчанию
Interpreter::Interpreter() : errorReporter(std::make_shared<ErrorReporter>()) {
    // Создаём постфиксный калькулятор для вычисления выражений
    postfixCalculator = std::make_unique<PostfixCalculator>();
}

// Реализация константных методов для репортинга ошибок
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

// Конструктор с указанием обработчика ошибок
Interpreter::Interpreter(std::shared_ptr<IErrorReporter> reporter) 
    : errorReporter(reporter ? reporter : std::make_shared<ErrorReporter>()), 
      postfixCalculator(std::make_unique<PostfixCalculator>()) {}
      
// Проверка существования переменной
bool Interpreter::isDeclared(const std::string& name) const {
    return symbols.find(name) != symbols.end();
}

// Получение значения переменной
Value Interpreter::getVariable(const std::string& name) const {
    auto it = symbols.find(name);
    if (it == symbols.end()) {
        throw std::runtime_error("Неизвестная переменная: " + name);
    }
    return it->second;
}

// Установка значения переменной
void Interpreter::setVariable(const std::string& name, const Value& value) {
    symbols[name] = value;
}

// Очистка всех символов
void Interpreter::clearSymbols() {
    symbols.clear();
}

// Оценка выражения по строке (интерфейсный метод)
Value Interpreter::evaluate(const std::string& expression) {
    LOG_INFO("Evaluating expression: " + expression);
    // Здесь должен быть код парсинга строки в AST
    // Для простоты вернем заглушку
    Value result;
    result.type = ValueType::String;
    result.stringValue = "Not implemented: " + expression;
    return result;
}

/**
 * Вычисляет значение выражения, представленного узлом AST
 * @param expression Узел AST, представляющий выражение
 * @return Результат вычисления выражения
 */
Value Interpreter::evaluateExpression(const std::shared_ptr<ASTNode>& expression) {
    // Делегируем вычисление методу evaluateUsingPostfix, который использует PostfixCalculator
    return evaluateUsingPostfix(expression);
}


/**
 * Главный метод интерпретации программы Pascal--
 * Рекурсивно обрабатывает узлы AST, начиная с корневого узла
 * @param root Корневой узел AST программы
 */
void Interpreter::run(const std::shared_ptr<ASTNode>& root) {
    if (!root) {
        reportWarning("Пустая программа");
        return; // Если узел пустой — ничего не делаем
    }
    
    LOG_INFO("Начало выполнения программы");
    switch (root->type) {
    case ASTNodeType::ConstDecl: {
        const std::string& name = root->value;
        const std::string& typeName = root->children[0]->value;
        // Используем постфиксный калькулятор для вычисления выражения
        Value val = evaluateUsingPostfix(root->children[1]);
        
        LOG_DEBUG("Объявление константы " + name + " типа " + typeName);
        
        if (typeName == "real" || typeName == "double")
            symbols[name] = Value(val.realValue);
        else if (typeName == "integer")
            symbols[name] = Value(val.intValue);
        else if (typeName == "boolean")
            symbols[name] = Value(val.boolValue);
        else if (typeName == "string")
            symbols[name] = Value(val.stringValue);
        else
            throw std::runtime_error("Неизвестный тип константы: " + typeName);
        break;
    }
    case ASTNodeType::VarDecl: {
        try {
            const std::string& name = root->value;
            const std::string& typeName = root->children[0]->value;
            
            LOG_DEBUG("Объявление переменной " + name + " типа " + typeName);
            
            // Создаем переменную с нулевым значением соответствующего типа
            if (typeName == "real" || typeName == "double") {
                symbols[name] = Value(0.0);
            } else if (typeName == "integer") {
                symbols[name] = Value(0);
            } else if (typeName == "boolean") {
                symbols[name] = Value(false);
            } else if (typeName == "string") {
                symbols[name] = Value("");
            } else {
                reportError("Неизвестный тип переменной: " + typeName);
                throw std::runtime_error("Неизвестный тип переменной: " + typeName);
            }
        } catch (const std::exception& e) {
            reportError(std::string("Ошибка при объявлении переменной: ") + e.what());
            throw; // Перебрасываем исключение дальше
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
        // Эти узлы обычно используются только в выражениях, но если встретились здесь — ничего не делаем
        break;
    // Удалены упоминания процедур и функций (Call, Return)
    case ASTNodeType::Expression:
    default:
        std::cerr << "Неизвестный оператор типа: " << static_cast<int>(root->type) << std::endl;
        throw std::runtime_error("Неизвестный оператор");
    }
}


// ===== Реализация executeFor =====
// Реализация executeFor как метода класса Interpreter
void Interpreter::executeFor(const std::shared_ptr<ASTNode>& node) {
    try {
        LOG_DEBUG("Выполнение цикла for");
        
        // node->value содержит имя переменной и, возможно, "|downto"
        std::string varName = node->value;
        bool isDownto = false;
        
        // Проверяем, если это цикл с убыванием (downto)
        size_t pipePos = varName.find('|');
        if (pipePos != std::string::npos) {
            isDownto = (varName.substr(pipePos + 1) == "downto");
            varName = varName.substr(0, pipePos);
        }
        
        // Используем постфиксный калькулятор для вычисления начального и конечного значений
        Value fromVal = evaluateUsingPostfix(node->children[0]);
        Value toVal = evaluateUsingPostfix(node->children[1]);
        auto& body = node->children[2];
        
        LOG_DEBUG("Цикл for " + varName + (isDownto ? " downto" : " to") + 
        " от " + std::to_string(fromVal.toInt()) + " до " + std::to_string(toVal.toInt()));

        // Проверка, что значения from и to можно преобразовать в целые числа
        if (fromVal.type != ValueType::Integer) {
            if (fromVal.type == ValueType::Real) {
                reportWarning("Значение типа Real будет преобразовано в целое для цикла for");
                fromVal = Value(static_cast<int>(fromVal.realValue));
            } else {
                reportError("Начальное значение цикла for должно быть числовым");
                throw std::runtime_error("Начальное значение цикла for должно быть числовым");
            }
        }
        
        if (toVal.type != ValueType::Integer) {
            if (toVal.type == ValueType::Real) {
                reportWarning("Конечное значение типа Real будет преобразовано в целое для цикла for");
                toVal = Value(static_cast<int>(toVal.realValue));
            } else {
                reportError("Конечное значение цикла for должно быть числовым");
                throw std::runtime_error("Конечное значение цикла for должно быть числовым");
            }
        }

        // Сохраняем текущее значение переменной цикла, если она существует
        Value oldValue;
        bool varExists = isDeclared(varName);
        if (varExists) {
            oldValue = getVariable(varName);
        }

        // Счетчик итераций для защиты от бесконечных циклов
        int iterations = 0;
        const int MAX_ITERATIONS = 10000; // Максимальное число итераций
        
        // Используем класс для автоматического восстановления переменной при выходе из функции
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
        
        // Создаем объект для автоматического восстановления переменной
        VariableRestorer restorer(symbols, varName, varExists, oldValue);
        
        try {
            // Выполняем цикл в зависимости от направления (to или downto)
            if (isDownto) {
                for (int i = fromVal.intValue; i >= toVal.intValue; --i) {
                    symbols[varName] = Value(i);
                    run(body);
                    
                    // Проверка на бесконечный цикл
                    iterations++;
                    if (iterations > MAX_ITERATIONS) {
                        reportWarning("Возможный бесконечный цикл for downto (превышено максимальное число итераций)");
                        break;
                    }
                }
            } else {
                for (int i = fromVal.intValue; i <= toVal.intValue; ++i) {
                    symbols[varName] = Value(i);
                    run(body);
                    
                    // Проверка на бесконечный цикл
                    iterations++;
                    if (iterations > MAX_ITERATIONS) {
                        reportWarning("Возможный бесконечный цикл for to (превышено максимальное число итераций)");
                        break;
                    }
                }
            }
            
            LOG_DEBUG("Завершение цикла for после " + std::to_string(iterations) + " итераций");
        } 
        catch (const std::exception& e) {
            reportError(std::string("Ошибка при выполнении цикла for: ") + e.what());
            throw; // Перебрасываем исключение дальше
        }
        // Нет необходимости явно восстанавливать переменную, это сделает деструктор VariableRestorer
    } 
    catch (const std::exception& e) {
        reportError(std::string("Ошибка в цикле for: ") + e.what());
        throw; // Перебрасываем исключение дальше
    }
}

// Метод evaluateExpression реализован в другом месте файла

void Interpreter::executeAssignment(const shared_ptr<ASTNode>& node) {
    try {
        // Обычное присваивание переменной
        std::string varName = node->children[0]->value;
        
        // Используем постфиксную форму для вычисления выражения
        Value value = evaluateUsingPostfix(node->children[1]);
        
        // Логируем операцию присваивания
        LOG_DEBUG("Присваивание переменной " + varName);
        
        // Сохраняем новое значение
        symbols[varName] = value;
    } catch (const std::exception& e) {
        reportError(std::string("Ошибка при выполнении присваивания: ") + e.what());
        throw; // Перебрасываем исключение для обработки на верхнем уровне
    }
}

void Interpreter::executeIf(const shared_ptr<ASTNode>& node) {
    try {
        // Вычисляем условие с использованием постфиксной формы
        Value cond = evaluateUsingPostfix(node->children[0]);
        
        // Проверяем, что условие имеет булевый тип
        if (cond.type != ValueType::Boolean) {
            reportWarning("Условие в операторе if должно быть логического типа");
            cond = Value(cond.toBool()); // Преобразуем к булевому типу
        }
        
        LOG_DEBUG("Выполнение условного оператора if, условие: " + std::string(cond.boolValue ? "true" : "false"));
        
        // Выполняем соответствующую ветвь
        if (cond.boolValue)
            run(node->children[1]); // then блок
        else if (node->children.size() > 2)
            run(node->children[2]); // else блок (если есть)
    } catch (const std::exception& e) {
        reportError(std::string("Ошибка при выполнении условного оператора: ") + e.what());
    }
}

void Interpreter::executeWhile(const shared_ptr<ASTNode>& node) {
    try {
        LOG_DEBUG("Начало выполнения цикла while");
        
        // Счетчик итераций для защиты от бесконечных циклов
        int iterations = 0;
        const int MAX_ITERATIONS = 10000; // Максимальное число итераций
        
        while (true) {
            // Вычисляем условие с использованием постфиксной формы
            Value cond = evaluateUsingPostfix(node->children[0]);
            
            // Проверяем, что условие имеет булевый тип
            if (cond.type != ValueType::Boolean) {
                reportWarning("Условие в операторе while должно быть логического типа");
                cond = Value(cond.toBool()); // Преобразуем к булевому типу
            }
            
            // Если условие не выполнено, выходим из цикла
            if (!cond.boolValue)
                break;
                
            // Выполняем тело цикла
            run(node->children[1]);
            
            // Проверка на бесконечный цикл
            iterations++;
            if (iterations > MAX_ITERATIONS) {
                reportWarning("Возможный бесконечный цикл while (превышено максимальное число итераций)");
                break;
            }
        }
        
        LOG_DEBUG("Завершение цикла while после " + std::to_string(iterations) + " итераций");
    } catch (const std::exception& e) {
        reportError(std::string("Ошибка при выполнении цикла while: ") + e.what());
    }
}

void Interpreter::executeWrite(const shared_ptr<ASTNode>& node) {
    try {
        LOG_DEBUG("Выполнение оператора write/writeln");
        
        for (size_t i = 0; i < node->children.size(); ++i) {
            // Используем постфиксный калькулятор для вычисления выражения
            Value val = evaluateUsingPostfix(node->children[i]);
            
            // Выводим значение в зависимости от его типа
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
                reportWarning("Неподдерживаемый тип данных для вывода");
                cout << "[Неизвестный тип]";
                break;
            }
            
            // Добавляем пробел между элементами
            if (i + 1 < node->children.size()) 
                cout << " ";
        }
    } catch (const std::exception& e) {
        reportError(std::string("Ошибка при выполнении write/writeln: ") + e.what());
    }
    if (node->type == ASTNodeType::Writeln) cout << endl;
}

void Interpreter::executeRead(const shared_ptr<ASTNode>& node) {
    try {
        LOG_DEBUG("Выполнение оператора read/readln");
        
        for (const auto& child : node->children) {
            // Получаем имя переменной
            string varName = child->value;
            
            // Проверяем, что переменная существует
            if (!isDeclared(varName)) {
                reportError("Попытка чтения в необъявленную переменную: " + varName);
                continue;
            }
            
            // Определяем тип переменной
            ValueType varType = getValueType(varName);
            
            // Вводим значение в зависимости от типа переменной
            switch (varType) {
                case ValueType::Integer: {
                    int v;
                    if (cin >> v) {
                        symbols[varName] = Value(v);
                    } else {
                        reportError("Ошибка при чтении целого числа");
                        cin.clear(); // Сбрасываем состояние ошибки
                    }
                    break;
                }
                case ValueType::Real: {
                    double v;
                    if (cin >> v) {
                        symbols[varName] = Value(v);
                    } else {
                        reportError("Ошибка при чтении вещественного числа");
                        cin.clear(); // Сбрасываем состояние ошибки
                    }
                    break;
                }
                case ValueType::Boolean: {
                    string input;
                    if (cin >> input) {
                        // Преобразовываем введенный текст в булево значение
                        transform(input.begin(), input.end(), input.begin(), ::tolower);
                        bool value = (input == "true" || input == "1" || input == "yes");
                        symbols[varName] = Value(value);
                    } else {
                        reportError("Ошибка при чтении логического значения");
                        cin.clear(); // Сбрасываем состояние ошибки
                    }
                    break;
                }
                case ValueType::String: {
                    string v;
                    if (cin >> v) {
                        symbols[varName] = Value(v);
                    } else {
                        reportError("Ошибка при чтении строки");
                        cin.clear(); // Сбрасываем состояние ошибки
                    }
                    break;
                }
                default: {
                    reportError("Неподдерживаемый тип переменной для ввода: " + varName);
                    break;
                }
            }
        }
        
        // Для readln пропускаем остаток строки
        if (node->type == ASTNodeType::Readln) {
            LOG_DEBUG("Чтение до конца строки (readln)");
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
    } catch (const std::exception& e) {
        reportError(std::string("Ошибка при выполнении read/readln: ") + e.what());
        cin.clear(); // Сбрасываем состояние ошибки
        cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Пропускаем остаток строки
    }
}

// Исправленная версия метода getVarValue для решения проблемы с вызовом const метода
int Interpreter::getVarValue(const string& name) const {
    try {
        // Проверяем существование переменной
        auto it = symbols.find(name);
        if (it == symbols.end()) {
            reportError("Переменная не найдена: " + name, 0, 0);  // Добавляем параметры line и column
            throw std::runtime_error("Переменная не найдена: " + name);
        }
        
        // Если переменная целого типа, возвращаем её значение
        if (it->second.type == ValueType::Integer) {
            return it->second.intValue;
        } else {
            // Для других типов пытаемся преобразовать к целому
            return it->second.toInt();
        }
    } catch (const std::exception& e) {
        LOG_ERROR(std::string("Ошибка при получении значения переменной: ") + e.what());
        throw; // Перебрасываем исключение дальше
    }
}


// Исправленная версия метода getValueType для решения проблемы с вызовом const метода
ValueType Interpreter::getValueType(const string& name) const {
    try {
        // Проверяем существование переменной в таблице символов
        auto it = symbols.find(name);
        if (it == symbols.end()) {
            // Если переменная не найдена, генерируем ошибку с указанием нулевых координат
            reportError("Переменная не найдена: " + name, 0, 0);
            throw std::runtime_error("Переменная не найдена: " + name);
        }
        
        // Возвращаем тип переменной из таблицы символов
        return it->second.type;
    } catch (const std::exception& e) {
        // Логируем ошибку и перебрасываем исключение дальше
        LOG_ERROR(std::string("Ошибка при определении типа переменной: ") + e.what());
        throw; // Перебрасываем исключение дальше
    }
}


// Исправленная версия метода evaluateUsingPostfix для вычисления выражений
/**
 * Вычисляет значение выражения, представленного узлом AST, используя постфиксный калькулятор
 * @param node Узел AST, представляющий выражение
 * @return Значение выражения
 */
Value Interpreter::evaluateUsingPostfix(const std::shared_ptr<ASTNode>& node) {
    try {
        // Используем метод evaluate из интерфейса IPostfixCalculator
        // Приводим типы к совместимым с интерфейсом
        if (postfixCalculator) {
            // Передаем таблицу символов в калькулятор для доступа к переменным
            const std::map<std::string, Value>& vars = symbols;
            return postfixCalculator->evaluate(node, vars);
        } else {
            // Ошибка, если калькулятор не инициализирован
            throw std::runtime_error("PostfixCalculator not initialized");
        }
    } catch (const std::exception& e) {
        // Обрабатываем и логируем ошибку, возвращаем значение по умолчанию
        reportError(std::string("Ошибка вычисления выражения: ") + e.what(), 0, 0);
        return Value(); // Возвращаем значение по умолчанию (0)
    }
}

