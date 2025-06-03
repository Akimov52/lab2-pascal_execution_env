#include <gtest.h>
#include "interpreter.h"
#include "parser.h"
#include "lexer.h"
#include "error_reporter.h"
#include <memory>
#include <sstream>

class InterpreterTest : public ::testing::Test {
protected:
    std::shared_ptr<IErrorReporter> errorReporter;
    std::shared_ptr<Interpreter> interpreter;
    
    void SetUp() override {
        errorReporter = std::make_shared<ErrorReporter>();
        interpreter = std::make_shared<Interpreter>(errorReporter);
    }
    
    // Helper method to parse and interpret a program
    void interpretProgram(const std::string& source) {
        // Tokenize
        Lexer lexer(source, errorReporter);
        std::vector<Token> tokens = lexer.tokenize();
        
        // Parse
        Parser parser(tokens, errorReporter);
        std::shared_ptr<ASTNode> ast = parser.parse();
        
        // Interpret
        interpreter->run(ast);
    }
    
    // Helper method to get the current value of a variable
    Value getVariableValue(const std::string& name) {
        return interpreter->getVariable(name);
    }
};

TEST_F(InterpreterTest, InterpretAssignment) {
    std::string source = 
        "program Test;\n"
        "var x: Integer;\n"
        "begin\n"
        "  x := 42;\n"
        "end.";
    
    interpretProgram(source);
    
    // Check if variable x was set to 42
    Value value = getVariableValue("x");
    EXPECT_EQ(ValueType::Integer, value.type);
    EXPECT_EQ(42, value.intValue);
}

TEST_F(InterpreterTest, InterpretArithmeticOperations) {
    std::string source = 
        "program Test;\n"
        "var a, b, c, d, e: Integer;\n"
        "begin\n"
        "  a := 10 + 5;\n"
        "  b := 20 - 8;\n"
        "  c := 6 * 7;\n"
        "  d := 100 div 5;\n"
        "  e := 17 mod 5;\n"
        "end.";
    
    interpretProgram(source);
    
    // Check results of arithmetic operations
    EXPECT_EQ(15, getVariableValue("a").intValue); // 10 + 5 = 15
    EXPECT_EQ(12, getVariableValue("b").intValue); // 20 - 8 = 12
    EXPECT_EQ(42, getVariableValue("c").intValue); // 6 * 7 = 42
    EXPECT_EQ(20, getVariableValue("d").intValue); // 100 div 5 = 20
    EXPECT_EQ(2, getVariableValue("e").intValue);  // 17 mod 5 = 2
}

TEST_F(InterpreterTest, InterpretIfStatement) {
    std::string source = 
        "program Test;\n"
        "var x, y: Integer;\n"
        "begin\n"
        "  x := 10;\n"
        "  if x > 5 then\n"
        "    y := 1\n"
        "  else\n"
        "    y := 0;\n"
        "end.";
    
    interpretProgram(source);
    
    // Check that y was set to 1 (condition is true)
    EXPECT_EQ(1, getVariableValue("y").intValue);
}

TEST_F(InterpreterTest, InterpretIfStatementFalse) {
    std::string source = 
        "program Test;\n"
        "var x, y: Integer;\n"
        "begin\n"
        "  x := 3;\n"
        "  if x > 5 then\n"
        "    y := 1\n"
        "  else\n"
        "    y := 0;\n"
        "end.";
    
    interpretProgram(source);
    
    // Check that y was set to 0 (condition is false)
    EXPECT_EQ(0, getVariableValue("y").intValue);
}

TEST_F(InterpreterTest, InterpretWhileLoop) {
    std::string source = 
        "program Test;\n"
        "var i, sum: Integer;\n"
        "begin\n"
        "  i := 1;\n"
        "  sum := 0;\n"
        "  while i <= 5 do\n"
        "  begin\n"
        "    sum := sum + i;\n"
        "    i := i + 1;\n"
        "  end;\n"
        "end.";
    
    interpretProgram(source);
    
    // Check that sum is 1+2+3+4+5 = 15
    EXPECT_EQ(15, getVariableValue("sum").intValue);
    // Check that i ended at 6
    EXPECT_EQ(6, getVariableValue("i").intValue);
}


TEST_F(InterpreterTest, InterpretNestedStatements) {
    std::string source = 
        "program Test;\n"
        "var i, j, sum: Integer;\n"
        "begin\n"
        "  sum := 0;\n"
        "  for i := 1 to 3 do\n"
        "  begin\n"
        "    for j := 1 to 2 do\n"
        "    begin\n"
        "      sum := sum + (i * j);\n"
        "    end;\n"
        "  end;\n"
        "end.";
    
    interpretProgram(source);
    
    // Calculate: (1*1)+(1*2)+(2*1)+(2*2)+(3*1)+(3*2) = 1+2+2+4+3+6 = 18
    EXPECT_EQ(18, getVariableValue("sum").intValue);
}

TEST_F(InterpreterTest, InterpretBooleanOperations) {
    std::string source = 
        "program Test;\n"
        "var a, b, c, d: Boolean;\n"
        "begin\n"
        "  a := true and true;\n"
        "  b := true or false;\n"
        "  c := not true;\n"
        "  d := (5 > 3) and (10 <> 20);\n"
        "end.";
    
    interpretProgram(source);
    
    // Check boolean operations
    EXPECT_TRUE(getVariableValue("a").boolValue);  // true and true = true
    EXPECT_TRUE(getVariableValue("b").boolValue);  // true or false = true
    EXPECT_FALSE(getVariableValue("c").boolValue); // not true = false
    EXPECT_TRUE(getVariableValue("d").boolValue);  // (true) and (true) = true
}