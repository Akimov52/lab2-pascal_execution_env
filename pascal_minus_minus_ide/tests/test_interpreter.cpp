#include <gtest/gtest.h>
#include "interpreter.h"
#include "ast.h"

TEST(InterpreterTest, RunEmptyProgram) {
    Interpreter interp;
    auto root = std::make_shared<ASTNode>(ASTNodeType::Program);
    EXPECT_NO_THROW(interp.run(root));
}

// TODO: Добавить тесты на выполнение выражений и операторов
