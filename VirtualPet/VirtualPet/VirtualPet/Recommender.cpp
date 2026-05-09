#include "Recommender.h"
#include "NeuralEngine.h"       // <-- ДОБАВИТЬ этот include
#include <algorithm>
#include <sstream>
#include <map>                  // <-- ДОБАВИТЬ для std::multimap


// ============ НОВЫЙ БЛОК: нейросетевые константы и функция кодирования ============

// Категории рекомендаций (соответствуют выходам модели в порядке обучения)
static const std::vector<std::string> g_recCategories = {
    "Обеспечьте питомцу больше свежей воды",
    "Требуется изменение рациона питания",
    "Рекомендуется визит к ветеринару",
    "Увеличьте физическую активность питомца",
    "Следите за температурой воздуха и состоянием питомца"
};

// One-hot породы (порядок должен совпадать с train_model.py)
static const std::vector<std::string> g_allBreeds = { "dog", "cat", "rabbit" };

// Список заболеваний (порядок как в train_model.py)
static const std::vector<std::string> g_allDiseases = {
    "arthritis", "diabetes", "allergy", "kidney_disease", "obesity", "underweight"
};

// Функция кодирования профиля в вектор признаков для нейросети
static std::vector<float> EncodeFeatures(const PetProfile& profile) {
    std::vector<float> feat;
    feat.push_back(static_cast<float>(profile.weight_kg));   // признак 0: вес
    feat.push_back(static_cast<float>(profile.height_cm));   // признак 1: рост

    // One-hot кодирование породы
    for (const auto& b : g_allBreeds) {
        feat.push_back(profile.breed == b ? 1.0f : 0.0f);
    }

    // Multi-hot кодирование заболеваний
    for (const auto& d : g_allDiseases) {
        bool has = std::find(profile.diseases.begin(),
            profile.diseases.end(), d) != profile.diseases.end();
        feat.push_back(has ? 1.0f : 0.0f);
    }

    return feat;
}

// -----------------------------------------------------------
// Вспомогательная функция: проверка наличия заболевания
// -----------------------------------------------------------
static bool HasDisease(const std::vector<std::string>& diseases,
    const std::string& target) {
    return std::find(diseases.begin(), diseases.end(), target) != diseases.end();
}

// -----------------------------------------------------------
// Правила для разных видов/пород
// -----------------------------------------------------------
static void AddBreedAdvice(const PetProfile& p,
    std::vector<std::string>& adv) {
    std::string breed = p.breed;
    double weight = p.weight_kg;
    double height = p.height_cm;

    if (breed == "dog") {
        if (weight > 30.0) {
            adv.push_back("Крупной собаке нужны длительные прогулки (не менее 1.5 часов в день).");
            adv.push_back("Следите за состоянием суставов, обеспечьте мягкую лежанку.");
        }
        else if (weight > 10.0) {
            adv.push_back("Средним собакам достаточно 1–1.5 часов активности в день.");
        }
        else {
            adv.push_back("Мелкие собаки чувствительны к холоду – приобретите тёплую одежду для прогулок.");
            adv.push_back("Регулярно проверяйте зубы и давайте жевательные лакомства для гигиены.");
        }
        adv.push_back("Не забывайте про ежегодную вакцинацию и обработку от паразитов.");
    }
    else if (breed == "cat") {
        adv.push_back("Обеспечьте удобный лоток с чистым наполнителем – кошки очень требовательны к чистоте.");
        adv.push_back("Установите когтеточку, чтобы избежать повреждения мебели.");
        if (weight > 6.0) {
            adv.push_back("Кошка имеет лишний вес – проконсультируйтесь с ветеринаром по диете.");
        }
        adv.push_back("Регулярно вычёсывайте шерсть и давайте пасту от комочков шерсти (особенно для длинношёрстных).");
    }
    else if (breed == "rabbit") {
        adv.push_back("Кролику необходимо неограниченное количество сена – это основа рациона.");
        adv.push_back("Убедитесь, что клетка достаточно просторная и имеет укрытие для отдыха.");
        adv.push_back("Вода всегда должна быть в свободном доступе, лучше в капельной поилке.");
        adv.push_back("Следите за длиной зубов – кроликам нужна твёрдая пища для стачивания.");
    }
    else {
        // Общие советы для неизвестных/экзотических видов
        adv.push_back("Для вашего питомца рекомендуется проконсультироваться со специалистом по данному виду.");
    }
}

// -----------------------------------------------------------
// Правила для заболеваний
// -----------------------------------------------------------
static void AddDiseaseAdvice(const std::vector<std::string>& diseases,
    std::vector<std::string>& adv) {
    if (HasDisease(diseases, "arthritis")) {
        adv.push_back("Артрит: обеспечьте мягкую и тёплую подстилку, избегайте скользких полов.");
        adv.push_back("Рекомендуются добавки с глюкозамином и хондроитином (после консультации с врачом).");
    }
    if (HasDisease(diseases, "diabetes")) {
        adv.push_back("Диабет: строго соблюдайте режим кормления и инъекций инсулина.");
        adv.push_back("Измеряйте уровень глюкозы согласно рекомендациям ветеринара.");
    }
    if (HasDisease(diseases, "allergy")) {
        adv.push_back("Аллергия: перейдите на гипоаллергенный корм, исключите аллергены из окружения.");
        adv.push_back("Регулярно проводите влажную уборку, мойте лежанку.");
    }
    if (HasDisease(diseases, "kidney_disease")) {
        adv.push_back("Заболевание почек: обеспечьте постоянный доступ к свежей воде, используйте лечебный корм.");
        adv.push_back("Контролируйте анализы мочи и крови каждые 3–6 месяцев.");
    }
    if (HasDisease(diseases, "obesity")) {
        adv.push_back("Ожирение: снизьте суточную норму корма на 20%, увеличьте физическую активность.");
    }
    if (HasDisease(diseases, "underweight")) {
        adv.push_back("Недостаток веса: временно увеличьте калорийность рациона, добавьте витаминные комплексы.");
    }
    if (HasDisease(diseases, "chronic_kidney")) {
        adv.push_back("Хроническая почечная недостаточность: переход на ренальный корм, контроль фосфора.");
    }
    // Добавьте другие заболевания по необходимости
}

// -----------------------------------------------------------
// Основной метод – строка
// -----------------------------------------------------------
std::string Recommender::GetAdvice(const PetProfile& profile) const {
    std::vector<std::string> list = GetAdviceList(profile);

    std::ostringstream oss;
    oss << "Рекомендации для " << profile.name << ":\n";
    for (const auto& item : list) {
        oss << " - " << item << "\n";
    }

    // Добавляем нейросетевые рекомендации (если движок есть)
    std::string neuralAdvice = GetNeuralAdvice(profile);
    if (!neuralAdvice.empty()) {
        oss << neuralAdvice;
    }

    return oss.str();
}

// -----------------------------------------------------------
// Основной метод – список
// -----------------------------------------------------------
std::vector<std::string> Recommender::GetAdviceList(const PetProfile& profile) const {
    std::vector<std::string> advice;

    // Универсальные рекомендации
    advice.push_back("Регулярно посещайте ветеринара (не реже 1 раза в год).");
    advice.push_back("Обеспечьте свежую воду и сбалансированное питание.");

    // Рекомендации по породе
    AddBreedAdvice(profile, advice);

    // Рекомендации по заболеваниям
    AddDiseaseAdvice(profile.diseases, advice);

    return advice;

}
std::string Recommender::GetNeuralAdvice(const PetProfile& profile) const {
    if (!m_pEngine) {
        return "";  // нет движка — нет рекомендаций
    }

    // Кодируем профиль
    auto features = EncodeFeatures(profile);

    // Инференс
    std::vector<float> probs;
    try {
        probs = m_pEngine->Infer(features);
    }
    catch (const std::exception& e) {
        std::ostringstream err;
        err << "\n[Нейросеть недоступна: " << e.what() << "]\n";
        return err.str();
    }

    // Сортируем категории по вероятности (берём те, что > 0.5)
    std::multimap<float, size_t, std::greater<float>> sorted;
    for (size_t i = 0; i < probs.size(); ++i) {
        if (probs[i] > 0.5f) {
            sorted.emplace(probs[i], i);
        }
    }

    // Формируем вывод (топ-3)
    std::ostringstream oss;
    oss << "\n--- Нейросетевые рекомендации ---\n";
    int count = 0;
    for (auto& pair : sorted) {
        if (count >= 3) break;
        size_t idx = pair.second;
        float prob = pair.first;
        if (idx < g_recCategories.size()) {
            oss << " - " << g_recCategories[idx]
                << " (вероятность " << static_cast<int>(prob * 100) << "%)\n";
        }
        ++count;
    }
    if (count == 0) {
        oss << " - Особых нейросетевых рекомендаций нет.\n";
    }

    return oss.str();
}