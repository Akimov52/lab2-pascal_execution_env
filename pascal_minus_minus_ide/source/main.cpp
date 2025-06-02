#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <windows.h>
#include <memory>

#include "parser.h"
#include "interpreter.h"
#include "error_reporter.h"
#include "postfix.h"
#include <symbol_table.h>

// Используем using namespace std для упрощения кода
using namespace std;

// Очистить экран консоли (Windows)
void clearConsole() {
    system("cls");
}

// Вывести меню команд для пользователя
void printMenu() {
    cout << "Добро пожаловать в Pascal-- IDE!" << endl;
    cout << "Доступные команды:" << endl;
    cout << "  new         - создать новый файл программы" << endl;
    cout << "  open <имя>  - открыть программу из файла" << endl;
    cout << "  save <имя>  - сохранить программу в файл" << endl;
    cout << "  edit        - перейти в режим редактирования" << endl;
    cout << "  run         - выполнить программу" << endl;
    cout << "  check       - проверить синтаксис" << endl;
    cout << "  vars        - показать значения переменных" << endl;
    cout << "  exit        - выйти из IDE" << endl;
    cout << endl << "Введите команду:" << endl;
}

// Установить кодировку консоли для корректного отображения русских символов
void setConsoleEncoding() {
    // Устанавливаем кодовую страницу консоли в 1251 (Windows-1251)
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);
    setlocale(LC_ALL, "Russian");

    setlocale(LC_NUMERIC, "C");

    // Очищаем экран
    system("cls");
}

int main() {
    // Устанавливаем кодировку консоли
    setConsoleEncoding();

    // Основные переменные для хранения команд, исходного кода, имени файла и состояния
    string source, filename;
    bool running = true;   // Флаг для основного цикла программы
    bool edited = false;   // Флаг наличия несохранённых изменений
    // Указатель на последний запущенный интерпретатор — нужен для команды vars
    IInterpreter* lastInterp = nullptr;

    // Главный цикл работы IDE
    while (running) {
        string command;
        clearConsole();    // Очищаем экран перед каждой командой
        printMenu();       // Показываем меню
        cin.clear();
        getline(cin, command);

        // Создание нового файла программы
        if (command == "new") {
            clearConsole();
            cout << "Введите исходный код программы (введите 'end' на новой строке для завершения):" << endl;
            source.clear();
            string line;
            while (getline(cin, line) && line != "end")
                source += line + "\n";
            edited = true;
            cout << endl << "Программа сохранена в буфере." << endl;
            system("pause");
        }
        // Открытие программы из файла
        else if (command.find("open ") == 0) {
            string fname = command.substr(5);
            if (fname.empty())
                cout << "Укажите имя файла после команды open." << endl;
            else {
                ifstream file(fname);
                if (file.is_open()) {
                    source = string((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
                    file.close();
                    filename = fname;
                    edited = false;
                    cout << endl << "Файл успешно загружен." << endl;
                }
                else
                    cerr << endl << "Ошибка: Не удалось открыть файл." << endl;
            }
            system("pause");
        }
        // Сохранение программы в файл
        else if (command.rfind("save ", 0) == 0) {
            string fname = command.substr(5);
            if (fname.empty())
                cout << "Укажите имя файла после команды save." << endl;
            else {
                ofstream file(fname);
                if (file.is_open()) {
                    file << source;
                    filename = fname;
                    edited = false;
                    cout << endl << "Файл успешно сохранён." << endl;
                }
                else
                    cerr << endl << "Ошибка: Не удалось сохранить файл." << endl;
            }
            system("pause");
        }
        // Редактирование исходного кода программы
        else if (command == "edit") {
            clearConsole();
            cout << "Текущий исходный код:" << endl << endl << source << endl;
            cout << "Введите новый исходный код (введите 'end' на новой строке для завершения):" << endl;
            source.clear();
            string line;
            while (getline(cin, line) && line != "end")
                source += line + "\n";
            edited = true;
            cout << endl << "Изменения сохранены." << endl;
            system("pause");
        }
        // Проверка синтаксиса программы
        else if (command == "check" && !source.empty()) {
            try {
                cout << endl << "Проверка синтаксиса..." << endl;
                // Создаем обработчик ошибок
                auto errorReporter = std::make_shared<ErrorReporter>();
                // Инициализируем лексер
                Lexer lexer(source, errorReporter);
                auto tokens = lexer.tokenize();
                // Создаем парсер
                std::shared_ptr<IParser> parser = std::make_shared<Parser>(tokens, errorReporter);
                parser->parse();
                cout << endl << "Синтаксических ошибок не обнаружено." << endl;
            }
            catch (const exception& e) {
                cerr << endl << "Ошибка: " << e.what() << endl;
            }
            system("pause");
        }
        // Запуск программы (интерпретация)
        else if (command == "run" && !source.empty()) {
            try {
                cout << endl << "Запуск программы..." << endl << endl;
                // Создаем обработчик ошибок
                auto errorReporter = std::make_shared<ErrorReporter>();
                // Инициализируем лексер
                Lexer lexer(source, errorReporter);
                auto tokens = lexer.tokenize();
                // Создаем парсер
                std::shared_ptr<IParser> parser = std::make_shared<Parser>(tokens, errorReporter);
                auto ast = parser->parse();
                delete lastInterp;
                lastInterp = new Interpreter(errorReporter);
                lastInterp->run(ast);
                cout << endl << endl << "Программа завершила выполнение." << endl;
            }
            catch (const exception& e) {
                cerr << endl << "Ошибка выполнения: " << e.what() << endl;
            }
            system("pause");
        }
        // Показать значения переменных (функционал не реализован)
        else if (command == "vars") {
            cout << endl;
            if (lastInterp) {
                // Заполняем SymbolTable из symbols интерпретатора
                SymbolTable st;
                auto syms = lastInterp->getAllSymbols(); // map<string, Value>
                for (const auto& [name, val] : syms) {
                    // Подготовим строку типа и значения
                    string typeStr;
                    string valStr;
                    switch (val.type) {
                    case ValueType::Integer:
                        typeStr = "integer";
                        valStr = to_string(val.intValue);
                        break;
                    case ValueType::Real:
                        typeStr = "real";
                        valStr = to_string(val.realValue);
                        break;
                    case ValueType::Boolean:
                        typeStr = "boolean";
                        valStr = val.boolValue ? "true" : "false";
                        break;
                    case ValueType::String:
                        typeStr = "string";
                        valStr = val.stringValue;
                        break;
                    }
                    st.addSymbol(SymbolInfo{ name, typeStr, valStr });
                }
                // Выводим таблицу
                st.printTable();
            }
            else
                cout << "Программу ещё не запускали, переменных нет." << endl;
            system("pause");
        }
        // Выход из IDE с предупреждением о несохранённых изменениях
        else if (command == "exit") {
            if (edited) {
                cout << endl << "У вас есть несохраненные изменения. Вы уверены, что хотите выйти? (y/n): ";
                string answer;
                getline(cin, answer);
                if (answer == "y" || answer == "Y")
                    running = false;
            }
            else
                running = false;
        }
        // Неизвестная команда
        else {
            cout << endl << "Неизвестная команда." << endl;
            system("pause");
        }
    }
    delete lastInterp;
    return 0;
}