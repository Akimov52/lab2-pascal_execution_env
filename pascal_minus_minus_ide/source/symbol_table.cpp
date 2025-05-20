#include "symbol_table.h"

// Конструктор SymbolInfo
SymbolInfo::SymbolInfo(string n, string t, string v)
    : name(n), type(t), value(v), table(nullptr) {
}

// Конструктор SymbolTable
SymbolTable::SymbolTable() {}

// Метод для добавления символа
void SymbolTable::addSymbol(const SymbolInfo& symbol) {
    symbols.push_back(make_shared<SymbolInfo>(symbol));
}

// Метод для поиска символа
SymbolInfo* SymbolTable::findSymbol(const string& name) {
    for (auto& symbol : symbols) {
        if (symbol->name == name) {
            return symbol.get();
        }
    }
    return nullptr;
}

// Метод для удаления символа
void SymbolTable::removeSymbol(const string& name) {
    auto it = remove_if(symbols.begin(), symbols.end(),
        [&name](const shared_ptr<SymbolInfo>& symbol) {
            return symbol->name == name;
        });
    symbols.erase(it, symbols.end());
}

// Метод для вывода таблицы символов
void SymbolTable::printTable() {
    for (const auto& symbol : symbols) {
        cout << "Name: " << symbol->name << ", Type: " << symbol->type
            << ", Value: " << symbol->value << endl;
    }
}
