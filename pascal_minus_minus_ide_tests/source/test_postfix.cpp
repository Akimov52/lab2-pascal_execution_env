#include <gtest.h>
#include "postfix.h"
#include "ast.h"
#include "interpreter.h"
#include <memory>
#include <map>

class PostfixTest : public ::testing::Test {
protected:
    PostfixCalculator calculator;
    std::map<std::string, Value> variables;
    
    void SetUp() override {
        // Initialize some variables for testing
        variables["a"] = Value(10);
        variables["b"] = Value(5);
        variables["c"] = Value(2.5);
        variables["flag"] = Value(true);
    }
    
    // Helper to create number literal AST node
    std::shared_ptr<ASTNode> createNumberNode(int value) {
        auto node = std::make_shared<ASTNode>(ASTNodeType::Number, std::to_string(value));
        return node;
    }
    
    // Helper to create variable reference AST node
    std::shared_ptr<ASTNode> createVarNode(const std::string& name) {
        auto node = std::make_shared<ASTNode>(ASTNodeType::Identifier, name);
        return node;
    }
    
    // Helper to create binary operation AST node
    std::shared_ptr<ASTNode> createBinaryOpNode(const std::string& op,
                                             std::shared_ptr<ASTNode> left,
                                             std::shared_ptr<ASTNode> right) {
        auto node = std::make_shared<ASTNode>(ASTNodeType::BinOp, op);
        node->children.push_back(left);
        node->children.push_back(right);
        return node;
    }
};

TEST_F(PostfixTest, EvaluateNumberLiteral) {
    auto node = createNumberNode(42);
    Value result = calculator.evaluate(node, variables);
    
    EXPECT_EQ(ValueType::Integer, result.type);
    EXPECT_EQ(42, result.intValue);
}

TEST_F(PostfixTest, EvaluateVariable) {
    auto node = createVarNode("a");
    Value result = calculator.evaluate(node, variables);
    
    EXPECT_EQ(ValueType::Integer, result.type);
    EXPECT_EQ(10, result.intValue);
}

TEST_F(PostfixTest, EvaluateAddition) {
    auto node = createBinaryOpNode("+",
                                createNumberNode(10),
                                createNumberNode(5));
    Value result = calculator.evaluate(node, variables);
    
    EXPECT_EQ(ValueType::Integer, result.type);
    EXPECT_EQ(15, result.intValue);
}

TEST_F(PostfixTest, EvaluateSubtraction) {
    auto node = createBinaryOpNode("-",
                                createNumberNode(10),
                                createNumberNode(5));
    Value result = calculator.evaluate(node, variables);
    
    EXPECT_EQ(ValueType::Integer, result.type);
    EXPECT_EQ(5, result.intValue);
}

TEST_F(PostfixTest, EvaluateMultiplication) {
    auto node = createBinaryOpNode("*",
                                createNumberNode(10),
                                createNumberNode(5));
    Value result = calculator.evaluate(node, variables);
    
    EXPECT_EQ(ValueType::Integer, result.type);
    EXPECT_EQ(50, result.intValue);
}

TEST_F(PostfixTest, EvaluateDivision) {
    auto node = createBinaryOpNode("/",
                                createNumberNode(10),
                                createNumberNode(5));
    Value result = calculator.evaluate(node, variables);
    
    EXPECT_EQ(ValueType::Real, result.type);
    EXPECT_DOUBLE_EQ(2.0, result.realValue);
}

TEST_F(PostfixTest, EvaluateIntegerDivision) {
    auto node = createBinaryOpNode("div",
                                createNumberNode(10),
                                createNumberNode(3));
    Value result = calculator.evaluate(node, variables);
    
    EXPECT_EQ(ValueType::Integer, result.type);
    EXPECT_EQ(3, result.intValue);
}

TEST_F(PostfixTest, EvaluateModulus) {
    auto node = createBinaryOpNode("mod",
                                createNumberNode(10),
                                createNumberNode(3));
    Value result = calculator.evaluate(node, variables);
    
    EXPECT_EQ(ValueType::Integer, result.type);
    EXPECT_EQ(1, result.intValue);
}

TEST_F(PostfixTest, EvaluateComplexExpression) {
    // Build AST for: (a + b) * c
    auto sumNode = createBinaryOpNode("+",
                                  createVarNode("a"),
                                  createVarNode("b"));
    auto mulNode = createBinaryOpNode("*",
                                  sumNode,
                                  createVarNode("c"));
    
    Value result = calculator.evaluate(mulNode, variables);
    
    EXPECT_EQ(ValueType::Real, result.type);
    // (10 + 5) * 2.5 = 37.5
    EXPECT_DOUBLE_EQ(37.5, result.realValue);
}

TEST_F(PostfixTest, EvaluateComparisonOperators) {
    // a > b (10 > 5)
    auto gtNode = createBinaryOpNode(">",
                                 createVarNode("a"),
                                 createVarNode("b"));
    Value gtResult = calculator.evaluate(gtNode, variables);
    EXPECT_EQ(ValueType::Boolean, gtResult.type);
    EXPECT_TRUE(gtResult.boolValue);
    
    // a < b (10 < 5)
    auto ltNode = createBinaryOpNode("<",
                                 createVarNode("a"),
                                 createVarNode("b"));
    Value ltResult = calculator.evaluate(ltNode, variables);
    EXPECT_EQ(ValueType::Boolean, ltResult.type);
    EXPECT_FALSE(ltResult.boolValue);
    
    // a = a (10 = 10)
    auto eqNode = createBinaryOpNode("=",
                                 createVarNode("a"),
                                 createVarNode("a"));
    Value eqResult = calculator.evaluate(eqNode, variables);
    EXPECT_EQ(ValueType::Boolean, eqResult.type);
    EXPECT_TRUE(eqResult.boolValue);
    
    // a <> b (10 <> 5)
    auto neNode = createBinaryOpNode("<>",
                                 createVarNode("a"),
                                 createVarNode("b"));
    Value neResult = calculator.evaluate(neNode, variables);
    EXPECT_EQ(ValueType::Boolean, neResult.type);
    EXPECT_TRUE(neResult.boolValue);
}

TEST_F(PostfixTest, EvaluateLogicalOperators) {
    // flag and true
    auto andNode = createBinaryOpNode("and",
                                  createVarNode("flag"),
                                  std::make_shared<ASTNode>(ASTNodeType::Boolean, "true"));
    Value andResult = calculator.evaluate(andNode, variables);
    EXPECT_EQ(ValueType::Boolean, andResult.type);
    EXPECT_TRUE(andResult.boolValue);
    
    // flag or false
    auto falseNode = std::make_shared<ASTNode>(ASTNodeType::Boolean, "false");
    auto orNode = createBinaryOpNode("or", createVarNode("flag"), falseNode);
    Value orResult = calculator.evaluate(orNode, variables);
    EXPECT_EQ(ValueType::Boolean, orResult.type);
    EXPECT_TRUE(orResult.boolValue);
    
    // not flag
    std::shared_ptr<ASTNode> flagNode = createVarNode("flag");
    auto notNode = std::make_shared<ASTNode>(ASTNodeType::UnOp, "not");
    notNode->children.push_back(flagNode);
    Value notResult = calculator.evaluate(notNode, variables);
    EXPECT_EQ(ValueType::Boolean, notResult.type);
    EXPECT_FALSE(notResult.boolValue);
}

TEST_F(PostfixTest, InfixToPostfixConversion) {
    // Test simple arithmetic conversion
    std::vector<std::string> infix = {"a", "+", "b", "*", "c"};
    std::vector<std::string> postfix = calculator.infixToPostfix(infix);
    
    // Expected: a b c * +
    std::vector<std::string> expected = {"a", "b", "c", "*", "+"};
    EXPECT_EQ(expected, postfix);
    
    // Test with parentheses
    infix = {"(", "a", "+", "b", ")", "*", "c"};
    postfix = calculator.infixToPostfix(infix);
    
    // Expected: a b + c *
    expected = {"a", "b", "+", "c", "*"};
    EXPECT_EQ(expected, postfix);
}

TEST_F(PostfixTest, EvaluatePostfixExpression) {
    // Evaluate a postfix expression: 3 4 + 5 *
    std::vector<std::string> postfix = {"3", "4", "+", "5", "*"};
    Value result = calculator.evaluatePostfix(postfix, variables);
    
    // (3 + 4) * 5 = 35
    EXPECT_EQ(ValueType::Integer, result.type);
    EXPECT_EQ(35, result.intValue);
}
