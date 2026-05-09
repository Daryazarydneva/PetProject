// SensorSimulator.h
#pragma once
#include "Pet.h"

// Класс, эмулирующий показания датчиков
class SensorSimulator {
public:
    // Конструктор, можно задать ёмкость поилки/миски (мл/г)
    SensorSimulator(double waterCapacityML = 500.0, double foodCapacityG = 400.0);

    // Получить текущую температуру питомца (с учётом породных норм)
    //double GetTemperature(const Pet& pet) const;

    // Остаток воды в поилке (мл)
    double GetWaterLevelML() const { return m_waterLevel; }

    // Остаток корма в миске (г)
    double GetFoodLevelG() const { return m_foodLevel; }

    // Вызов при каждом такте симуляции: расход воды/корма, небольшие случайные изменения
    // elapsedSeconds - сколько секунд прошло с предыдущего обновления
    void Update(double elapsedSeconds = 1.0);

private:
    double m_waterCapacity;
    double m_foodCapacity;

    double m_waterLevel;   // текущий остаток
    double m_foodLevel;    // текущий остаток

    // Накопленное время для плавного расхода
    double m_waterAccum = 0.0;
    double m_foodAccum = 0.0;
};