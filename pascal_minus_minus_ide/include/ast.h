#ifndef AST_H
#define AST_H

/**
 * @file ast.h
 * @brief Абстрактное синтаксическое дерево (AST) для Pascal--
 * 
 * Определяет структуры данных для представления программы в виде дерева,
 * что упрощает анализ и интерпретацию кода.
 */

#include <string>
#include <vector>
#include <memory>

// Предварительное объявление типов для устранения циклических зависимостей
struct Token;

// Используем TokenType из пространства имен PascalToken
namespace PascalToken { enum TokenType; }
using PascalToken::TokenType;

using namespace std;

/**
 * Перечисление поддерживаемых типов данных для семантического анализа и проверки типов
 * Все базовые типы данных, поддерживаемые в языке Pascal--
 */
enum class DataType {
    Integer,    // Целое число
    Real,       // Вещественное число
    Boolean,    // Логический тип
    String      // Строка
};

/**
 * Описание типа переменной или выражения
 * Структура позволяет описывать базовые типы данных
 */
struct TypeInfo {
    DataType type;                         // Базовый тип данных

    explicit TypeInfo(DataType t) : type(t) {}
};

/**
 * Перечисление всех возможных типов узлов AST (абстрактного синтаксического дерева)
 * Каждый тип узла представляет определенную конструкцию языка Pascal--
 */
enum class ASTNodeType {
    Program,        // Главная программа
    Block,          // Блок begin-end
    ConstSection,   // Секция объявления констант
    ConstDecl,      // Объявление одной константы
    VarSection,     // Секция объявления переменных
    VarDecl,        // Объявление одной переменной
    Assignment,     // Присваивание
    If,             // Условный оператор
    While,          // Цикл while
    Write,          // Оператор вывода
    Writeln,        // Оператор вывода с переходом на новую строку
    Read,           // Оператор ввода
    Readln,         // Оператор ввода с переходом на новую строку
    Expression,     // Обобщённое выражение
    Number,         // Целое число
    Real,           // Вещественное число
    String,         // Строка
    Boolean,        // Логическое значение
    Identifier,     // Идентификатор (имя переменной)
    BinOp,          // Бинарная операция (+, -, *, /, и т.д.)
    UnOp,           // Унарная операция (например, -a, not a)
    ForLoop         // for ... := ... to ... do ...
};

// Структура узла AST (абстрактного синтаксического дерева)
class ASTNode {
public:
    ASTNodeType type;                              // Тип узла
    string value;                                  // Значение (например, имя переменной или литерал)
    vector<shared_ptr<ASTNode>> children;          // Дочерние узлы (например, аргументы, тело блока)

    ASTNode(ASTNodeType t, const string& v = "") : type(t), value(v) {}
    ASTNode(ASTNodeType t, const string& v, const shared_ptr<ASTNode>& child) : type(t), value(v) { children.push_back(child); }
    virtual ~ASTNode() = default;
};

#endif // AST_H