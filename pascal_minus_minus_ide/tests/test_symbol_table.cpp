#include <gtest/gtest.h>
#include "symbol_table.h"

// ����: ������� � ����������� ����� �������
TEST(SymbolTableTest, InsertAndLookup) {
    SymbolTable table;

    // ��������� ���������� "a" � ����� Variable � ��������� 1
    EXPECT_TRUE(table.insert("a", SymbolType::Variable, 1));

    SymbolInfo info;

    // ���������, ��� ������ ������
    EXPECT_TRUE(table.lookup("a", info));

    // ���������, ��� �������� ���������
    EXPECT_EQ(info.value, 1);
}

// ����: ������� ��������� ������� ���� �� ��������������
TEST(SymbolTableTest, InsertDuplicate) {
    SymbolTable table;

    // ������ ������� ������ ������ �������
    EXPECT_TRUE(table.insert("a", SymbolType::Variable, 1));

    // ��������� ������� � ��� �� ������ ������ ���� ���������
    EXPECT_FALSE(table.insert("a", SymbolType::Variable, 2));
}

// ����: ����� ��������������� ��������������
TEST(SymbolTableTest, LookupNotExist) {
    SymbolTable table;
    SymbolInfo info;

    // �������, ��� ����� �� ������������ ����� ������ false
    EXPECT_FALSE(table.lookup("not_exist", info));
}

// ����: ���������� �������� ������������� �������
TEST(SymbolTableTest, UpdateSymbol) {
    SymbolTable table;

    // ��������� ����������
    table.insert("a", SymbolType::Variable, 1);

    // ��������� �������� � 1 �� 2
    EXPECT_TRUE(table.update("a", 2));

    SymbolInfo info;

    // ����������, ��� ���������� ���������
    EXPECT_TRUE(table.lookup("a", info));
    EXPECT_EQ(info.value, 2);
}

// ����: ������� �������� �������������� ������
TEST(SymbolTableTest, UpdateNotExist) {
    SymbolTable table;

    // �������, ��� update �� ��������� (������� ���)
    EXPECT_FALSE(table.update("not_exist", 42));
}