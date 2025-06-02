#include <gtest/gtest.h>
#include "parser.h"

// Тест: Пустая программа (ожидается пустой AST с типом Program)
TEST(ParserTest, ParseEmptyProgram) {
    Parser parser("");
    auto root = parser.parse();
    EXPECT_EQ(root->type, ASTNodeType::Program);  // Узел программы
    EXPECT_TRUE(root->children.empty());          // Без содержимого
}

// Тест: Присваивание (x := 42;)
TEST(ParserTest, ParseAssignment) {
    Parser parser("x := 42;");
    auto root = parser.parse();
    ASSERT_FALSE(root->children.empty());                 // Проверка, что AST не пустой
    EXPECT_EQ(root->children[0]->type, ASTNodeType::Assignment); // Первый узел — присваивание
}

// Тест: Условная конструкция с else
TEST(ParserTest, ParseIfElse) {
    Parser parser("if 1 then x := 1 else x := 2;");
    auto root = parser.parse();
    ASSERT_FALSE(root->children.empty());           // Убедиться, что есть содержимое
    EXPECT_EQ(root->children[0]->type, ASTNodeType::If); // Первый узел — условие
    EXPECT_EQ(root->children[0]->children.size(), 3);    // cond, then, else
}

// Тест: Цикл while
TEST(ParserTest, ParseWhile) {
    Parser parser("while x < 10 do x := x + 1;");
    auto root = parser.parse();
    ASSERT_FALSE(root->children.empty());             // AST не должен быть пустым
    EXPECT_EQ(root->children[0]->type, ASTNodeType::While); // Тип — цикл
}

// Тест: Вывод через write и writeln
TEST(ParserTest, ParseWriteWriteln) {
    Parser parser("write('hi'); writeln('bye');");
    auto root = parser.parse();
    ASSERT_GE(root->children.size(), 2);              // Ожидаем минимум 2 инструкции
    EXPECT_EQ(root->children[0]->type, ASTNodeType::Write);   // Первый — write
    EXPECT_EQ(root->children[1]->type, ASTNodeType::Writeln); // Второй — writeln
}

// Тест: Объявления переменных
TEST(ParserTest, ParseVarDecl) {
    Parser parser("var x: integer; y: integer;");
    auto root = parser.parse();
    // Здесь можно проверить, что корень содержит узел объявления переменных
    // Например, если в AST создаётся специальный узел VarDecl:
    // EXPECT_EQ(root->children[0]->type, ASTNodeType::VarDecl);
    // EXPECT_EQ(root->children[0]->children.size(), 2); // x и y
}

// Тест: Объявления констант
TEST(ParserTest, ParseConstDecl) {
    Parser parser("const a = 10; b = 20;");
    auto root = parser.parse();
}

// Тест: Сложная программа с var, begin-end и циклом внутри
TEST(ParserTest, ParseComplexProgram) {
    Parser parser("var x: integer; begin x := 1; while x < 5 do x := x + 1; end.");
    auto root = parser.parse();
    EXPECT_EQ(root->type, ASTNodeType::Program);   // Главный узел — программа


// Тест: Ошибка синтаксиса — выражение отсутствует
TEST(ParserTest, ParseSyntaxError) {
    Parser parser("begin x := ; end.");
    EXPECT_THROW(parser.parse(), std::runtime_error); // Ожидаем исключение
}

// Тест: Последовательность нескольких присваиваний
TEST(ParserTest, ParseMultipleStatements) {
    Parser parser("x := 1; y := 2; z := x + y;");
    auto root = parser.parse();

    // Проверка количества операторов
    EXPECT_EQ(root->children.size(), 3);
    // Проверка типов узлов
    EXPECT_EQ(root->children[0]->type, ASTNodeType::Assignment);
    EXPECT_EQ(root->children[1]->type, ASTNodeType::Assignment);
    EXPECT_EQ(root->children[2]->type, ASTNodeType::Assignment);
}

// Тест: Цикл for с возрастанием (to)
TEST(ParserTest, ParseForTo) {
    Parser parser("for i := 1 to 10 do x := x + i;");
    auto root = parser.parse();
    ASSERT_FALSE(root->children.empty());
    EXPECT_EQ(root->children[0]->type, ASTNodeType::ForLoop);
    EXPECT_EQ(root->children[0]->value, "i"); // Имя переменной цикла
    ASSERT_EQ(root->children[0]->children.size(), 3); // От, до и тело цикла
}

// Тест: Цикл for с убыванием (downto)
TEST(ParserTest, ParseForDownto) {
    Parser parser("for i := 10 downto 1 do x := x - 1;");
    auto root = parser.parse();
    ASSERT_FALSE(root->children.empty());
    EXPECT_EQ(root->children[0]->type, ASTNodeType::ForLoop);
    EXPECT_EQ(root->children[0]->value, "i|downto"); // Имя переменной с флагом downto
    ASSERT_EQ(root->children[0]->children.size(), 3); // От, до и тело цикла
}

// Тест: Чтение данных через readln
TEST(ParserTest, ParseReadln) {
    Parser parser("readln(x, y, z);");
    auto root = parser.parse();
    ASSERT_FALSE(root->children.empty());
    EXPECT_EQ(root->children[0]->type, ASTNodeType::Readln);
    ASSERT_GE(root->children[0]->children.size(), 3); // Три переменных для чтения
}


