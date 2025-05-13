#include <gtest/gtest.h>
#include "symbol_table.h"

// Тест: Вставка и последующий поиск символа
TEST(SymbolTableTest, InsertAndLookup) {
    SymbolTable table;

    // Вставляем переменную "a" с типом Variable и значением 1
    EXPECT_TRUE(table.insert("a", SymbolType::Variable, 1));

    SymbolInfo info;

    // Проверяем, что символ найден
    EXPECT_TRUE(table.lookup("a", info));

    // Проверяем, что значение корректно
    EXPECT_EQ(info.value, 1);
}

// Тест: Попытка повторной вставки того же идентификатора
TEST(SymbolTableTest, InsertDuplicate) {
    SymbolTable table;

    // Первая вставка должна пройти успешно
    EXPECT_TRUE(table.insert("a", SymbolType::Variable, 1));

    // Повторная вставка с тем же именем должна быть отклонена
    EXPECT_FALSE(table.insert("a", SymbolType::Variable, 2));
}

// Тест: Поиск несуществующего идентификатора
TEST(SymbolTableTest, LookupNotExist) {
    SymbolTable table;
    SymbolInfo info;

    // Ожидаем, что поиск по неизвестному имени вернет false
    EXPECT_FALSE(table.lookup("not_exist", info));
}

// Тест: Обновление значения существующего символа
TEST(SymbolTableTest, UpdateSymbol) {
    SymbolTable table;

    // Вставляем переменную
    table.insert("a", SymbolType::Variable, 1);

    // Обновляем значение с 1 на 2
    EXPECT_TRUE(table.update("a", 2));

    SymbolInfo info;

    // Убеждаемся, что обновление сработало
    EXPECT_TRUE(table.lookup("a", info));
    EXPECT_EQ(info.value, 2);
}

// Тест: Попытка обновить несуществующий символ
TEST(SymbolTableTest, UpdateNotExist) {
    SymbolTable table;

    // Ожидаем, что update не сработает (символа нет)
    EXPECT_FALSE(table.update("not_exist", 42));
}