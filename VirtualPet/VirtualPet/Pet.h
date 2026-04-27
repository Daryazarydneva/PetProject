// Pet.h
#pragma once
#include <string>
#include <vector>
#include <map>
#include <utility>

// Структура анкеты питомца
struct PetProfile {
    std::string name;                   // кличка
    std::string breed;                  // порода/вид: dog, cat, rabbit...
    double height_cm = 0.0;            // рост (высота в холке, см)
    double weight_kg = 0.0;            // вес (кг)
    std::vector<std::string> diseases; // имеющиеся заболевания
};

// Класс питомца
class Pet {
public:
    // Конструктор из анкеты
    explicit Pet(const PetProfile& profile);

    // Геттеры характеристик
    const PetProfile& GetProfile() const { return m_profile; }
    double GetHeight() const { return m_profile.height_cm; }
    double GetWeight() const { return m_profile.weight_kg; }
    const std::string& GetBreed() const { return m_profile.breed; }
    const std::vector<std::string>& GetDiseases() const { return m_profile.diseases; }

    // Обновление веса (например, по результатам взвешивания)
    void UpdateWeight(double newWeight);

    // Нормы содержания, зависящие от породы, веса и болезней
    std::pair<double, double> GetTemperatureRange() const; // мин, макс °C
    double GetDailyWaterML() const;                       // мл воды в сутки
    double GetDailyFoodG() const;                         // граммов корма в сутки

private:
    PetProfile m_profile;

    // Вспомогательный метод: загружает табличные нормы для породы
    void LoadBreedDefaults();

    // Внутренние таблицы (заполняются в LoadBreedDefaults)
    struct BreedDefaults {
        std::pair<double, double> tempRange; // {min, max}
        double waterPerKgML = 50.0;          // мл на 1 кг веса
        double foodPerKgG = 25.0;            // г на 1 кг веса
    };
    std::map<std::string, BreedDefaults> m_breedTable;
};