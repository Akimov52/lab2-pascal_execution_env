#ifndef ERROR_REPORTER_H
#define ERROR_REPORTER_H

#include "interfaces.h"
#include <vector>
#include <string>
#include <iostream>

/**
 * Реализация интерфейса IErrorReporter для обработки и отображения ошибок и предупреждений
 * Класс отвечает за сбор, хранение и форматированный вывод сообщений в процессе компиляции и выполнения
 */
class ErrorReporter : public IErrorReporter {
public:
    /**
     * Структура для хранения ошибок и предупреждений
     * Содержит уровень важности, текст сообщения и позицию в исходном коде
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
     * Сообщить об ошибке
     * @param message Текст сообщения об ошибке
     * @param line Номер строки в исходном коде (если известно)
     * @param column Номер столбца в исходном коде (если известно)
     */
    void reportError(const std::string& message, int line = -1, int column = -1) override {
        messages.emplace_back(Message::Level::Error, message, line, column);
        std::cerr << "\033[1;31mОшибка" << (line >= 0 ? " [Строка " + std::to_string(line) + "]" : "") 
                  << (column >= 0 ? " [Позиция " + std::to_string(column) + "]" : "") 
                  << ": " << message << "\033[0m" << std::endl;
    }

    /**
     * Сообщить предупреждение
     * @param message Текст предупреждения
     * @param line Номер строки в исходном коде (если известно)
     * @param column Номер столбца в исходном коде (если известно)
     */
    void reportWarning(const std::string& message, int line = -1, int column = -1) override {
        messages.emplace_back(Message::Level::Warning, message, line, column);
        std::cerr << "\033[1;33mПредупреждение" << (line >= 0 ? " [Строка " + std::to_string(line) + "]" : "") 
                  << (column >= 0 ? " [Позиция " + std::to_string(column) + "]" : "") 
                  << ": " << message << "\033[0m" << std::endl;
    }

    /**
     * Проверить, есть ли ошибки
     * @return true, если были зарегистрированы ошибки, иначе false
     */
    bool hasErrors() const override {
        for (const auto& msg : messages) {
            if (msg.level == Message::Level::Error)
                return true;
        }
        return false;
    }

    /**
     * Проверить, есть ли предупреждения
     * @return true, если были зарегистрированы предупреждения, иначе false
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
