#ifndef POSTFIX_H
#define POSTFIX_H

#include <string>
#include <vector>

using namespace std;

// Класс для вычисления выражений в обратной польской записи (ОПЗ, postfix notation)
class PostfixCalculator {
public:
    // Вычислить значение выражения, представленного в виде вектора токенов (postfix)
    double eval(const vector<string>& tokens);

    // Преобразовать инфиксное выражение (как строку) в вектор токенов в постфиксной форме
    vector<string> to_postfix(const string& expr);
};

#endif // POSTFIX_H