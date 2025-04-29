#ifndef PARSER_H
#define PARSER_H
#include "ast.h"
#include <string>
#include <memory>

class Parser {
public:
    std::shared_ptr<ASTNode> parse(const std::string& source);
};

#endif // PARSER_H
