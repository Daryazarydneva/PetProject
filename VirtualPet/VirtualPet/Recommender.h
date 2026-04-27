// Recommender.h
#pragma once
#include "Pet.h"
#include <string>
#include <vector>

/// Класс для генерации рекомендаций по уходу на основе анкеты питомца
class Recommender {
public:
    /// Возвращает рекомендации одной строкой (с переносами)
    std::string GetAdvice(const PetProfile& profile) const;

    /// Возвращает список отдельных рекомендаций (для возможного парсинга)
    std::vector<std::string> GetAdviceList(const PetProfile& profile) const;
};