#include "postfix.h"
#include <stack>
#include <sstream>
#include <stdexcept>
#include <cctype>
#include <map>

using namespace std;

// Вспомогательная функция: возвращает true, если строка — число (целое или вещественное)
static bool is_number(const string& s) {
    if (s.empty()) return false;
    istringstream iss(s);
    double d;
    iss >> d;
    return iss.eof() && !iss.fail();
}

// Вычисляет значение выражения в постфиксной записи (Reverse Polish Notation)
// Использует стек для хранения промежуточных результатов
double PostfixCalculator::eval(const vector<string>& tokens) {
    stack<double> st;
    for (const auto& token : tokens) {
        if (is_number(token)) {
            // Если токен — число, помещаем в стек
            st.push(stod(token));
        } else if (token == "+" || token == "-" || token == "*" || token == "/" ||
                   token == "^") {
            // Если токен — оператор, извлекаем два значения из стека и применяем оператор
            if (st.size() < 2) throw runtime_error("Недостаточно операндов");
            double b = st.top(); st.pop();
            double a = st.top(); st.pop();
            if (token == "+") st.push(a + b);
            else if (token == "-") st.push(a - b);
            else if (token == "*") st.push(a * b);
            else if (token == "/") st.push(a / b);
            else if (token == "^") st.push(pow(a, b));
        } else {
            // Если токен не распознан — ошибка
            throw runtime_error("Неизвестный токен в постфиксном выражении: " + token);
        }
    }
    // После вычисления в стеке должен остаться ровно один результат
    if (st.size() != 1) throw runtime_error("Ошибка вычисления постфиксного выражения");
    return st.top();
}

// Возвращает приоритет оператора (чем выше число — тем выше приоритет)
static int precedence(const string& op) {
    if (op == "+" || op == "-") return 1;
    if (op == "*" || op == "/") return 2;
    if (op == "^") return 3;
    return 0;
}

// Проверяет, является ли строка оператором (+, -, *, /, ^)
static bool is_operator(const string& op) {
    return op == "+" || op == "-" || op == "*" || op == "/" || op == "^";
}

// Разделяет строку на токены (числа, операторы, скобки)
// Пример: "2+3*(4-1)" -> ["2", "+", "3", "*", "(", "4", "-", "1", ")"]
static vector<string> tokenize(const string& expr) {
    vector<string> tokens;
    string num;
    for (size_t i = 0; i < expr.size(); ++i) {
        char c = expr[i];
        if (isspace(c)) continue; // пропускаем пробелы
        if (isdigit(c) || c == '.') {
            // Если цифра или точка — продолжаем накапливать число
            num += c;
        } else {
            // Если встретили оператор или скобку — сначала добавляем накопленное число
            if (!num.empty()) {
                tokens.push_back(num);
                num.clear();
            }
            // Добавляем оператор или скобку как отдельный токен
            if (is_operator(string(1, c)) || c == '(' || c == ')') {
                tokens.push_back(string(1, c));
            }
        }
    }
    // Добавляем последнее число, если оно есть
    if (!num.empty()) tokens.push_back(num);
    return tokens;
}

// Переводит инфиксное выражение в постфиксное (алгоритм сортировочной станции Дейкстры)
// Например, "2+3*4" -> ["2", "3", "4", "*", "+"]
vector<string> PostfixCalculator::to_postfix(const string& expr) {
    vector<string> output;      // Выходная очередь (постфиксная запись)
    stack<string> ops;          // Стек операторов
    vector<string> tokens = tokenize(expr); // Токенизация исходного выражения

    for (const auto& token : tokens) {
        if (is_number(token)) {
            // Если токен — число, сразу в выходную очередь
            output.push_back(token);
        } else if (is_operator(token)) {
            // Если токен — оператор, учитываем приоритеты и ассоциативность
            while (!ops.empty() && is_operator(ops.top()) &&
                   ((precedence(token) < precedence(ops.top())) ||
                    (precedence(token) == precedence(ops.top()) && token != "^"))) {
                output.push_back(ops.top());
                ops.pop();
            }
            ops.push(token);
        } else if (token == "(") {
            // Открывающая скобка — просто в стек операторов
            ops.push(token);
        } else if (token == ")") {
            // Закрывающая скобка — выталкиваем операторы до открывающей скобки
            while (!ops.empty() && ops.top() != "(") {
                output.push_back(ops.top());
                ops.pop();
            }
            if (!ops.empty() && ops.top() == "(") ops.pop();
            else throw runtime_error("Несогласованные скобки");
        } else {
            // Неизвестный токен — ошибка
            throw runtime_error("Неизвестный токен: " + token);
        }
    }
    // Выталкиваем оставшиеся операторы в выходную очередь
    while (!ops.empty()) {
        if (ops.top() == "(" || ops.top() == ")")
            throw runtime_error("Несогласованные скобки");
        output.push_back(ops.top());
        ops.pop();
    }
    return output;
}