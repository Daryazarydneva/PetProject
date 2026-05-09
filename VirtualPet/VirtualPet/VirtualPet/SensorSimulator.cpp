// SensorSimulator.cpp
#include "SensorSimulator.h"
#include <algorithm>
#include <random>
#include <chrono>

// Глобальный генератор случайных чисел (инициализируется один раз)
static std::mt19937& GetRNG() {
    static std::mt19937 gen(static_cast<unsigned>(
        std::chrono::steady_clock::now().time_since_epoch().count()
        ));
    return gen;
}

SensorSimulator::SensorSimulator(double waterCapacityML, double foodCapacityG)
    : m_waterCapacity(waterCapacityML), m_foodCapacity(foodCapacityG)
{
    // На старте поилка и миска полные
    m_waterLevel = m_waterCapacity;
    m_foodLevel = m_foodCapacity;
}

//double SensorSimulator::GetTemperature(const Pet& pet) const {
//    // Получаем диапазон нормы
//    auto [minT, maxT] = pet.GetTemperatureRange();
//
//    // Генерируем температуру вблизи нормы с небольшим шумом, 
//    // иногда – с выбросом за границы (для демонстрации мониторинга)
//    std::uniform_real_distribution<double> baseDist(minT + 0.1, maxT - 0.1);
//    double baseTemp = baseDist(GetRNG());
//
//    // Добавляем случайный шум (гауссовский, stddev 0.2)
//    std::normal_distribution<double> noise(0.0, 0.2);
//    double temp = baseTemp + noise(GetRNG());
//
//    // С вероятностью ~5% имитируем серьёзное отклонение
//    std::uniform_real_distribution<double> outChance(0.0, 1.0);
//    if (outChance(GetRNG()) < 0.05) {
//        // случайное отклонение выше нормы или ниже
//        if (outChance(GetRNG()) < 0.5)
//            temp = maxT + 0.5 + std::abs(noise(GetRNG())) * 1.5;
//        else
//            temp = minT - 0.5 - std::abs(noise(GetRNG())) * 1.5;
//    }
//
//    // Округляем до одного знака после запятой
//    return std::round(temp * 10.0) / 10.0;
//}

void SensorSimulator::Update(double elapsedSeconds) {
    if (elapsedSeconds <= 0.0) return;

    // Расход воды: питомец пьёт с определённой скоростью (мл/сек)
    // Случайная скорость, зависит от активности
    std::uniform_real_distribution<double> waterRateDist(0.5, 1.5); // мл/сек
    double waterRate = waterRateDist(GetRNG());

    // Расход корма
    std::uniform_real_distribution<double> foodRateDist(0.2, 0.6); // г/сек
    double foodRate = foodRateDist(GetRNG());

    // Уменьшаем уровни
    m_waterLevel -= waterRate * elapsedSeconds;
    m_foodLevel -= foodRate * elapsedSeconds;

    // Ограничиваем снизу нулём
    m_waterLevel = std::max(0.0, m_waterLevel);
    m_foodLevel = std::max(0.0, m_foodLevel);

    // Иногда имитируем, что хозяин пополнил запасы (если совсем пусто)
    if (m_waterLevel <= 0.01) {
        m_waterLevel = m_waterCapacity;
        // Небольшой разброс, чтобы не всегда до краёв
        std::uniform_real_distribution<double> refill(0.8, 1.0);
        m_waterLevel *= refill(GetRNG());
    }
    if (m_foodLevel <= 0.01) {
        m_foodLevel = m_foodCapacity;
        std::uniform_real_distribution<double> refill(0.8, 1.0);
        m_foodLevel *= refill(GetRNG());
    }
}