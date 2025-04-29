#include <gtest/gtest.h>
#include "symbol_table.h"

TEST(SymbolTableTest, InsertFail) {
    SymbolTable table;
    // Попытка вставить дважды одну и ту же переменную
    EXPECT_TRUE(table.insert("a", SymbolType::Variable, 1));
    EXPECT_FALSE(table.insert("a", SymbolType::Variable, 2));
}

TEST(SymbolTableTest, LookupFail) {
    SymbolTable table;
    SymbolInfo info;
    EXPECT_FALSE(table.lookup("not_exist", info));
}

TEST(SymbolTableTest, UpdateFail) {
    SymbolTable table;
    EXPECT_FALSE(table.update("not_exist", 42));
}
