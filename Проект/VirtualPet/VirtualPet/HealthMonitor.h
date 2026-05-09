// HealthMonitor.h
#pragma once
#include "Pet.h"
#include <vector>
#include <string>

/// Класс для анализа показаний датчиков и выработки предупреждений
class HealthMonitor {
public:
    /// Проверить отклонения от нормы
    /// @param pet текущий питомец (для норм)
    /// @param temp текущая температура, °C
    /// @param water текущий остаток воды, мл
    /// @param food текущий остаток корма, г
    /// @return список сообщений о проблемах (пустой, если всё в порядке)
    std::vector<std::string> CheckDeviations(const Pet& pet,
        double temp,
        double water,
        double food) const;
};