// Pet.cpp
#include "Pet.h"
#include <stdexcept>
#include <algorithm>
#include <cmath>

// --------------------------------------------------------
// Конструктор: заполняет профиль и инициализирует таблицы норм
// --------------------------------------------------------
Pet::Pet(const PetProfile& profile)
    : m_profile(profile)
{
    if (m_profile.breed.empty())
        throw std::invalid_argument("Порода/вид не может быть пустым.");
    if (m_profile.weight_kg <= 0 || m_profile.height_cm <= 0)
        throw std::invalid_argument("Вес и рост должны быть положительными.");

    LoadBreedDefaults(); // заполняем таблицы для известных пород
}

// --------------------------------------------------------
// Обновление веса
// --------------------------------------------------------
void Pet::UpdateWeight(double newWeight) {
    if (newWeight <= 0)
        throw std::invalid_argument("Вес должен быть положительным.");
    m_profile.weight_kg = newWeight;
}

// --------------------------------------------------------
// Загрузка табличных норм (можно легко расширять)
// --------------------------------------------------------
void Pet::LoadBreedDefaults() {
    // Собака
    m_breedTable["dog"] = {
        {37.5, 39.0},   // температура
        50.0,           // мл воды на кг веса
        20.0            // г корма на кг веса (для средних собак)
    };
    // Кошка
    m_breedTable["cat"] = {
        {38.0, 39.2},
        45.0,
        15.0
    };
    // Кролик
    m_breedTable["rabbit"] = {
        {38.5, 40.0},
        100.0,          // кролики много пьют
        40.0
    };
    // Можно добавить другие породы...
    // Если порода неизвестна – используем усреднённые значения «по умолчанию»
}

// --------------------------------------------------------
// Диапазон допустимой температуры с учётом болезней
// --------------------------------------------------------
std::pair<double, double> Pet::GetTemperatureRange() const {
    BreedDefaults def = { {37.0, 39.0}, 50.0, 25.0 }; // значения по умолчанию
    auto it = m_breedTable.find(m_profile.breed);
    if (it != m_breedTable.end())
        def = it->second;

    double minT = def.tempRange.first;
    double maxT = def.tempRange.second;

    // При некоторых заболеваниях допуск может расширяться
    bool hasChronic = false;
    for (const auto& d : m_profile.diseases) {
        if (d == "arthritis" || d == "diabetes" || d == "chronic_kidney") {
            hasChronic = true;
            break;
        }
    }
    if (hasChronic) {
        minT -= 0.3;
        maxT += 0.3;
    }
    return { minT, maxT };
}

// --------------------------------------------------------
// Суточная норма воды в мл
// --------------------------------------------------------
double Pet::GetDailyWaterML() const {
    BreedDefaults def = { {37.0, 39.0}, 50.0, 25.0 };
    auto it = m_breedTable.find(m_profile.breed);
    if (it != m_breedTable.end())
        def = it->second;

    double water = def.waterPerKgML * m_profile.weight_kg;

    // Корректировка при некоторых болезнях
    for (const auto& d : m_profile.diseases) {
        if (d == "diabetes")      water *= 1.5;     // больше пьют
        if (d == "kidney_disease") water *= 1.3;
        // и т.д.
    }
    return std::ceil(water); // округляем вверх до целых мл
}

// --------------------------------------------------------
// Суточная норма корма в граммах
// --------------------------------------------------------
double Pet::GetDailyFoodG() const {
    BreedDefaults def = { {37.0, 39.0}, 50.0, 25.0 };
    auto it = m_breedTable.find(m_profile.breed);
    if (it != m_breedTable.end())
        def = it->second;

    double food = def.foodPerKgG * m_profile.weight_kg;

    // Изменения при заболеваниях
    for (const auto& d : m_profile.diseases) {
        if (d == "obesity")       food *= 0.8; // снижаем калораж
        if (d == "underweight")   food *= 1.2;
        // аллергия может диктовать спецкорм, но это уже на уровне рекомендаций
    }

    return std::ceil(food * 10) / 10.0; // округление до 0.1 г
}