#include <gtest/gtest.h>
#include "postfix.h"

// ����: �������������� �������� ��������� "2 + 3" � ����������� �����
TEST(PostfixTest, ConvertSimpleExpression) {
    PostfixConverter converter;

    // ������� ��������� "2 3 +"
    EXPECT_EQ(converter.toPostfix("2 + 3"), "2 3 +");
}

// ����: �������������� ��������� � ������������ �������� "2 + 3 * 4"
TEST(PostfixTest, ConvertComplexExpression) {
    PostfixConverter converter;

    // ��������� ������ ����������� ������: "3 * 4", ����� "2 +"
    // => ��������: "2 3 4 * +"
    EXPECT_EQ(converter.toPostfix("2 + 3 * 4"), "2 3 4 * +");
}

// ����: ���������� �������� ������������ ��������� "2 3 +"
TEST(PostfixTest, EvaluateSimple) {
    PostfixEvaluator evaluator;

    // 2 + 3 = 5
    EXPECT_EQ(evaluator.evaluate("2 3 +"), 5);
}

// ����: ���������� ����� �������� ������������ ��������� "2 3 4 * +"
TEST(PostfixTest, EvaluateComplex) {
    PostfixEvaluator evaluator;

    // 3 * 4 = 12, ����� 2 + 12 = 14
    EXPECT_EQ(evaluator.evaluate("2 3 4 * +"), 14);
}

// ����: ��������� ������ ��� ������������ ����������� ��������� "2 +"
TEST(PostfixTest, ThrowsOnInvalid) {
    PostfixEvaluator evaluator;

    // ������� ���������� runtime_error, ��� ��� ��������� ������������
    EXPECT_THROW(evaluator.evaluate("2 +"), runtime_error);
}
