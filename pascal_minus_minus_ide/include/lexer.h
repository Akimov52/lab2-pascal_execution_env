#ifndef LEXER_H
#define LEXER_H
#include <string>
#include <vector>

struct Token {
    std::string type;
    std::string value;
};

class Lexer {
public:
    std::vector<Token> tokenize(const std::string& source);
};

#endif // LEXER_H
