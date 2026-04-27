// HealthMonitor.cpp
#include "HealthMonitor.h"
#include <sstream>
#include <iomanip>


std::vector<std::string> HealthMonitor::CheckDeviations(
    const Pet& pet,
    double temp,
    double water,
    double food) const
{
    std::vector<std::string> alerts;

    // 1. Температура
    auto [tMin, tMax] = pet.GetTemperatureRange();
    if (temp < tMin || temp > tMax) {
        std::ostringstream msg;
        msg << "Температура " << std::fixed << std::setprecision(1) << temp
            << " °C вне нормы (" << tMin << " - " << tMax << ")!";
        alerts.push_back(msg.str());
    }

    // 2. Вода: порог – 20% от суточной нормы
    double dailyWater = pet.GetDailyWaterML();
    double waterThreshold = 0.2 * dailyWater;
    if (water < waterThreshold) {
        std::ostringstream msg;
        msg << "Вода почти закончилась! Осталось " << water
            << " мл (суточная норма " << dailyWater << " мл).";
        alerts.push_back(msg.str());
    }

    // 3. Корм: порог – 20% от суточной нормы
    double dailyFood = pet.GetDailyFoodG();
    double foodThreshold = 0.2 * dailyFood;
    if (food < foodThreshold) {
        std::ostringstream msg;
        msg << "Корма почти не осталось! Осталось " << food
            << " г (суточная норма " << dailyFood << " г).";
        alerts.push_back(msg.str());
    }

    return alerts;
}