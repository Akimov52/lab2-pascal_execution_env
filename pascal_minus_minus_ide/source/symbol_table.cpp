#include "symbol_table.h"

bool SymbolTable::insert(const std::string& name, SymbolType type, const SymbolValue& value) {
    if (table.find(name) != table.end()) return false;
    table[name] = SymbolInfo{type, name, value};
    return true;
}

bool SymbolTable::lookup(const std::string& name, SymbolInfo& out) const {
    auto it = table.find(name);
    if (it == table.end()) return false;
    out = it->second;
    return true;
}

bool SymbolTable::update(const std::string& name, const SymbolValue& value) {
    auto it = table.find(name);
    if (it == table.end()) return false;
    it->second.value = value;
    return true;
}
