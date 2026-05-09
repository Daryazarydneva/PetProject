// main.cpp (демонстрация 1-го этапа)
#include <iostream>
#include <string>
#include <vector>
#include <limits>
#include <thread>
#include "Pet.h"
#include "SensorSimulator.h"
#include "HealthMonitor.h"
#include "Notifier.h"
#include "Recommender.h"
#include <clocale>
#include <cstdlib>
#define NOMINMAX

#include <Windows.h>

int main() {
   
    system("chcp 1251>nul");
    system("cls");
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    setlocale(LC_ALL, "RU");

    std::cout << "=== ВИРТУАЛЬНЫЙ ПИТОМЕЦ: РЕКОМЕНДАЦИИ И МОНИТОРИНГ ===\n";

    // ---------- Этап 1: анкета ----------
    PetProfile profile;
    std::cout << "Кличка: ";
    std::getline(std::cin, profile.name);
    std::cout << "Вид/порода (dog, cat, rabbit...): ";
    std::cin >> profile.breed;
    std::cout << "Рост (см): ";
    std::cin >> profile.height_cm;
    std::cout << "Вес (кг): ";
    std::cin >> profile.weight_kg;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    std::cout << "Имеющиеся заболевания (через запятую, если нет – Enter): ";
    std::string diseasesLine;
    std::getline(std::cin, diseasesLine);

    if (!diseasesLine.empty()) {
        size_t pos = 0;
        std::string line = diseasesLine;
        while ((pos = line.find(',')) != std::string::npos) {
            std::string d = line.substr(0, pos);
            d.erase(0, d.find_first_not_of(" \t"));
            d.erase(d.find_last_not_of(" \t") + 1);
            if (!d.empty()) profile.diseases.push_back(d);
            line.erase(0, pos + 1);
        }
        std::string last = line;
        last.erase(0, last.find_first_not_of(" \t"));
        last.erase(last.find_last_not_of(" \t") + 1);
        if (!last.empty()) profile.diseases.push_back(last);
    }

    try {
        Pet myPet(profile);

        // Нормы
        auto [minT, maxT] = myPet.GetTemperatureRange();
        std::cout << "\n--- Нормы питомца ---\n";
        std::cout << "Температура: " << minT << " - " << maxT << " °C\n";
        std::cout << "Вода в сутки: " << myPet.GetDailyWaterML() << " мл\n";
        std::cout << "Корма в сутки: " << myPet.GetDailyFoodG() << " г\n";

        // ---------- Этап 4: рекомендации (правила) ----------
        Recommender recommender;
        std::cout << "\n" << recommender.GetAdvice(profile) << std::endl;

        // ---------- Этап 2: эмуляция датчиков ----------
        SensorSimulator sensor(500.0, 400.0);

        // ---------- Этап 3: мониторинг и уведомления ----------
        HealthMonitor monitor;
        Notifier notifier("pet_alerts.log");
        notifier.SetConsolePrefix("[PET-ALERT]");

        std::cout << "\n=== Запуск мониторинга (10 циклов с интервалом 1.5 сек) ===\n";
        for (int i = 0; i < 10; ++i) {
            sensor.Update(1.5);

            double temp = sensor.GetTemperature(myPet);
            double water = sensor.GetWaterLevelML();
            double food = sensor.GetFoodLevelG();

            std::cout << "[" << i + 1 << "] "
                << "T=" << temp << " C, "
                << "W=" << water << " мл, "
                << "F=" << food << " г";

            auto alerts = monitor.CheckDeviations(myPet, temp, water, food);
            if (alerts.empty()) {
                std::cout << " [OK]" << std::endl;
            }
            else {
                std::cout << std::endl;
                notifier.SendMany(alerts);
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(1500));
        }

        std::cout << "\nМониторинг завершён. Лог записан в pet_alerts.log\n";
    }
    catch (const std::exception& e) {
        std::cerr << "Ошибка: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
