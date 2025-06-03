#pragma once

/**
 * @file symbol_table.h
 * @brief Таблица символов для компилятора Pascal--
 * 
 * Реализует хеш-таблицу для эффективного хранения и поиска символов
 * (переменных, констант, процедур, функций)
 */

#include <iostream>
#include <string>
#include <vector>
#include <memory>

using namespace std;

/**
 * Класс для хранения информации о символе (переменной, константе, функции или процедуре)
 */
class SymbolInfo {
public:
    string name;  // Имя символа
    string type;  // Тип символа (Integer, Real, Boolean, String и т.д.)
    string value; // Значение символа (для констант)
    shared_ptr<SymbolInfo> table; // Для вложенных таблиц

    /**
     * Конструктор SymbolInfo
     * @param n Имя символа
     * @param t Тип символа
     * @param v Значение символа
     */
    SymbolInfo(const string& n, const string& t, const string& v);
};

/**
 * Таблица символов - хеш-таблица для эффективного хранения и поиска символов
 * Используется для проверки объявления переменных и хранения значений во время интерпретации
 */
class SymbolTable {
protected:
    /**
     * Структура узла для разрешения коллизий в хеш-таблице
     * Реализует связный список для цепочек в хеш-таблице
     */
    struct ChainNode {
        SymbolInfo data; // Данные о символе
        ChainNode* next; // Указатель на следующий элемент в цепочке
        ChainNode(const SymbolInfo& s, ChainNode* n) : data(s), next(n) {}
    };

    ChainNode** buckets;
    size_t capacity;
    size_t count;

    size_t hashKey(const string& key) const;

public:
    static constexpr size_t DEFAULT_CAPACITY = 128;

    SymbolTable(size_t cap = DEFAULT_CAPACITY);
    ~SymbolTable();

    void addSymbol(const SymbolInfo& symbol);
    SymbolInfo* findSymbol(const string& name);
    void removeSymbol(const string& name);
    void printTable();
};