#include <gtest/gtest.h>
#include "postfix.h"

TEST(PostfixTest, ConvertSimpleExpression) {
    PostfixConverter converter;
    EXPECT_EQ(converter.toPostfix("2 + 3"), "2 3 +");
}

TEST(PostfixTest, ConvertComplexExpression) {
    PostfixConverter converter;
    EXPECT_EQ(converter.toPostfix("2 + 3 * 4"), "2 3 4 * +");
}

TEST(PostfixTest, EvaluateSimple) {
    PostfixEvaluator evaluator;
    EXPECT_EQ(evaluator.evaluate("2 3 +"), 5);
}

TEST(PostfixTest, EvaluateComplex) {
    PostfixEvaluator evaluator;
    EXPECT_EQ(evaluator.evaluate("2 3 4 * +"), 14);
}

TEST(PostfixTest, ThrowsOnInvalid) {
    PostfixEvaluator evaluator;
    EXPECT_THROW(evaluator.evaluate("2 +"), std::runtime_error);
}