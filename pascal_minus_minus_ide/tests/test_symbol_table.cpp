#include <gtest.h>
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

// Тест: Добавление константы в таблицу символов
TEST(SymbolTableTest, InsertConstant) {
    SymbolTable table;

    // Добавляем константу "PI" со значением 3.14
    EXPECT_TRUE(table.insert("PI", SymbolType::Constant, 3.14));

    SymbolInfo info;

    // Проверяем, что константа добавлена верно
    EXPECT_TRUE(table.lookup("PI", info));
    EXPECT_EQ(info.type, SymbolType::Constant);
    EXPECT_DOUBLE_EQ(info.value, 3.14);
}

// Тест: Попытка обновить константу (должна быть неуспешной)
TEST(SymbolTableTest, UpdateConstant) {
    SymbolTable table;

    // Добавляем константу
    EXPECT_TRUE(table.insert("MAX_VALUE", SymbolType::Constant, 100));

    // Попытка обновить константу должна быть неуспешной, так как константы неизменяемы
    EXPECT_FALSE(table.update("MAX_VALUE", 200));

    SymbolInfo info;
    EXPECT_TRUE(table.lookup("MAX_VALUE", info));
    EXPECT_EQ(info.value, 100); // Значение не должно было измениться
}

// Тест: Добавление и поиск типа переменной
TEST(SymbolTableTest, VariableWithType) {
    SymbolTable table;

    // Добавляем переменную с типом Integer
    EXPECT_TRUE(table.insert("counter", SymbolType::Variable, 0, "Integer"));

    SymbolInfo info;
    EXPECT_TRUE(table.lookup("counter", info));

    // Проверяем тип данных переменной
    EXPECT_EQ(info.dataType, "Integer");
}



// Тест: Очистка таблицы символов
TEST(SymbolTableTest, ClearTable) {
    SymbolTable table;

    // Добавляем несколько символов
    EXPECT_TRUE(table.insert("a", SymbolType::Variable, 1));
    EXPECT_TRUE(table.insert("b", SymbolType::Variable, 2));
    EXPECT_TRUE(table.insert("c", SymbolType::Constant, 3));

    // Проверяем, что символы существуют
    SymbolInfo info;
    EXPECT_TRUE(table.lookup("a", info));
    EXPECT_TRUE(table.lookup("b", info));
    EXPECT_TRUE(table.lookup("c", info));

    // Очищаем таблицу
    table.clear();

    // Проверяем, что все символы удалены
    EXPECT_FALSE(table.lookup("a", info));
    EXPECT_FALSE(table.lookup("b", info));
    EXPECT_FALSE(table.lookup("c", info));
}

// Тест: Проверка существования символа без получения его информации
TEST(SymbolTableTest, ContainsSymbol) {
    SymbolTable table;

    // Добавляем символ
    EXPECT_TRUE(table.insert("x", SymbolType::Variable, 10));

    // Проверяем, что символ существует в таблице
    EXPECT_TRUE(table.contains("x"));
    EXPECT_FALSE(table.contains("y"));
}