// Recommender.h
#pragma once
#include "Pet.h"
#include <string>
#include <vector>

/// Класс для генерации рекомендаций по уходу на основе анкеты питомца
class NeuralEngine;
class Recommender {
public:
    Recommender() : m_pEngine(nullptr) {}
    void SetNeuralEngine(NeuralEngine* engine) { m_pEngine = engine; }
    /// Возвращает рекомендации одной строкой (с переносами)
    std::string GetAdvice(const PetProfile& profile) const;

    /// Возвращает список отдельных рекомендаций (для возможного парсинга)
    std::vector<std::string> GetAdviceList(const PetProfile& profile) const;
    std::string GetNeuralAdvice(const PetProfile& profile) const;
private:
    NeuralEngine* m_pEngine;
};