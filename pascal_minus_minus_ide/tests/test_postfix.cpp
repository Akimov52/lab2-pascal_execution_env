#include <gtest/gtest.h>
#include "postfix.h"

// Тест: Преобразование простого выражения "2 + 3" в постфиксную форму
TEST(PostfixTest, ConvertSimpleExpression) {
    PostfixConverter converter;

    // Ожидаем результат "2 3 +"
    EXPECT_EQ(converter.toPostfix("2 + 3"), "2 3 +");
}

// Тест: Преобразование выражения с приоритетами операций "2 + 3 * 4"
TEST(PostfixTest, ConvertComplexExpression) {
    PostfixConverter converter;

    // Умножение должно выполняться раньше: "3 * 4", затем "2 +"
    // => постфикс: "2 3 4 * +"
    EXPECT_EQ(converter.toPostfix("2 + 3 * 4"), "2 3 4 * +");
}

// Тест: Вычисление простого постфиксного выражения "2 3 +"
TEST(PostfixTest, EvaluateSimple) {
    PostfixEvaluator evaluator;

    // 2 + 3 = 5
    EXPECT_EQ(evaluator.evaluate("2 3 +"), 5);
}

// Тест: Вычисление более сложного постфиксного выражения "2 3 4 * +"
TEST(PostfixTest, EvaluateComplex) {
    PostfixEvaluator evaluator;

    // 3 * 4 = 12, затем 2 + 12 = 14
    EXPECT_EQ(evaluator.evaluate("2 3 4 * +"), 14);
}

// Тест: Обработка ошибки при некорректном постфиксном выражении "2 +"
TEST(PostfixTest, ThrowsOnInvalid) {
    PostfixEvaluator evaluator;

    // Ожидаем исключение runtime_error, так как выражение некорректное
    EXPECT_THROW(evaluator.evaluate("2 +"), runtime_error);
}
