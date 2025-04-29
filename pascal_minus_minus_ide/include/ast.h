#ifndef AST_H
#define AST_H
#include <string>
#include <vector>
#include <memory>

// Типы узлов AST
enum class ASTNodeType {
    Program, ConstDecl, VarDecl, Assignment, If, While, Block, Write, Read, Expression, Number, Identifier, BinOp
};

struct ASTNode {
    ASTNodeType type;
    std::string value;
    std::vector<std::shared_ptr<ASTNode>> children;
    ASTNode(ASTNodeType t, const std::string& v = "") : type(t), value(v) {}
};

#endif // AST_H
