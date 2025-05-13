#include <gtest/gtest.h>
#include "parser.h"

// ����: ������ ��������� (��������� ������ AST � ����� Program)
TEST(ParserTest, ParseEmptyProgram) {
    Parser parser("");
    auto root = parser.parse();
    EXPECT_EQ(root->type, ASTNodeType::Program);  // ���� ���������
    EXPECT_TRUE(root->children.empty());          // ��� �����������
}

// ����: ������������ (x := 42;)
TEST(ParserTest, ParseAssignment) {
    Parser parser("x := 42;");
    auto root = parser.parse();
    ASSERT_FALSE(root->children.empty());                 // ��������, ��� AST �� ������
    EXPECT_EQ(root->children[0]->type, ASTNodeType::Assignment); // ������ ���� � ������������
}

// ����: �������� ����������� � else
TEST(ParserTest, ParseIfElse) {
    Parser parser("if 1 then x := 1 else x := 2;");
    auto root = parser.parse();
    ASSERT_FALSE(root->children.empty());           // ���������, ��� ���� ����������
    EXPECT_EQ(root->children[0]->type, ASTNodeType::If); // ������ ���� � �������
    EXPECT_EQ(root->children[0]->children.size(), 3);    // cond, then, else
}

// ����: ���� while
TEST(ParserTest, ParseWhile) {
    Parser parser("while x < 10 do x := x + 1;");
    auto root = parser.parse();
    ASSERT_FALSE(root->children.empty());             // AST �� ������ ���� ������
    EXPECT_EQ(root->children[0]->type, ASTNodeType::While); // ��� � ����
}

// ����: ����� ����� write � writeln
TEST(ParserTest, ParseWriteWriteln) {
    Parser parser("write('hi'); writeln('bye');");
    auto root = parser.parse();
    ASSERT_GE(root->children.size(), 2);              // ������� ������� 2 ����������
    EXPECT_EQ(root->children[0]->type, ASTNodeType::Write);   // ������ � write
    EXPECT_EQ(root->children[1]->type, ASTNodeType::Writeln); // ������ � writeln
}

// ����: ���������� ����������
TEST(ParserTest, ParseVarDecl) {
    Parser parser("var x: integer; y: integer;");
    auto root = parser.parse();
    // ����� ����� ���������, ��� ������ �������� ���� ���������� ����������
    // ��������, ���� � AST �������� ����������� ���� VarDecl:
    // EXPECT_EQ(root->children[0]->type, ASTNodeType::VarDecl);
    // EXPECT_EQ(root->children[0]->children.size(), 2); // x � y
}

// ����: ���������� ��������
TEST(ParserTest, ParseConstDecl) {
    Parser parser("const a = 10; b = 20;");
    auto root = parser.parse();
}

// ����: ������� ��������� � var, begin-end � ������ ������
TEST(ParserTest, ParseComplexProgram) {
    Parser parser("var x: integer; begin x := 1; while x < 5 do x := x + 1; end.");
    auto root = parser.parse();
    EXPECT_EQ(root->type, ASTNodeType::Program);   // ������� ���� � ���������


// ����: ������ ���������� � ��������� �����������
TEST(ParserTest, ParseSyntaxError) {
    Parser parser("begin x := ; end.");
    EXPECT_THROW(parser.parse(), std::runtime_error); // ������� ����������
}

// ����: ������������������ ���������� ������������
TEST(ParserTest, ParseMultipleStatements) {
    Parser parser("x := 1; y := 2; z := x + y;");
    auto root = parser.parse();

    // �������� ���������� ����������
    EXPECT_EQ(root->children.size(), 3);
    // �������� ����� �����
    EXPECT_EQ(root->children[0]->type, ASTNodeType::Assignment);
    EXPECT_EQ(root->children[1]->type, ASTNodeType::Assignment);
    EXPECT_EQ(root->children[2]->type, ASTNodeType::Assignment);
}
