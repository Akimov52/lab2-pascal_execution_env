#include <gtest.h>
#include "lexer.h"
#include "error_reporter.h"
#include <memory>

class LexerTest : public ::testing::Test {
protected:
    std::shared_ptr<IErrorReporter> errorReporter;

    void SetUp() override {
        errorReporter = std::make_shared<ErrorReporter>();
    }
};

TEST_F(LexerTest, TokenizeEmptyString) {
    Lexer lexer("", errorReporter);
    auto tokens = lexer.tokenize();
    ASSERT_EQ(1, tokens.size());
    EXPECT_EQ(TokenType::EndOfFile, tokens[0].type);
}

TEST_F(LexerTest, TokenizeKeywords) {
    std::string source = "program var const begin end if then else while do";
    Lexer lexer(source, errorReporter);
    auto tokens = lexer.tokenize();
    
    ASSERT_EQ(11, tokens.size()); // 10 keywords + EOF
    
    EXPECT_EQ(TokenType::Program, tokens[0].type);
    EXPECT_EQ(TokenType::Var, tokens[1].type);
    EXPECT_EQ(TokenType::Const, tokens[2].type);
    EXPECT_EQ(TokenType::Begin, tokens[3].type);
    EXPECT_EQ(TokenType::End, tokens[4].type);
    EXPECT_EQ(TokenType::If, tokens[5].type);
    EXPECT_EQ(TokenType::Then, tokens[6].type);
    EXPECT_EQ(TokenType::Else, tokens[7].type);
    EXPECT_EQ(TokenType::While, tokens[8].type);
    EXPECT_EQ(TokenType::Do, tokens[9].type);
    EXPECT_EQ(TokenType::EndOfFile, tokens[10].type);
}

TEST_F(LexerTest, TokenizeIdentifiers) {
    std::string source = "identifier1 _under2 camelCase PascalCase";
    Lexer lexer(source, errorReporter);
    auto tokens = lexer.tokenize();
    
    ASSERT_EQ(5, tokens.size()); // 4 identifiers + EOF
    
    EXPECT_EQ(TokenType::Identifier, tokens[0].type);
    EXPECT_EQ("identifier1", tokens[0].value);
    
    EXPECT_EQ(TokenType::Identifier, tokens[1].type);
    EXPECT_EQ("_under2", tokens[1].value);
    
    EXPECT_EQ(TokenType::Identifier, tokens[2].type);
    EXPECT_EQ("camelCase", tokens[2].value);
    
    EXPECT_EQ(TokenType::Identifier, tokens[3].type);
    EXPECT_EQ("PascalCase", tokens[3].value);
}

TEST_F(LexerTest, TokenizeNumbers) {
    std::string source = "123 45.67 0 9.0";
    Lexer lexer(source, errorReporter);
    auto tokens = lexer.tokenize();
    
    ASSERT_EQ(5, tokens.size()); // 4 numbers + EOF
    
    EXPECT_EQ(TokenType::Number, tokens[0].type);
    EXPECT_EQ("123", tokens[0].value);
    
    EXPECT_EQ(TokenType::RealLiteral, tokens[1].type);
    EXPECT_EQ("45.67", tokens[1].value);
    
    EXPECT_EQ(TokenType::Number, tokens[2].type);
    EXPECT_EQ("0", tokens[2].value);
    
    EXPECT_EQ(TokenType::RealLiteral, tokens[3].type);
    EXPECT_EQ("9.0", tokens[3].value);
}

TEST_F(LexerTest, TokenizeStrings) {
    std::string source = "'Hello, world!' 'Another string' ''";
    Lexer lexer(source, errorReporter);
    auto tokens = lexer.tokenize();
    
    ASSERT_EQ(4, tokens.size()); // 3 strings + EOF
    
    EXPECT_EQ(TokenType::StringLiteral, tokens[0].type);
    EXPECT_EQ("Hello, world!", tokens[0].value);
    
    EXPECT_EQ(TokenType::StringLiteral, tokens[1].type);
    EXPECT_EQ("Another string", tokens[1].value);
    
    EXPECT_EQ(TokenType::StringLiteral, tokens[2].type);
    EXPECT_EQ("", tokens[2].value);
}

TEST_F(LexerTest, TokenizePascalProgram) {
    std::string source = 
        "program TestProgram;\n"
        "var\n"
        "  x, y: Integer;\n"
        "begin\n"
        "  x := 10;\n"
        "  y := x * 2;\n"
        "  writeln('Result: ', y);\n"
        "end.";
    
    Lexer lexer(source, errorReporter);
    auto tokens = lexer.tokenize();
    
    // Verify program structure tokens (not testing every token)
    ASSERT_GT(tokens.size(), 10);
    
    // Check specific tokens
    EXPECT_EQ(TokenType::Program, tokens[0].type);
    EXPECT_EQ(TokenType::Identifier, tokens[1].type);
    EXPECT_EQ("TestProgram", tokens[1].value);
    EXPECT_EQ(TokenType::Semicolon, tokens[2].type);
    
    // Find 'begin' token position
    int beginPos = -1;
    for (size_t i = 0; i < tokens.size(); i++) {
        if (tokens[i].type == TokenType::Begin) {
            beginPos = i;
            break;
        }
    }
    
    ASSERT_NE(-1, beginPos);
    
    // Check line and column tracking
    EXPECT_GT(tokens[beginPos].line, 1); // begin should be after line 1
}