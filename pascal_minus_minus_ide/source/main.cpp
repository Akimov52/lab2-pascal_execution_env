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

// ���������� TokenType �� lexer.h
using PascalToken::TokenType;

using namespace std;

// �������� ����� ������� (Windows)
void clearConsole() {
    system("cls");
}

// ������� ���� ������ ��� ������������
void printMenu() {
    cout << "����� ���������� � Pascal-- IDE!\n";
    cout << "��������� �������:\n";
    cout << "  new         - ������� ����� ���� ���������\n";
    cout << "  open <���>  - ������� ��������� �� �����\n";
    cout << "  save <���>  - ��������� ��������� � ����\n";
    cout << "  edit        - ������� � ����� ��������������\n";
    cout << "  run         - ��������� ���������\n";
    cout << "  check       - ��������� ���������\n";
    cout << "  vars        - �������� �������� ����������\n";
    cout << "  exit        - ����� �� IDE\n";
    cout << "\n������� �������:\n";
}

// ���������� ��������� ������� ��� ����������� ����������� ������� ��������
void setConsoleEncoding() {
    // ������������� ������� �������� ������� � 1251 (Windows-1251)
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);
    setlocale(LC_ALL, "Russian");

    // ������� �����
    system("cls");
}

int main() {
    // ������������� ��������� �������
    setConsoleEncoding();
    
    // ������� �������������� ��������� � ������� ���������� �������
    cout << "Pascal-- IDE ��������. ��������� �������: " << GetConsoleCP() << endl;
    
    // �������� ���������� ��� �������� ������, ��������� ����, ����� ����� � ���������
    string command;
    string source, filename;
    bool running = true;   // ���� ��� ��������� ����� ���������
    bool edited = false;   // ���� ������� ������������ ���������

    // ������� ���� ������ IDE
    while (running) {
        clearConsole();    // ������� ����� ����� ������ ��������
        printMenu();       // ���������� ����
        cin.clear();
        getline(cin, command);

        // �������� ������ ����� ���������
        if (command == "new") {
            clearConsole();
            cout << "������� �������� ��� ��������� (������� 'end' �� ����� ������ ��� ����������):\n";
            source.clear();
            string line;
            while (getline(cin, line) && line != "end") {
                source += line + "\n";
            }
            edited = true;
            cout << "\n��������� ��������� � ������.\n";
            system("pause");
        }
        // �������� ��������� �� �����
        else if (command.find("open ") == 0) {
            string fname = command.substr(5);
            if (fname.empty()) {
                cout << "������� ��� ����� ����� ������� open.\n";
            } else {
                ifstream file(fname);
                if (file.is_open()) {
                    source = string((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
                    file.close();
                    filename = fname;
                    edited = false;
                    cout << "\n���� ������� ��������.\n";
                } else {
                    cerr << "\n������: �� ������� ������� ����.\n";
                }
            }
            system("pause");
        }
        // ���������� ��������� � ����
        else if (command.rfind("save ", 0) == 0) {
            string fname = command.substr(5);
            if (fname.empty()) {
                cout << "������� ��� ����� ����� ������� save.\n";
            } else {
                ofstream file(fname);
                if (file.is_open()) {
                    file << source;
                    filename = fname;
                    edited = false;
                    cout << "\n���� ������� �������.\n";
                } else {
                    cerr << "\n������: �� ������� ��������� ����.\n";
                }
            }
            system("pause");
        }
        // �������������� ��������� ���� ���������
        else if (command == "edit") {
            clearConsole();
            cout << "������� �������� ���:\n\n" << source << "\n";
            cout << "������� ����� �������� ��� (������� 'end' �� ����� ������ ��� ����������):\n";
            source.clear();
            string line;
            while (getline(cin, line) && line != "end") {
                source += line + "\n";
            }
            edited = true;
            cout << "\n��������� ���������.\n";
            system("pause");
        }
        // �������� ���������� ���������
        else if (command == "check" && !source.empty()) {
            try {
                cout << "\n�������� ����������...\n";
                Lexer lexer(source);
                auto tokens = lexer.tokenize();
                Parser parser(tokens);
                parser.parse();
                cout << "\n�������������� ������ �� ����������.\n";
            } catch (const exception& e) {
                cerr << "\n������: " << e.what() << endl;
            }
            system("pause");
        }
        // ������ ��������� (�������������)
        else if (command == "run" && !source.empty()) {
            try {
                cout << "\n������ ���������...\n\n";
                Lexer lexer(source);
                auto tokens = lexer.tokenize();
                Parser parser(tokens);
                auto ast = parser.parse();
                Interpreter interpreter;
                interpreter.run(ast);
                cout << "\n\n��������� ��������� ����������.\n";
            } catch (const exception& e) {
                cerr << "\n������ ����������: " << e.what() << endl;
            }
            system("pause");
        }
        // �������� �������� ���������� (���������� �� ����������)
        else if (command == "vars") {
            cout << "\n���������� ����������� ���������� ��� �� ����������.\n";
            system("pause");
        }
        // ����� �� IDE � ��������������� � ������������ ����������
        else if (command == "exit") {
            if (edited) {
                cout << "\n� ��� ���� ������������� ���������. �� �������, ��� ������ �����? (y/n): ";
                string answer;
                getline(cin, answer);
                if (answer == "y" || answer == "Y") {
                    running = false;
                }
            } else {
                running = false;
            }
        }
        // ������� help (���� ��������� ������)
        else if (command == "help") {
            // ���� ��� ��������� � ������ ������ ��������
        }
        // ����������� �������
        else {
            cout << "\n����������� �������. ������� 'help' ��� ������ ������ ������.\n";
            system("pause");
        }
    }
    
    return 0;
}