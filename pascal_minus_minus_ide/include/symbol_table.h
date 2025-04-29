#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H
#include <string>
#include <unordered_map>
#include <variant>

enum class SymbolType { Variable, Constant };
using SymbolValue = std::variant<int, double, std::string>;

struct SymbolInfo {
    SymbolType type;
    std::string name;
    SymbolValue value;
};

class SymbolTable {
public:
    bool insert(const std::string& name, SymbolType type, const SymbolValue& value);
    bool lookup(const std::string& name, SymbolInfo& out) const;
    bool update(const std::string& name, const SymbolValue& value);
private:
    std::unordered_map<std::string, SymbolInfo> table;
};

#endif // SYMBOL_TABLE_H
