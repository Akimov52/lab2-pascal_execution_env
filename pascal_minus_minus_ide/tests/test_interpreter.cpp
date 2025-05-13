#include <gtest/gtest.h>
#include "interpreter.h"
#include "ast.h"

// Проверка, что интерпретатор может обработать пустую программу без ошибок
TEST(InterpreterTest, RunEmptyProgram) {
    Interpreter interp;
    auto root = make_shared<ASTNode>(ASTNodeType::Program); // Создаём пустой узел программы
    EXPECT_NO_THROW(interp.run(root)); // Ожидаем, что выполнение не вызовет исключений
}

// Проверка присваивания и получения значения переменной
TEST(InterpreterTest, AssignmentAndGetVarValue) {
    Interpreter interp;
    // Создаём узел присваивания: x := 5;
    auto assign = make_shared<ASTNode>(ASTNodeType::Assignment);
    auto id = make_shared<ASTNode>(ASTNodeType::Identifier); // Идентификатор переменной
    id->value = "x";
    auto val = make_shared<ASTNode>(ASTNodeType::Number); // Значение 5
    val->value = "5";
    assign->children = {id, val}; // Добавляем детей в узел присваивания
    interp.run(assign); // Выполняем присваивание
    EXPECT_EQ(interp.getVarValue("x"), 5); // Проверяем, что переменная x получила значение 5
}

// Проверка ветвления if-else
TEST(InterpreterTest, IfElseBranching) {
    Interpreter interp;
    // if (true) then x := 1 else x := 2
    auto ifNode = make_shared<ASTNode>(ASTNodeType::If);
    auto cond = make_shared<ASTNode>(ASTNodeType::Number); // Условие true (1)
    cond->value = "1";

    // then-ветка: x := 1
    auto thenAssign = make_shared<ASTNode>(ASTNodeType::Assignment);
    auto id = make_shared<ASTNode>(ASTNodeType::Identifier); id->value = "x";
    auto val1 = make_shared<ASTNode>(ASTNodeType::Number); val1->value = "1";
    thenAssign->children = {id, val1};

    // else-ветка: x := 2
    auto elseAssign = make_shared<ASTNode>(ASTNodeType::Assignment);
    auto id2 = make_shared<ASTNode>(ASTNodeType::Identifier); id2->value = "x";
    auto val2 = make_shared<ASTNode>(ASTNodeType::Number); val2->value = "2";
    elseAssign->children = {id2, val2};

    ifNode->children = {cond, thenAssign, elseAssign}; // Добавляем условие и обе ветки
    interp.run(ifNode); // Выполняем if
    EXPECT_EQ(interp.getVarValue("x"), 1); // Проверяем, что была выполнена then-ветка
}

// Проверка цикла while
TEST(InterpreterTest, WhileLoop) {
    Interpreter interp;
    // x := 0;
    auto assign = make_shared<ASTNode>(ASTNodeType::Assignment);
    auto id = make_shared<ASTNode>(ASTNodeType::Identifier); id->value = "x";
    auto val = make_shared<ASTNode>(ASTNodeType::Number); val->value = "0";
    assign->children = {id, val};
    interp.run(assign); // Устанавливаем начальное значение x

    // while (x < 3) do x := x + 1;
    auto whileNode = make_shared<ASTNode>(ASTNodeType::While);

    // Условие x < 3
    auto cond = make_shared<ASTNode>(ASTNodeType::BinOp); cond->value = "<";
    auto idCond = make_shared<ASTNode>(ASTNodeType::Identifier); idCond->value = "x";
    auto valCond = make_shared<ASTNode>(ASTNodeType::Number); valCond->value = "3";
    cond->children = {idCond, valCond};

    // Тело цикла: x := x + 1
    auto body = make_shared<ASTNode>(ASTNodeType::Assignment);
    auto idBody = make_shared<ASTNode>(ASTNodeType::Identifier); idBody->value = "x";
    auto binop = make_shared<ASTNode>(ASTNodeType::BinOp); binop->value = "+";
    auto idBody2 = make_shared<ASTNode>(ASTNodeType::Identifier); idBody2->value = "x";
    auto one = make_shared<ASTNode>(ASTNodeType::Number); one->value = "1";
    binop->children = {idBody2, one};
    body->children = {idBody, binop};

    whileNode->children = {cond, body}; // Добавляем условие и тело в узел while
    interp.run(whileNode); // Запускаем цикл
    EXPECT_EQ(interp.getVarValue("x"), 3); // Проверяем результат: x должен быть равен 3
}

// Проверка корректной оценки выражений разных типов
TEST(InterpreterTest, EvaluateExpressionTypes) {
    Interpreter interp;
    // Целое число
    auto n = make_shared<ASTNode>(ASTNodeType::Number); n->value = "7";
    EXPECT_EQ(interp.evaluateExpression(n).intValue, 7); // Проверяем значение

    // Строка
    auto s = make_shared<ASTNode>(ASTNodeType::String); s->value = "hello";
    EXPECT_EQ(interp.evaluateExpression(s).strValue, "hello"); // Проверяем строку
}

// Проверка конкатенации строк с помощью +
TEST(InterpreterTest, BinOpStringConcat) {
    Interpreter interp;
    auto binop = make_shared<ASTNode>(ASTNodeType::BinOp); binop->value = "+";
    auto l = make_shared<ASTNode>(ASTNodeType::String); l->value = "A";
    auto r = make_shared<ASTNode>(ASTNodeType::String); r->value = "B";
    binop->children = {l, r}; // A + B
    EXPECT_EQ(interp.evaluateExpression(binop).strValue, "AB"); // Ожидаем "AB"
}

// Проверка арифметики и логики: + и and
TEST(InterpreterTest, BinOpArithmeticAndLogic) {
    Interpreter interp;

    // 2 + 2
    auto plus = make_shared<ASTNode>(ASTNodeType::BinOp); plus->value = "+";
    auto l = make_shared<ASTNode>(ASTNodeType::Number); l->value = "2";
    auto r = make_shared<ASTNode>(ASTNodeType::Number); r->value = "2";
    plus->children = {l, r};
    EXPECT_EQ(interp.evaluateExpression(plus).intValue, 4); // Ожидаем 4

    // 1 and 0
    auto andNode = make_shared<ASTNode>(ASTNodeType::BinOp); andNode->value = "and";
    auto l2 = make_shared<ASTNode>(ASTNodeType::Number); l2->value = "1";
    auto r2 = make_shared<ASTNode>(ASTNodeType::Number); r2->value = "0";
    andNode->children = {l2, r2};
    EXPECT_FALSE(interp.evaluateExpression(andNode).boolValue); // Ожидаем false
}

// Проверка унарных операций: отрицание и логическое not
TEST(InterpreterTest, UnOpNegateAndNot) {
    Interpreter interp;

    // -5
    auto neg = make_shared<ASTNode>(ASTNodeType::UnOp); neg->value = "-";
    auto n = make_shared<ASTNode>(ASTNodeType::Number); n->value = "5";
    neg->children = {n};
    EXPECT_EQ(interp.evaluateExpression(neg).intValue, -5); // Ожидаем -5

    // not 0
    auto notNode = make_shared<ASTNode>(ASTNodeType::UnOp); notNode->value = "not";
    auto zero = make_shared<ASTNode>(ASTNodeType::Number); zero->value = "0";
    notNode->children = {zero};
    EXPECT_TRUE(interp.evaluateExpression(notNode).boolValue); // not false = true
}

// Проверка, что попытка получить значение несуществующей переменной вызывает исключение
TEST(InterpreterTest, ThrowsOnUnknownVariable) {
    Interpreter interp;
    auto id = make_shared<ASTNode>(ASTNodeType::Identifier); id->value = "not_exist";
    EXPECT_THROW(interp.evaluateExpression(id), runtime_error); // Ожидаем исключение
}
