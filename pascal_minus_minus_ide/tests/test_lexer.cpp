#include <gtest/gtest.h>
#include "lexer.h"

TEST(LexerTest, TokenizeSimple) {
    Lexer lexer;
    string code = "program test; var x := 42;";
    auto tokens = lexer.tokenize(code);

    ASSERT_GE(tokens.size(), 7);
    EXPECT_EQ(tokens[0].type, "keyword");   // program
    EXPECT_EQ(tokens[1].type, "id");        // test
    EXPECT_EQ(tokens[2].type, "op");        // ;
    EXPECT_EQ(tokens[3].type, "keyword");   // var
    EXPECT_EQ(tokens[4].type, "id");        // x
    EXPECT_EQ(tokens[5].type, "assign");    // :=
    EXPECT_EQ(tokens[6].type, "number");    // 42
    EXPECT_EQ(tokens[7].type, "op");        // ;
}
