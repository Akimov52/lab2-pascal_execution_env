#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "lexer.h"
#include "parser.h"
#include "interpreter.h"

int main(int argc, char* argv[]) {
    std::cout << "Pascal-- IDE (console prototype)\n";
    std::string filename;
    if (argc > 1) {
        filename = argv[1];
    } else {
        std::cout << "Введите имя файла программы: ";
        std::getline(std::cin, filename);
    }
    std::ifstream file(filename);
    if (!file) {
        std::cerr << "Ошибка открытия файла: " << filename << std::endl;
        return 1;
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string source = buffer.str();

    Lexer lexer;
    auto tokens = lexer.tokenize(source);
    Parser parser;
    auto ast = parser.parse(source);
    Interpreter interp;
    interp.run(ast);
    return 0;
}
