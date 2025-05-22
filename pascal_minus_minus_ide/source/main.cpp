#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <windows.h>

#include "parser.h"
#include "interpreter.h"
#include <symbol_table.h>

// �������� ����� ������� (Windows)
void clearConsole() {
    system("cls");
}

// ������� ���� ������ ��� ������������
void printMenu() {
    cout << "����� ���������� � Pascal-- IDE!" << endl;
    cout << "��������� �������:" << endl;
    cout << "  new         - ������� ����� ���� ���������" << endl;
    cout << "  open <���>  - ������� ��������� �� �����" << endl;
    cout << "  save <���>  - ��������� ��������� � ����" << endl;
    cout << "  edit        - ������� � ����� ��������������" << endl;
    cout << "  run         - ��������� ���������" << endl;
    cout << "  check       - ��������� ���������" << endl;
    cout << "  vars        - �������� �������� ����������" << endl;
    cout << "  exit        - ����� �� IDE" << endl;
    cout << endl << "������� �������:" << endl;
}

// ���������� ��������� ������� ��� ����������� ����������� ������� ��������
void setConsoleEncoding() {
    // ������������� ������� �������� ������� � 1251 (Windows-1251)
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);
    setlocale(LC_ALL, "Russian");

    setlocale(LC_NUMERIC, "C");

    // ������� �����
    system("cls");
}

int main() {
    // ������������� ��������� �������
    setConsoleEncoding();

    // �������� ���������� ��� �������� ������, ��������� ����, ����� ����� � ���������
    string source, filename;
    bool running = true;   // ���� ��� ��������� ����� ���������
    bool edited = false;   // ���� ������� ������������ ���������
    // ��������� �� ��������� ���������� ������������� � ����� ��� ������� vars
    Interpreter* lastInterp = nullptr;

    // ������� ���� ������ IDE
    while (running) {
        string command;
        clearConsole();    // ������� ����� ����� ������ ��������
        printMenu();       // ���������� ����
        cin.clear();
        getline(cin, command);

        // �������� ������ ����� ���������
        if (command == "new") {
            clearConsole();
            cout << "������� �������� ��� ��������� (������� 'end' �� ����� ������ ��� ����������):" << endl;
            source.clear();
            string line;
            while (getline(cin, line) && line != "end")
                source += line + "\n";
            edited = true;
            cout << endl << "��������� ��������� � ������." << endl;
            system("pause");
        }
        // �������� ��������� �� �����
        else if (command.find("open ") == 0) {
            string fname = command.substr(5);
            if (fname.empty())
                cout << "������� ��� ����� ����� ������� open." << endl;
            else {
                ifstream file(fname);
                if (file.is_open()) {
                    source = string((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
                    file.close();
                    filename = fname;
                    edited = false;
                    cout << endl << "���� ������� ��������." << endl;
                }
                else
                    cerr << endl << "������: �� ������� ������� ����." << endl;
            }
            system("pause");
        }
        // ���������� ��������� � ����
        else if (command.rfind("save ", 0) == 0) {
            string fname = command.substr(5);
            if (fname.empty())
                cout << "������� ��� ����� ����� ������� save." << endl;
            else {
                ofstream file(fname);
                if (file.is_open()) {
                    file << source;
                    filename = fname;
                    edited = false;
                    cout << endl << "���� ������� �������." << endl;
                }
                else
                    cerr << endl << "������: �� ������� ��������� ����." << endl;
            }
            system("pause");
        }
        // �������������� ��������� ���� ���������
        else if (command == "edit") {
            clearConsole();
            cout << "������� �������� ���:" << endl << endl << source << endl;
            cout << "������� ����� �������� ��� (������� 'end' �� ����� ������ ��� ����������):" << endl;
            source.clear();
            string line;
            while (getline(cin, line) && line != "end")
                source += line + "\n";
            edited = true;
            cout << endl << "��������� ���������." << endl;
            system("pause");
        }
        // �������� ���������� ���������
        else if (command == "check" && !source.empty()) {
            try {
                cout << endl << "�������� ����������..." << endl;
                Lexer lexer(source);
                auto tokens = lexer.tokenize();
                Parser parser(tokens);
                parser.parse();
                cout << endl << "�������������� ������ �� ����������." << endl;
            }
            catch (const exception& e) {
                cerr << endl << "������: " << e.what() << endl;
            }
            system("pause");
        }
        // ������ ��������� (�������������)
        else if (command == "run" && !source.empty()) {
            try {
                cout << endl << "������ ���������..." << endl << endl;
                Lexer lexer(source);
                auto tokens = lexer.tokenize();
                Parser parser(tokens);
                auto ast = parser.parse();
                delete lastInterp;
                lastInterp = new Interpreter();
                lastInterp->run(ast);
                cout << endl << endl << "��������� ��������� ����������." << endl;
            }
            catch (const exception& e) {
                cerr << endl << "������ ����������: " << e.what() << endl;
            }
            system("pause");
        }
        // �������� �������� ���������� (���������� �� ����������)
        else if (command == "vars") {
            cout << endl;
            if (lastInterp) {
                // ��������� SymbolTable �� symbols ��������������
                SymbolTable st;
                auto syms = lastInterp->getAllSymbols(); // map<string, Value>
                for (const auto& [name, val] : syms) {
                    // ���������� ������ ���� � ��������
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
                        valStr = val.strValue;
                        break;
                    }
                    st.addSymbol(SymbolInfo{ name, typeStr, valStr });
                }
                // ������� �������
                st.printTable();
            }
            else
                cout << "��������� ��� �� ���������, ���������� ���." << endl;
            system("pause");
        }
        // ����� �� IDE � ��������������� � ������������ ����������
        else if (command == "exit") {
            if (edited) {
                cout << endl << "� ��� ���� ������������� ���������. �� �������, ��� ������ �����? (y/n): ";
                string answer;
                getline(cin, answer);
                if (answer == "y" || answer == "Y")
                    running = false;
            }
            else
                running = false;
        }
        // ����������� �������
        else {
            cout << endl << "����������� �������." << endl;
            system("pause");
        }
    }
    delete lastInterp;
    return 0;
}