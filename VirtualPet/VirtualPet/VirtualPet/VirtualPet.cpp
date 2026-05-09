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
#include "NeuralEngine.h"
#include <Windows.h>

// Вспомогательная функция: разбор строки с заболеваниями
std::vector<std::string> ParseDiseases(const std::string& input) {
    std::vector<std::string> result;
    if (input.empty()) return result;

    std::string line = input;
    size_t pos = 0;
    while ((pos = line.find(',')) != std::string::npos) {
        std::string disease = line.substr(0, pos);
        disease.erase(0, disease.find_first_not_of(" \t"));
        disease.erase(disease.find_last_not_of(" \t") + 1);
        if (!disease.empty()) {
            result.push_back(disease);
        }
        line.erase(0, pos + 1);
    }
    // Последнее заболевание после последней запятой
    std::string last = line;
    last.erase(0, last.find_first_not_of(" \t"));
    last.erase(last.find_last_not_of(" \t") + 1);
    if (!last.empty()) {
        result.push_back(last);
    }
    return result;
}

int main() {


   
    system("chcp 1251>nul");
    system("cls");
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    setlocale(LC_ALL, "RU");


    std::cout << "===============================================\n";
    std::cout << "       ВИРТУАЛЬНЫЙ ПИТОМЕЦ v1.0\n";
    std::cout << "===============================================\n\n";

    // ==================== ЭТАП 1: АНКЕТА ====================
    PetProfile profile;

    std::cout << ">>> ЗАПОЛНЕНИЕ АНКЕТЫ ПИТОМЦА <<<\n\n";

    std::cout << "Кличка: ";
    std::getline(std::cin, profile.name);

    std::cout << "Вид/порода (dog, cat, rabbit...): ";
    std::getline(std::cin, profile.breed);

    std::cout << "Рост (см): ";
    while (!(std::cin >> profile.height_cm) || profile.height_cm <= 0) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Ошибка! Введите положительное число: ";
    }

    std::cout << "Вес (кг): ";
    while (!(std::cin >> profile.weight_kg) || profile.weight_kg <= 0) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Ошибка! Введите положительное число: ";
    }

    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    std::cout << "Имеющиеся заболевания (через запятую, если нет — Enter): ";
    std::string diseasesLine;
    std::getline(std::cin, diseasesLine);
    profile.diseases = ParseDiseases(diseasesLine);

    // Создаём объект питомца
    Pet myPet(profile);

    // ==================== ЭТАП 2: НОРМЫ ====================
    auto [minT, maxT] = myPet.GetTemperatureRange();
    double dailyWater = myPet.GetDailyWaterML();
    double dailyFood = myPet.GetDailyFoodG();

    std::cout << "\n>>> НОРМЫ ДЛЯ " << profile.name << " <<<\n";
    std::cout << "Температура: " << minT << " – " << maxT << " °C\n";
    std::cout << "Суточная норма воды: " << dailyWater << " мл\n";
    std::cout << "Суточная норма корма: " << dailyFood << " г\n";

    // ==================== ЭТАП 4-5: РЕКОМЕНДАЦИИ ====================
    Recommender recommender;

    std::cout << "\n>>> ЗАГРУЗКА НЕЙРОСЕТИ <<<\n";

    // Проверяем наличие файла модели
    std::ifstream modelFile("pet_advisor.onnx", std::ios::binary);
    if (!modelFile.good()) {
        std::cout << "[ПРЕДУПРЕЖДЕНИЕ] Файл pet_advisor.onnx не найден в папке с программой.\n";
        std::cout << "Нейросетевые рекомендации будут недоступны.\n";
        std::cout << "Поместите pet_advisor.onnx рядом с .exe и перезапустите программу.\n\n";
    }
    else {
        std::cout << "[OK] Файл pet_advisor.onnx обнаружен.\n";
        modelFile.close();
    }

    // Пытаемся загрузить нейросеть
    NeuralEngine* neuralEnginePtr = nullptr;
    try {
        neuralEnginePtr = new NeuralEngine(L"pet_advisor.onnx");
        std::cout << "[OK] Нейросеть загружена успешно.\n";

        // Устанавливаем движок в Recommender
        recommender.SetNeuralEngine(neuralEnginePtr);

        // Диагностика: тестовый прогон (чтобы убедиться, что модель работает)
        size_t expectedInputSize = 2 + 3 + 6; // вес + рост + 3 породы + 6 болезней = 11
        std::vector<float> testInput(expectedInputSize, 0.5f);
        auto testOutput = neuralEnginePtr->Infer(testInput);
        std::cout << "[ДИАГНОСТИКА] Тестовый вывод модели: [";
        for (size_t i = 0; i < testOutput.size(); ++i) {
            std::cout << testOutput[i];
            if (i < testOutput.size() - 1) std::cout << ", ";
        }
        std::cout << "]\n\n";
    }
    catch (const std::exception& e) {
        std::cout << "[ОШИБКА] Не удалось загрузить нейросеть: " << e.what() << "\n";
        std::cout << "Будут использоваться только правила.\n\n";
        delete neuralEnginePtr;
        neuralEnginePtr = nullptr;

        // Явно говорим Recommender'у, что нейросети нет
        recommender.SetNeuralEngine(nullptr);
    }

    // Вывод рекомендаций
    std::cout << ">>> РЕКОМЕНДАЦИИ ПО УХОДУ <<<\n";
    std::cout << recommender.GetAdvice(profile) << std::endl;

    // ==================== ЭТАП 2: ИНИЦИАЛИЗАЦИЯ ДАТЧИКОВ ====================
    std::cout << ">>> ИНИЦИАЛИЗАЦИЯ ДАТЧИКОВ <<<\n";
    std::cout << "Ёмкость поилки: 500 мл\n";
    std::cout << "Ёмкость миски: 400 г\n";
    SensorSimulator sensor(500.0, 400.0);

    // ==================== ЭТАП 3: МОНИТОРИНГ ====================
    HealthMonitor monitor;
    Notifier notifier("pet_alerts.log");
    notifier.SetConsolePrefix("[ТРЕВОГА]");

    std::cout << "\n>>> ЗАПУСК МОНИТОРИНГА <<<\n";
    std::cout << "(10 циклов с интервалом 1.5 сек, для выхода — Ctrl+C)\n\n";

    for (int i = 0; i < 10; ++i) {
        // Обновление симуляции (расход воды и корма)
        sensor.Update(1.5);

        // Считывание показаний
        //double temp = sensor.GetTemperature(myPet);
        double water = sensor.GetWaterLevelML();
        double food = sensor.GetFoodLevelG();

        // Вывод текущего состояния
        std::cout << "[" << i + 1 << "] ";
        //std::cout << "t=" << temp << "°C, ";
        std::cout << "вода=" << water << " мл, ";
        std::cout << "корм=" << food << " г";

        // Проверка отклонений
        std::vector<std::string> alerts = monitor.CheckDeviations(myPet, water, food);

        if (alerts.empty()) {
            std::cout << "  [✓ норма]" << std::endl;
        }
        else {
            std::cout << std::endl;
            notifier.SendMany(alerts);
        }

        // Пауза между измерениями
        std::this_thread::sleep_for(std::chrono::milliseconds(1500));
    }

    std::cout << "\n>>> МОНИТОРИНГ ЗАВЕРШЁН <<<\n";
    std::cout << "Лог-файл с предупреждениями: pet_alerts.log\n";

    // ==================== ЗАВЕРШЕНИЕ ====================
    delete neuralEnginePtr;  // освобождаем память

    std::cout << "\nНажмите Enter для выхода...";
    std::cin.get();

    return 0;
}