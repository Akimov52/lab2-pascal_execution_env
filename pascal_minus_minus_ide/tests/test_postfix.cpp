#include <gtest.h>
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

// Тест: Преобразование выражения со скобками "(2 + 3) * 4"
TEST(PostfixTest, ConvertParenthesizedExpression) {
    PostfixConverter converter;

    // Скобки должны изменить порядок выполнения: "(2 + 3)" вычисляется первым, затем "* 4"
    // => постфикс: "2 3 + 4 *"
    EXPECT_EQ(converter.toPostfix("(2 + 3) * 4"), "2 3 + 4 *");
}

// Тест: Преобразование выражения с вложенными скобками "2 * (3 + (4 - 1))"
TEST(PostfixTest, ConvertNestedParentheses) {
    PostfixConverter converter;

    // Внутренние скобки вычисляются первыми, затем внешние, затем умножение
    // => постфикс: "2 3 4 1 - + *"
    EXPECT_EQ(converter.toPostfix("2 * (3 + (4 - 1))"), "2 3 4 1 - + *");
}

// Тест: Вычисление выражения со скобками "2 3 + 4 *"
TEST(PostfixTest, EvaluateParenthesized) {
    PostfixEvaluator evaluator;

    // (2 + 3) * 4 = 5 * 4 = 20
    EXPECT_EQ(evaluator.evaluate("2 3 + 4 *"), 20);
}

// Тест: Вычисление выражения с вложенными скобками "2 3 4 1 - + *"
TEST(PostfixTest, EvaluateNestedParentheses) {
    PostfixEvaluator evaluator;

    // 2 * (3 + (4 - 1)) = 2 * (3 + 3) = 2 * 6 = 12
    EXPECT_EQ(evaluator.evaluate("2 3 4 1 - + *"), 12);
}

// Тест: Преобразование выражения с логическими операторами "a > b and c < d"
TEST(PostfixTest, ConvertLogicalExpression) {
    PostfixConverter converter;

    // Логические операторы имеют свой приоритет
    EXPECT_EQ(converter.toPostfix("a > b and c < d"), "a b > c d < and");
}

// Тест: Преобразование выражения с операторами не (not) и или (or)
TEST(PostfixTest, ConvertNotAndOrOperators) {
    PostfixConverter converter;

    // Приоритет операций: not (высокий), and (средний), or (низкий)
    EXPECT_EQ(converter.toPostfix("not a or b and c"), "a not b c and or");
}

// Тест: Обработка неопределенных переменных в постфиксном выражении
TEST(PostfixTest, ThrowsOnUndefinedVariable) {
    PostfixEvaluator evaluator;
    
    // При попытке вычислить выражение с неопределенной переменной должно выбрасываться исключение
    // Пример с переменной в постфиксном выражении
    EXPECT_THROW(evaluator.evaluate("b 3 +"), runtime_error);
}

// Тест: Вычисление выражения с оператором деления
TEST(PostfixTest, EvaluateDivision) {
    PostfixEvaluator evaluator;

    // 10 / 2 = 5
    EXPECT_EQ(evaluator.evaluate("10 2 /"), 5);
}

// Тест: Деление на ноль должно вызывать исключение
TEST(PostfixTest, ThrowsOnDivisionByZero) {
    PostfixEvaluator evaluator;

    // При делении на ноль должно выбрасываться исключение
    EXPECT_THROW(evaluator.evaluate("5 0 /"), runtime_error);
}
