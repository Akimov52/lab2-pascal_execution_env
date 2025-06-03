#ifndef LOGGER_H
#define LOGGER_H

/**
 * @file logger.h
 * @brief Система логирования для компилятора Pascal--
 * 
 * Обеспечивает запись сообщений о работе компилятора в консоль и файл
 * с различными уровнями важности и форматированием.
 */

#include <string>
#include <fstream>
#include <iostream>
#include <chrono>
#include <iomanip>
#include <memory>

/**
 * Уровни важности сообщений в системе логирования
 * Используются для фильтрации сообщений по значимости
 */
enum class LogLevel {
    Debug,    // Отладочная информация
    Info,     // Информационные сообщения
    Warning,  // Предупреждения
    Error,    // Ошибки
    Fatal     // Критические ошибки
};

/**
 * Класс для ведения логов компилятора
 * Реализован как синглтон (Singleton) для удобства использования в разных частях программы
 */
class Logger {
public:
    /**
     * Получает единственный экземпляр класса Logger (Singleton pattern)
     * @return Ссылка на экземпляр класса Logger
     */
    static Logger& getInstance() {
        static Logger instance;
        return instance;
    }

    /**
     * Устанавливает файл для записи логов
     * @param path Путь к файлу логов
     */
    void setLogFile(const std::string& path) {
        if (logFile.is_open()) {
            logFile.close();
        }
        logFile.open(path, std::ios::app);
    }

    /**
     * Устанавливает минимальный уровень важности сообщений для логирования
     * @param level Минимальный уровень важности для записи в лог
     */
    void setLogLevel(LogLevel level) {
        currentLevel = level;
    }

    /**
     * Основной метод для записи сообщений в лог
     * @param level Уровень важности сообщения
     * @param message Текст сообщения для записи в лог
     */
    void log(LogLevel level, const std::string& message) {
        if (level < currentLevel) return;

        // Получаем текущее время
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        std::tm tm_buf;
        localtime_s(&tm_buf, &time);

        // Строка для уровня лога
        std::string levelStr;
        switch (level) {
        case LogLevel::Debug: levelStr = "DEBUG"; break;
        case LogLevel::Info: levelStr = "INFO"; break;
        case LogLevel::Warning: levelStr = "WARNING"; break;
        case LogLevel::Error: levelStr = "ERROR"; break;
        case LogLevel::Fatal: levelStr = "FATAL"; break;
        }

        // Форматируем вывод
        std::stringstream logMessage;
        logMessage << std::put_time(&tm_buf, "%Y-%m-%d %H:%M:%S") << " [" 
                  << std::setw(7) << std::left << levelStr << "] " 
                  << message;

        // Вывод в консоль и файл
        std::cout << logMessage.str() << std::endl;
        if (logFile.is_open()) {
            logFile << logMessage.str() << std::endl;
        }
    }

    void debug(const std::string& message) { log(LogLevel::Debug, message); }
    void info(const std::string& message) { log(LogLevel::Info, message); }
    void warning(const std::string& message) { log(LogLevel::Warning, message); }
    void error(const std::string& message) { log(LogLevel::Error, message); }
    void fatal(const std::string& message) { log(LogLevel::Fatal, message); }

    ~Logger() {
        if (logFile.is_open()) {
            logFile.close();
        }
    }

private:
    Logger() : currentLevel(LogLevel::Info) {}
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    LogLevel currentLevel;
    std::ofstream logFile;
};

/**
 * Макросы для удобства использования логгера в коде
 * Позволяют использовать краткие и понятные вызовы для разных уровней логирования
 */
#define LOG_DEBUG(msg) Logger::getInstance().debug(msg)
#define LOG_INFO(msg) Logger::getInstance().info(msg)
#define LOG_WARNING(msg) Logger::getInstance().warning(msg)
#define LOG_ERROR(msg) Logger::getInstance().error(msg)
#define LOG_FATAL(msg) Logger::getInstance().fatal(msg)

#endif // LOGGER_H
