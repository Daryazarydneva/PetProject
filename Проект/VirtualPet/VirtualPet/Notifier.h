// Notifier.h
#pragma once
#include <string>
#include <fstream>
#include <vector>

/// Класс для отправки уведомлений (консоль + лог-файл)
class Notifier {
public:
    /// Конструктор с именем лог-файла (по умолчанию "pet_alerts.log")
    explicit Notifier(const std::string& logFile = "pet_alerts.log");

    /// Отправить одно уведомление
    void Send(const std::string& message);

    /// Отправить несколько уведомлений
    void SendMany(const std::vector<std::string>& messages);

    /// Установить префикс для консольных сообщений (например, "[ALERT]")
    void SetConsolePrefix(const std::string& prefix) { m_consolePrefix = prefix; }

private:
    std::string m_logFile;
    std::string m_consolePrefix = "[ALERT]";
    bool m_firstWrite = true; // чтобы записать заголовок лога
};