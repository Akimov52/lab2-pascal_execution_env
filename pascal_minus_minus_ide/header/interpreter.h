#pragma once

#include "ast.h"
#include "interfaces.h"
#include "error_reporter.h"
#include "postfix.h"  // Включаем полное определение PostfixCalculator
#include <map>
#include <vector>
#include <string>
#include <memory>

enum class ValueType { Integer, Real, Boolean, String };

/**
 * Универсальная структура для хранения значений разных типов в Pascal--
 * Поддерживает четыре основных типа данных: Integer, Real, Boolean и String
 * и предоставляет методы для преобразования между ними
 */
struct Value {
    ValueType type;        // Тип значения (Integer, Real, Boolean, String)
    int intValue;          // Целочисленное значение (для типа Integer)
    double realValue;      // Вещественное значение (для типа Real)
    bool boolValue;        // Логическое значение (для типа Boolean)
    string stringValue;    // Строковое значение (для типа String)
    
    /**
     * Методы для безопасного преобразования между типами
     * Генерируют исключения при невозможности преобразования
     */
    int toInt() const;      // Преобразование к целому числу
    double toReal() const;  // Преобразование к вещественному числу
    bool toBool() const;    // Преобразование к логическому значению
    string toString() const; // Преобразование к строке
    
    // Конструкторы для различных типов данных
    Value();                  // Конструктор по умолчанию (создает нулевое значение)
    explicit Value(int v);    // Создание из целого числа
    explicit Value(double v); // Создание из вещественного числа
    explicit Value(bool v);   // Создание из логического значения
    explicit Value(const string& v); // Создание из строки
};

/**
 * Класс интерпретатора языка Pascal--
 * Отвечает за выполнение программы, представленной в виде абстрактного синтаксического дерева (AST)
 * Включает функции для управления переменными, вычисления выражений и выполнения операторов
 * Использует PostfixCalculator для вычисления выражений в постфиксной форме
 */
class Interpreter : public IInterpreter {
public:
    /**
     * Конструктор по умолчанию
     * Создает интерпретатор без обработчика ошибок
     */
    Interpreter();
    
    /**
     * Конструктор с обработчиком ошибок
     * @param errorReporter Обработчик ошибок для вывода сообщений об ошибках и предупреждениях
     */
    explicit Interpreter(shared_ptr<IErrorReporter> errorReporter);
    
    /**
     * Реализация методов интерфейса IInterpreter
     */

    /**
     * Запускает интерпретацию программы, представленной деревом AST
     * @param ast Корневой узел AST программы
     */
    void run(const shared_ptr<ASTNode>& ast) override;
    
    /**
     * Вычисляет значение строкового выражения
     * @param expression Строковое представление выражения
     * @return Результат вычисления выражения
     */
    Value evaluate(const string& expression) override;
    
    /**
     * Проверяет, объявлена ли переменная с указанным именем
     * @param name Имя переменной
     * @return true, если переменная объявлена, иначе false
     */
    bool isDeclared(const string& name) const override;
    
    /**
     * Получает значение переменной по имени
     * @param name Имя переменной
     * @return Значение переменной или генерирует исключение, если переменная не найдена
     */
    Value getVariable(const string& name) const override;
    
    /**
     * Устанавливает значение переменной
     * @param name Имя переменной
     * @param value Новое значение переменной
     */
    void setVariable(const string& name, const Value& value) override;
    
    /**
     * Очищает таблицу символов (удаляет все переменные)
     */
    void clearSymbols() override;
    
    /**
     * Возвращает имя компонента
     * @return Строка "Interpreter"
     */
    string getComponentName() const override { return "Interpreter"; }
    
    /**
     * Дополнительные методы интерпретатора
     */

    /**
     * Выполняет оператор цикла For в Pascal--
     * @param node Узел AST, представляющий оператор For
     */
    void executeFor(const shared_ptr<ASTNode>& node);

    /**
     * Возвращает целочисленное значение переменной
     * @param name Имя переменной
     * @return Целочисленное значение переменной или преобразованное значение другого типа
     * @throws Исключение, если переменная не найдена или не может быть преобразована к целому типу
     */
    int getVarValue(const string& name) const;

    /**
     * Возвращает тип переменной
     * @param name Имя переменной
     * @return Тип переменной (Integer, Real, Boolean, String)
     * @throws Исключение, если переменная не найдена
     */
    ValueType getValueType(const string& name) const;

    /**
     * Возвращает ссылку на таблицу символов (все переменные)
     * @return Константная ссылка на карту символов
     */
    const map<string, Value>& getAllSymbols() const { return symbols; }
    
    /**
     * Методы для репортинга ошибок и предупреждений
     */

    /**
     * Сообщает об ошибке в интерпретаторе
     * @param message Сообщение об ошибке
     * @param line Номер строки (по умолчанию 0)
     * @param column Номер столбца (по умолчанию 0)
     */
    void reportError(const string& message, int line = 0, int column = 0) const;

    /**
     * Сообщает о предупреждении в интерпретаторе
     * @param message Сообщение о предупреждении
     * @param line Номер строки (по умолчанию 0)
     * @param column Номер столбца (по умолчанию 0)
     */
    void reportWarning(const string& message, int line = 0, int column = 0) const;

private:
    map<string, Value> symbols;
    shared_ptr<IErrorReporter> errorReporter;
    unique_ptr<PostfixCalculator> postfixCalculator;
    
    // Методы выполнения операторов
    void executeAssignment(const std::shared_ptr<ASTNode>& node);
    void executeIf(const std::shared_ptr<ASTNode>& node);
    void executeWhile(const std::shared_ptr<ASTNode>& node);
    void executeWrite(const std::shared_ptr<ASTNode>& node);
    void executeRead(const std::shared_ptr<ASTNode>& node);
    
    // Методы для вычисления выражений
    Value evaluateExpression(const std::shared_ptr<ASTNode>& node);
    Value evaluateUsingPostfix(const std::shared_ptr<ASTNode>& node);
};
