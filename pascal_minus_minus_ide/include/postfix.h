#ifndef POSTFIX_H
#define POSTFIX_H
#include <string>
#include <vector>
#include <variant>

class PostfixCalculator {
public:
    double eval(const std::vector<std::string>& tokens);
    std::vector<std::string> to_postfix(const std::string& expr);
};

#endif // POSTFIX_H
