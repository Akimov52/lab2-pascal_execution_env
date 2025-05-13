#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstdlib>
#include <windows.h>
#include <io.h>
#include <fcntl.h>
#include <locale>
#include <codecvt>

#include "lexer.h"
#include "parser.h"
#include "interpreter.h"

// Используем TokenType из lexer.h
using PascalToken::TokenType;

using namespace std;

// Очистить экран консоли (Windows)
void clearConsole() {
    system("cls");
}

// Вывести меню команд для пользователя
void printMenu() {
    cout << "Добро пожаловать в Pascal-- IDE!\n";
    cout << "Доступные команды:\n";
    cout << "  new         - создать новый файл программы\n";
    cout << "  open <имя>  - открыть программу из файла\n";
    cout << "  save <имя>  - сохранить программу в файл\n";
    cout << "  edit        - перейти в режим редактирования\n";
    cout << "  run         - выполнить программу\n";
    cout << "  check       - проверить синтаксис\n";
    cout << "  vars        - показать значения переменных\n";
    cout << "  exit        - выйти из IDE\n";
    cout << "\nВведите команду:\n";
}

// Установить кодировку консоли для корректного отображения русских символов
void setConsoleEncoding() {
    // Устанавливаем кодовую страницу консоли в 1251 (Windows-1251)
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);
    setlocale(LC_ALL, "Russian");

    // Очищаем экран
    system("cls");
}

int main() {
    // Устанавливаем кодировку консоли
    setConsoleEncoding();
    
    // Выводим приветственное сообщение с текущей кодировкой консоли
    cout << "Pascal-- IDE запущена. Кодировка консоли: " << GetConsoleCP() << endl;
    
    // Основные переменные для хранения команд, исходного кода, имени файла и состояния
    string command;
    string source, filename;
    bool running = true;   // Флаг для основного цикла программы
    bool edited = false;   // Флаг наличия несохранённых изменений

    // Главный цикл работы IDE
    while (running) {
        clearConsole();    // Очищаем экран перед каждой командой
        printMenu();       // Показываем меню
        cin.clear();
        getline(cin, command);

        // Создание нового файла программы
        if (command == "new") {
            clearConsole();
            cout << "Введите исходный код программы (введите 'end' на новой строке для завершения):\n";
            source.clear();
            string line;
            while (getline(cin, line) && line != "end") {
                source += line + "\n";
            }
            edited = true;
            cout << "\nПрограмма сохранена в буфере.\n";
            system("pause");
        }
        // Открытие программы из файла
        else if (command.find("open ") == 0) {
            string fname = command.substr(5);
            if (fname.empty()) {
                cout << "Укажите имя файла после команды open.\n";
            } else {
                ifstream file(fname);
                if (file.is_open()) {
                    source = string((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
                    file.close();
                    filename = fname;
                    edited = false;
                    cout << "\nФайл успешно загружен.\n";
                } else {
                    cerr << "\nОшибка: Не удалось открыть файл.\n";
                }
            }
            system("pause");
        }
        // Сохранение программы в файл
        else if (command.rfind("save ", 0) == 0) {
            string fname = command.substr(5);
            if (fname.empty()) {
                cout << "Укажите имя файла после команды save.\n";
            } else {
                ofstream file(fname);
                if (file.is_open()) {
                    file << source;
                    filename = fname;
                    edited = false;
                    cout << "\nФайл успешно сохранён.\n";
                } else {
                    cerr << "\nОшибка: Не удалось сохранить файл.\n";
                }
            }
            system("pause");
        }
        // Редактирование исходного кода программы
        else if (command == "edit") {
            clearConsole();
            cout << "Текущий исходный код:\n\n" << source << "\n";
            cout << "Введите новый исходный код (введите 'end' на новой строке для завершения):\n";
            source.clear();
            string line;
            while (getline(cin, line) && line != "end") {
                source += line + "\n";
            }
            edited = true;
            cout << "\nИзменения сохранены.\n";
            system("pause");
        }
        // Проверка синтаксиса программы
        else if (command == "check" && !source.empty()) {
            try {
                cout << "\nПроверка синтаксиса...\n";
                Lexer lexer(source);
                auto tokens = lexer.tokenize();
                Parser parser(tokens);
                parser.parse();
                cout << "\nСинтаксических ошибок не обнаружено.\n";
            } catch (const exception& e) {
                cerr << "\nОшибка: " << e.what() << endl;
            }
            system("pause");
        }
        // Запуск программы (интерпретация)
        else if (command == "run" && !source.empty()) {
            try {
                cout << "\nЗапуск программы...\n\n";
                Lexer lexer(source);
                auto tokens = lexer.tokenize();
                Parser parser(tokens);
                auto ast = parser.parse();
                Interpreter interpreter;
                interpreter.run(ast);
                cout << "\n\nПрограмма завершила выполнение.\n";
            } catch (const exception& e) {
                cerr << "\nОшибка выполнения: " << e.what() << endl;
            }
            system("pause");
        }
        // Показать значения переменных (функционал не реализован)
        else if (command == "vars") {
            cout << "\nФункционал отображения переменных еще не реализован.\n";
            system("pause");
        }
        // Выход из IDE с предупреждением о несохранённых изменениях
        else if (command == "exit") {
            if (edited) {
                cout << "\nУ вас есть несохраненные изменения. Вы уверены, что хотите выйти? (y/n): ";
                string answer;
                getline(cin, answer);
                if (answer == "y" || answer == "Y") {
                    running = false;
                }
            } else {
                running = false;
            }
        }
        // Команда help (меню выводится всегда)
        else if (command == "help") {
            // Меню уже выводится в начале каждой итерации
        }
        // Неизвестная команда
        else {
            cout << "\nНеизвестная команда. Введите 'help' для вывода списка команд.\n";
            system("pause");
        }
    }
    
    return 0;
}