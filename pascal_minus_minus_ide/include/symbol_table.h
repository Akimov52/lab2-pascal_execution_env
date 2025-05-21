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
public:
    SymbolTable();

    void addSymbol(const SymbolInfo& symbol);
    SymbolInfo* findSymbol(const string& name);
    void removeSymbol(const string& name);
    void printTable();

private:
    vector<shared_ptr<SymbolInfo>> symbols;
};