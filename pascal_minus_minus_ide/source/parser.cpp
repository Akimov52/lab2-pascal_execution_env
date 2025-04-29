#include "parser.h"
#include <memory>

std::shared_ptr<ASTNode> Parser::parse(const std::string& source) {
    // TODO: Реализовать синтаксический анализатор Pascal--
    return std::make_shared<ASTNode>(ASTNodeType::Program);
}
