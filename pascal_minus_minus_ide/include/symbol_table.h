#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <memory>

using namespace std;

class SymbolInfo {
public:
    string name;
    string type;
    string value;
    shared_ptr<SymbolInfo> table; // Для вложенных таблиц

    SymbolInfo(const string& n, const string& t, const string& v);
};

class SymbolTable {
protected:
    struct ChainNode {
        SymbolInfo data;
        ChainNode* next;
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