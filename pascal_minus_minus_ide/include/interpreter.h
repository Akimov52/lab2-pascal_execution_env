#ifndef INTERPRETER_H
#define INTERPRETER_H
#include "ast.h"
#include "symbol_table.h"
#include "postfix.h"
#include <memory>

class Interpreter {
public:
    Interpreter();
    void run(const std::shared_ptr<ASTNode>& root);
private:
    SymbolTable symbols;
    PostfixCalculator postfix;
};

#endif // INTERPRETER_H
