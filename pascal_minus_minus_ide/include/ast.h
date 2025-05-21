#ifndef AST_H
#define AST_H

#include "lexer.h" // Для TokenType

// Перечисление поддерживаемых типов данных для семантического анализа и проверки типов
enum class DataType {
    Integer,    // Целое число
    Real,       // Вещественное число
    Boolean,    // Логический тип
    String,     // Строка
    Array,      // Массив
    Procedure,  // Процедура
    Function,   // Функция
    Void        // Пустой тип (например, для процедур)
};

// Описание типа переменной или выражения, включая вложенные типы для массивов
struct TypeInfo {
    DataType type;                         // Базовый тип данных
    shared_ptr<TypeInfo> elementType;      // Тип элементов (для массивов)
    int arraySize = 0;                     // Размер массива (если применимо)

    explicit TypeInfo(DataType t) : type(t), elementType(nullptr), arraySize(0) {}
};

// Перечисление всех возможных типов узлов AST (абстрактного синтаксического дерева)
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
    Identifier,     // Идентификатор (имя переменной, функции и т.д.)
    ProcCall,       // Вызов процедуры
    BinOp,          // Бинарная операция (+, -, *, /, и т.д.)
    UnOp,           // Унарная операция (например, -a, not a)
    ArrayDecl,      // Объявление массива
    ArrayAccess,    // Обращение к элементу массива
    ProcedureDecl,  // Объявление процедуры
    FunctionDecl,   // Объявление функции
    Call,           // Вызов функции/процедуры
    Return          // Оператор возврата
};

// Структура узла AST (абстрактного синтаксического дерева)
struct ASTNode {
    ASTNodeType type;                              // Тип узла
    string value;                                  // Значение (например, имя переменной или литерал)
    vector<shared_ptr<ASTNode>> children;          // Дочерние узлы (например, аргументы, тело блока)

    ASTNode(ASTNodeType t, const string& v = "") : type(t), value(v) {}
    virtual ~ASTNode() = default;
};

#endif // AST_H