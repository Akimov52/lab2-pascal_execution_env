#include <gtest/gtest.h>
#include "postfix.h"

TEST(PostfixCalculatorTest, SimpleSum) {
    PostfixCalculator calc;
    std::vector<std::string> postfix = {"2", "3", "+"};
    EXPECT_DOUBLE_EQ(calc.eval(postfix), 5.0);
}

// TODO: Добавить тесты на преобразование в постфиксную форму
