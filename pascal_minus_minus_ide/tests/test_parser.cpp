#include <gtest/gtest.h>
#include "parser.h"

TEST(ParserTest, EmptyProgram) {
    Parser parser;
    auto ast = parser.parse("");
    EXPECT_EQ(ast->type, ASTNodeType::Program);
}

// TODO: Добавить тесты на корректные и некорректные программы
