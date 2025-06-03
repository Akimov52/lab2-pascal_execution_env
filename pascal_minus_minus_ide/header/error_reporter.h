#ifndef ERROR_REPORTER_H
#define ERROR_REPORTER_H

#include "interfaces.h"
#include <vector>
#include <string>
#include <iostream>

/**
 * ���������� ���������� IErrorReporter ��� ��������� � ����������� ������ � ��������������
 * ����� �������� �� ����, �������� � ��������������� ����� ��������� � �������� ���������� � ����������
 */
class ErrorReporter : public IErrorReporter {
public:
    /**
     * ��������� ��� �������� ������ � ��������������
     * �������� ������� ��������, ����� ��������� � ������� � �������� ����
     */
    struct Message {
        enum class Level { Error, Warning } level;
        std::string text;
        int line;
        int column;

        Message(Level l, const std::string& t, int ln, int col)
            : level(l), text(t), line(ln), column(col) {}
    };

    /**
     * �������� �� ������
     * @param message ����� ��������� �� ������
     * @param line ����� ������ � �������� ���� (���� ��������)
     * @param column ����� ������� � �������� ���� (���� ��������)
     */
    void reportError(const std::string& message, int line = -1, int column = -1) override {
        messages.emplace_back(Message::Level::Error, message, line, column);
        std::cerr << "\033[1;31m������" << (line >= 0 ? " [������ " + std::to_string(line) + "]" : "") 
                  << (column >= 0 ? " [������� " + std::to_string(column) + "]" : "") 
                  << ": " << message << "\033[0m" << std::endl;
    }

    /**
     * �������� ��������������
     * @param message ����� ��������������
     * @param line ����� ������ � �������� ���� (���� ��������)
     * @param column ����� ������� � �������� ���� (���� ��������)
     */
    void reportWarning(const std::string& message, int line = -1, int column = -1) override {
        messages.emplace_back(Message::Level::Warning, message, line, column);
        std::cerr << "\033[1;33m��������������" << (line >= 0 ? " [������ " + std::to_string(line) + "]" : "") 
                  << (column >= 0 ? " [������� " + std::to_string(column) + "]" : "") 
                  << ": " << message << "\033[0m" << std::endl;
    }

    /**
     * ���������, ���� �� ������
     * @return true, ���� ���� ���������������� ������, ����� false
     */
    bool hasErrors() const override {
        for (const auto& msg : messages) {
            if (msg.level == Message::Level::Error)
                return true;
        }
        return false;
    }

    /**
     * ���������, ���� �� ��������������
     * @return true, ���� ���� ���������������� ��������������, ����� false
     */
    bool hasWarnings() const {
        for (const auto& msg : messages) {
            if (msg.level == Message::Level::Warning)
                return true;
        }
        return false;
    }

    // u041fu043eu043bu0443u0447u0438u0442u044c u0432u0441u0435 u0441u043eu043eu0431u0449u0435u043du0438u044f
    const std::vector<Message>& getMessages() const {
        return messages;
    }

    // u041eu0447u0438u0441u0442u0438u0442u044c u0432u0441u0435 u0441u043eu043eu0431u0449u0435u043du0438u044f
    void clear() {
        messages.clear();
    }

private:
    std::vector<Message> messages;
};

#endif // ERROR_REPORTER_H
