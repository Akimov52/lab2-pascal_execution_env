#ifndef LOGGER_H
#define LOGGER_H

/**
 * @file logger.h
 * @brief ������� ����������� ��� ����������� Pascal--
 * 
 * ������������ ������ ��������� � ������ ����������� � ������� � ����
 * � ���������� �������� �������� � ���������������.
 */

#include <string>
#include <fstream>
#include <iostream>
#include <chrono>
#include <iomanip>
#include <memory>

/**
 * ������ �������� ��������� � ������� �����������
 * ������������ ��� ���������� ��������� �� ����������
 */
enum class LogLevel {
    Debug,    // ���������� ����������
    Info,     // �������������� ���������
    Warning,  // ��������������
    Error,    // ������
    Fatal     // ����������� ������
};

/**
 * ����� ��� ������� ����� �����������
 * ���������� ��� �������� (Singleton) ��� �������� ������������� � ������ ������ ���������
 */
class Logger {
public:
    /**
     * �������� ������������ ��������� ������ Logger (Singleton pattern)
     * @return ������ �� ��������� ������ Logger
     */
    static Logger& getInstance() {
        static Logger instance;
        return instance;
    }

    /**
     * ������������� ���� ��� ������ �����
     * @param path ���� � ����� �����
     */
    void setLogFile(const std::string& path) {
        if (logFile.is_open()) {
            logFile.close();
        }
        logFile.open(path, std::ios::app);
    }

    /**
     * ������������� ����������� ������� �������� ��������� ��� �����������
     * @param level ����������� ������� �������� ��� ������ � ���
     */
    void setLogLevel(LogLevel level) {
        currentLevel = level;
    }

    /**
     * �������� ����� ��� ������ ��������� � ���
     * @param level ������� �������� ���������
     * @param message ����� ��������� ��� ������ � ���
     */
    void log(LogLevel level, const std::string& message) {
        if (level < currentLevel) return;

        // �������� ������� �����
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        std::tm tm_buf;
        localtime_s(&tm_buf, &time);

        // ������ ��� ������ ����
        std::string levelStr;
        switch (level) {
        case LogLevel::Debug: levelStr = "DEBUG"; break;
        case LogLevel::Info: levelStr = "INFO"; break;
        case LogLevel::Warning: levelStr = "WARNING"; break;
        case LogLevel::Error: levelStr = "ERROR"; break;
        case LogLevel::Fatal: levelStr = "FATAL"; break;
        }

        // ����������� �����
        std::stringstream logMessage;
        logMessage << std::put_time(&tm_buf, "%Y-%m-%d %H:%M:%S") << " [" 
                  << std::setw(7) << std::left << levelStr << "] " 
                  << message;

        // ����� � ������� � ����
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
 * ������� ��� �������� ������������� ������� � ����
 * ��������� ������������ ������� � �������� ������ ��� ������ ������� �����������
 */
#define LOG_DEBUG(msg) Logger::getInstance().debug(msg)
#define LOG_INFO(msg) Logger::getInstance().info(msg)
#define LOG_WARNING(msg) Logger::getInstance().warning(msg)
#define LOG_ERROR(msg) Logger::getInstance().error(msg)
#define LOG_FATAL(msg) Logger::getInstance().fatal(msg)

#endif // LOGGER_H
