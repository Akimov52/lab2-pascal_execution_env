#include <gtest.h>
#include "symbol_table.h"
#include <string>

class SymbolTableTest : public ::testing::Test {
protected:
    SymbolTable table;
    
    void SetUp() override {
        // Default capacity is used
    }
};

TEST_F(SymbolTableTest, AddAndFindSymbol) {
    // Create and add a symbol
    SymbolInfo symbol("x", "Integer", "10");
    table.addSymbol(symbol);
    
    // Find the symbol and verify its properties
    SymbolInfo* foundSymbol = table.findSymbol("x");
    
    ASSERT_NE(nullptr, foundSymbol);
    EXPECT_EQ("x", foundSymbol->name);
    EXPECT_EQ("Integer", foundSymbol->type);
    EXPECT_EQ("10", foundSymbol->value);
}

TEST_F(SymbolTableTest, FindNonExistentSymbol) {
    // Try to find a symbol that doesn't exist
    SymbolInfo* symbol = table.findSymbol("nonexistent");
    
    EXPECT_EQ(nullptr, symbol);
}

TEST_F(SymbolTableTest, AddMultipleSymbols) {
    // Add multiple symbols
    table.addSymbol(SymbolInfo("a", "Integer", "1"));
    table.addSymbol(SymbolInfo("b", "Real", "2.5"));
    table.addSymbol(SymbolInfo("c", "Boolean", "true"));
    table.addSymbol(SymbolInfo("message", "String", "Hello"));
    
    // Verify all symbols can be found
    SymbolInfo* symbolA = table.findSymbol("a");
    SymbolInfo* symbolB = table.findSymbol("b");
    SymbolInfo* symbolC = table.findSymbol("c");
    SymbolInfo* symbolMessage = table.findSymbol("message");
    
    ASSERT_NE(nullptr, symbolA);
    EXPECT_EQ("Integer", symbolA->type);
    EXPECT_EQ("1", symbolA->value);
    
    ASSERT_NE(nullptr, symbolB);
    EXPECT_EQ("Real", symbolB->type);
    EXPECT_EQ("2.5", symbolB->value);
    
    ASSERT_NE(nullptr, symbolC);
    EXPECT_EQ("Boolean", symbolC->type);
    EXPECT_EQ("true", symbolC->value);
    
    ASSERT_NE(nullptr, symbolMessage);
    EXPECT_EQ("String", symbolMessage->type);
    EXPECT_EQ("Hello", symbolMessage->value);
}

TEST_F(SymbolTableTest, RemoveSymbol) {
    // Add and then remove a symbol
    table.addSymbol(SymbolInfo("temp", "Integer", "42"));
    
    // Verify it exists
    ASSERT_NE(nullptr, table.findSymbol("temp"));
    
    // Remove it
    table.removeSymbol("temp");
    
    // Verify it no longer exists
    EXPECT_EQ(nullptr, table.findSymbol("temp"));
}

TEST_F(SymbolTableTest, UpdateSymbol) {
    // Add a symbol
    table.addSymbol(SymbolInfo("counter", "Integer", "0"));
    
    // Verify initial value
    SymbolInfo* symbol = table.findSymbol("counter");
    ASSERT_NE(nullptr, symbol);
    EXPECT_EQ("0", symbol->value);
    
    // Update by adding the same name with new value
    table.addSymbol(SymbolInfo("counter", "Integer", "1"));
    
    // Verify updated value
    symbol = table.findSymbol("counter");
    ASSERT_NE(nullptr, symbol);
    EXPECT_EQ("1", symbol->value);
}

TEST_F(SymbolTableTest, HandleCollisions) {
    // Add many symbols to force hash collisions
    for (int i = 0; i < 200; i++) {
        std::string name = "var" + std::to_string(i);
        std::string value = std::to_string(i);
        table.addSymbol(SymbolInfo(name, "Integer", value));
    }
    
    // Verify all symbols can be found
    for (int i = 0; i < 200; i++) {
        std::string name = "var" + std::to_string(i);
        std::string expectedValue = std::to_string(i);
        
        SymbolInfo* symbol = table.findSymbol(name);
        ASSERT_NE(nullptr, symbol);
        EXPECT_EQ(expectedValue, symbol->value);
    }
}

TEST_F(SymbolTableTest, CustomCapacityTable) {
    // Create a table with smaller capacity
    SymbolTable smallTable(10);
    
    // Add and verify symbols
    smallTable.addSymbol(SymbolInfo("x", "Integer", "1"));
    smallTable.addSymbol(SymbolInfo("y", "Integer", "2"));
    
    ASSERT_NE(nullptr, smallTable.findSymbol("x"));
    ASSERT_NE(nullptr, smallTable.findSymbol("y"));
    
    EXPECT_EQ("1", smallTable.findSymbol("x")->value);
    EXPECT_EQ("2", smallTable.findSymbol("y")->value);
}
