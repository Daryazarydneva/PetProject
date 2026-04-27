// Notifier.cpp
#include "Notifier.h"
#include <iostream>
#include <chrono>
#include <ctime>
#include <iomanip>

Notifier::Notifier(const std::string& logFile)
    : m_logFile(logFile)
{
    // При создании можно сразу очистить старый лог, но лучше дописывать
}

void Notifier::Send(const std::string& message) {
    // Вывод в консоль
    std::cout << m_consolePrefix << " " << message << std::endl;

    // Запись в лог-файл
    std::ofstream log(m_logFile, std::ios::app);
    if (log.is_open()) {
        // Время
        auto now = std::chrono::system_clock::now();
        std::time_t tt = std::chrono::system_clock::to_time_t(now);
        struct tm timeinfo;
        localtime_s(&timeinfo, &tt); // Windows; для Linux используйте localtime_r
        log << std::put_time(&timeinfo, "%Y-%m-%d %H:%M:%S") << " | " << message << std::endl;
    }
}

void Notifier::SendMany(const std::vector<std::string>& messages) {
    for (const auto& msg : messages) {
        Send(msg);
    }
}