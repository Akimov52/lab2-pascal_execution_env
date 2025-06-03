#include "symbol_table.h"
#include <functional>

// Конструктор SymbolInfo
SymbolInfo::SymbolInfo(const string& n, const string& t, const string& v) : name(n), type(t), value(v), table(nullptr) {}

// Конструктор SymbolTable
SymbolTable::SymbolTable(size_t cap) : capacity(cap), count(0) {
    buckets = new ChainNode * [capacity];
    for (size_t i = 0; i < capacity; ++i)
        buckets[i] = nullptr;
}

SymbolTable::~SymbolTable() {
    for (size_t i = 0; i < capacity; ++i) {
        ChainNode* node = buckets[i];
        while (node) {
            ChainNode* tmp = node;
            node = node->next;
            delete tmp;
        }
    }
    delete[] buckets;
}

size_t SymbolTable::hashKey(const string& key) const {
    return hash<string>{}(key) % capacity;
}

// Метод для добавления символа
void SymbolTable::addSymbol(const SymbolInfo& symbol) {
    size_t idx = hashKey(symbol.name);
    ChainNode* node = buckets[idx];
    while (node) {
        if (node->data.name == symbol.name) {
            // Перезаписываем
            node->data.type = symbol.type;
            node->data.value = symbol.value;
            return;
        }
        node = node->next;
    }
    // Вставляем в голову списка
    buckets[idx] = new ChainNode(symbol, buckets[idx]);
    ++count;
}

// Метод для поиска символа
SymbolInfo* SymbolTable::findSymbol(const string& name) {
    size_t idx = hashKey(name);
    ChainNode* node = buckets[idx];
    while (node) {
        if (node->data.name == name)
            return &node->data;
        node = node->next;
    }
    return nullptr;
}

// Метод для удаления символа
void SymbolTable::removeSymbol(const string& name) {
    size_t idx = hashKey(name);
    ChainNode* node = buckets[idx];
    ChainNode* prev = nullptr;
    while (node) {
        if (node->data.name == name) {
            if (prev)
                prev->next = node->next;
            else
                buckets[idx] = node->next;
            delete node;
            --count;
            return;
        }
        prev = node;
        node = node->next;
    }
}

// Метод для вывода таблицы символов
void SymbolTable::printTable() {
    for (size_t i = 0; i < capacity; ++i) {
        ChainNode* node = buckets[i];
        while (node) {
            cout << "Name: " << node->data.name << ", Type: " << node->data.type << ", Value: " << node->data.value << endl;
            node = node->next;
        }
    }
}