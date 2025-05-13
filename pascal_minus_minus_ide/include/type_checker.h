#ifndef TYPE_CHECKER_H
#define TYPE_CHECKER_H

#include "ast.h"
#include <unordered_map>
#include <vector>
#include <string>

// Класс для проверки типов (semantic/type checking) в AST
class TypeChecker {
    // Вложенная структура для области видимости (scope)
    struct Scope {
        unordered_map<string, TypeInfo> variables; // Имя переменной -> информация о типе
        unordered_map<string, pair<vector<TypeInfo>, TypeInfo>> functions; // Имя функции -> (типы параметров, возвращаемый тип)
    };
    vector<Scope> scopes;            // Стек областей видимости (локальные и глобальные переменные/функции)
    TypeInfo currentReturnType;      // Текущий возвращаемый тип (для проверки return)
    bool inFunction = false;         // Флаг: находимся ли мы внутри функции

public:
    // Войти в новую область видимости (например, при входе в блок или функцию)
    void enterScope();
    // Выйти из текущей области видимости
    void exitScope();
    // Объявить переменную в текущей области видимости
    void declareVariable(const string& name, const TypeInfo& type);
    // Объявить функцию в текущей области видимости
    void declareFunction(const string& name, const vector<TypeInfo>& params, const TypeInfo& returnType);
    // Получить тип переменной (поиск по всем областям видимости)
    TypeInfo getVariableType(const string& name) const;
    // Получить тип функции (типы параметров и возвращаемый тип)
    pair<vector<TypeInfo>, TypeInfo> getFunctionType(const string& name) const;
    // Установить ожидаемый возвращаемый тип (для функции)
    void setReturnType(const TypeInfo& type);
    // Получить текущий возвращаемый тип
    TypeInfo getCurrentReturnType() const;
    // Проверить, находимся ли мы внутри функции
    bool isInFunction() const { return inFunction; }
    // Войти в функцию (установить флаг)
    void enterFunction() { inFunction = true; }
    // Выйти из функции (сбросить флаг)
    void exitFunction() { inFunction = false; }
    // Проверить совместимость типов (например, при присваивании)
    bool isTypeCompatible(const TypeInfo& expected, const TypeInfo& actual) const;
    // Проверить возможность приведения типа (например, integer -> real)
    bool canCast(const TypeInfo& from, const TypeInfo& to) const;
};

#endif // TYPE_CHECKER_H