#include <gtest.h>
#include "parser.h"
#include "lexer.h"
#include "error_reporter.h"
#include <memory>

class ParserTest : public ::testing::Test {
protected:
    std::shared_ptr<IErrorReporter> errorReporter;

    void SetUp() override {
        errorReporter = std::make_shared<ErrorReporter>();
    }
    
    std::vector<Token> tokenize(const std::string& source) {
        Lexer lexer(source, errorReporter);
        return lexer.tokenize();
    }
};

TEST_F(ParserTest, ParseEmptyProgram) {
    std::string source = "program Test; begin end.";
    std::vector<Token> tokens = tokenize(source);
    
    Parser parser(tokens, errorReporter);
    auto ast = parser.parse();
    
    ASSERT_NE(nullptr, ast);
    EXPECT_EQ(ASTNodeType::Program, ast->type);
    EXPECT_EQ("Test", ast->value);
    
    ASSERT_EQ(1, ast->children.size()); // Should have a block node
    
    auto blockNode = ast->children[0];
    EXPECT_EQ(ASTNodeType::Block, blockNode->type);
    EXPECT_EQ(0, blockNode->children.size()); // Empty block
}

TEST_F(ParserTest, ParseVariableDeclaration) {
    std::string source = "program Test; var x: Integer; begin end.";
    std::vector<Token> tokens = tokenize(source);
    
    Parser parser(tokens, errorReporter);
    auto ast = parser.parse();
    
    ASSERT_NE(nullptr, ast);
    ASSERT_GE(ast->children.size(), 2); // Should have var section and block node
    
    // Find var section
    bool found = false;
    for (auto& child : ast->children) {
        if (child->type == ASTNodeType::VarSection) {
            found = true;
            ASSERT_EQ(1, child->children.size());
            
            auto varDecl = child->children[0];
            EXPECT_EQ(ASTNodeType::VarDecl, varDecl->type);
            EXPECT_EQ("x", varDecl->value);
            // Здесь предполагается, что тип хранится в дочернем узле или в value
            // Нужно адаптировать в зависимости от реальной реализации
            break;
        }
    }
    EXPECT_TRUE(found);
}

TEST_F(ParserTest, ParseIfStatement) {
    std::string source = "program Test; var x: Integer; begin if x = 10 then x := 20; end.";
    std::vector<Token> tokens = tokenize(source);
    
    Parser parser(tokens, errorReporter);
    auto ast = parser.parse();
    
    ASSERT_NE(nullptr, ast);
    
    // Find block node
    std::shared_ptr<ASTNode> blockNode;
    for (const auto& child : ast->children) {
        if (child->type == ASTNodeType::Block) {
            blockNode = child;
            break;
        }
    }
    
    ASSERT_NE(nullptr, blockNode);
    ASSERT_EQ(1, blockNode->children.size()); // One if statement
    
    auto ifNode = blockNode->children[0];
    EXPECT_EQ(ASTNodeType::If, ifNode->type);
    
    ASSERT_EQ(2, ifNode->children.size()); // Condition and then-statement
    
    auto conditionNode = ifNode->children[0];
    EXPECT_EQ(ASTNodeType::BinOp, conditionNode->type);
    EXPECT_EQ("=", conditionNode->value);
    
    auto thenNode = ifNode->children[1];
    EXPECT_EQ(ASTNodeType::Assignment, thenNode->type);
}

TEST_F(ParserTest, ParseWhileLoop) {
    std::string source = "program Test; var x: Integer; begin while x < 10 do x := x + 1; end.";
    std::vector<Token> tokens = tokenize(source);
    
    Parser parser(tokens, errorReporter);
    auto ast = parser.parse();
    
    ASSERT_NE(nullptr, ast);
    
    // Find block node
    std::shared_ptr<ASTNode> blockNode;
    for (const auto& child : ast->children) {
        if (child->type == ASTNodeType::Block) {
            blockNode = child;
            break;
        }
    }
    
    ASSERT_NE(nullptr, blockNode);
    ASSERT_EQ(1, blockNode->children.size()); // One while statement
    
    auto whileNode = blockNode->children[0];
    EXPECT_EQ(ASTNodeType::While, whileNode->type);
    
    ASSERT_EQ(2, whileNode->children.size()); // Condition and loop-statement
    
    auto conditionNode = whileNode->children[0];
    EXPECT_EQ(ASTNodeType::BinOp, conditionNode->type);
    EXPECT_EQ("<", conditionNode->value);
    
    auto bodyNode = whileNode->children[1];
    EXPECT_EQ(ASTNodeType::Assignment, bodyNode->type);
}

TEST_F(ParserTest, ParseForLoop) {
    std::string source = "program Test; var i: Integer; begin for i := 1 to 10 do i := i * 2; end.";
    std::vector<Token> tokens = tokenize(source);
    
    Parser parser(tokens, errorReporter);
    auto ast = parser.parse();
    
    ASSERT_NE(nullptr, ast);
    
    // Find block node
    std::shared_ptr<ASTNode> blockNode;
    for (const auto& child : ast->children) {
        if (child->type == ASTNodeType::Block) {
            blockNode = child;
            break;
        }
    }
    
    ASSERT_NE(nullptr, blockNode);
    ASSERT_EQ(1, blockNode->children.size()); // One for statement
    
    auto forNode = blockNode->children[0];
    EXPECT_EQ(ASTNodeType::ForLoop, forNode->type);
    std::string expectedValue = "i";
    EXPECT_EQ(expectedValue, forNode->value); // Предполагается, что имя переменной хранится в value
    
    ASSERT_EQ(3, forNode->children.size()); // Start, end, and body
}

TEST_F(ParserTest, ParseBlockWithMultipleStatements) {
    std::string source = 
        "program Test;\n"
        "var x, y: Integer;\n"
        "begin\n"
        "  x := 10;\n"
        "  y := 20;\n"
        "  if x < y then\n"
        "    writeln('x is less than y');\n"
        "end.";
    
    std::vector<Token> tokens = tokenize(source);
    
    Parser parser(tokens, errorReporter);
    auto ast = parser.parse();
    
    ASSERT_NE(nullptr, ast);
    
    // Find block node
    std::shared_ptr<ASTNode> blockNode;
    for (const auto& child : ast->children) {
        if (child->type == ASTNodeType::Block) {
            blockNode = child;
            break;
        }
    }
    
    ASSERT_NE(nullptr, blockNode);
    ASSERT_EQ(3, blockNode->children.size()); // Three statements in the block
    
    // First two statements should be assignments
    EXPECT_EQ(ASTNodeType::Assignment, blockNode->children[0]->type);
    EXPECT_EQ(ASTNodeType::Assignment, blockNode->children[1]->type);
    
    // Third statement should be an if statement
    EXPECT_EQ(ASTNodeType::If, blockNode->children[2]->type);
}
