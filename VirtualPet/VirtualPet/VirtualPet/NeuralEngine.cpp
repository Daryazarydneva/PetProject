// NeuralEngine.cpp
#include "NeuralEngine.h"
#include <stdexcept>

NeuralEngine::NeuralEngine(const std::wstring& modelPath)
    : m_env(ORT_LOGGING_LEVEL_WARNING, "PetRecommender"),
    m_session(m_env, modelPath.c_str(), Ort::SessionOptions{ nullptr })
{
    // Получить имена входа
    size_t numInputNodes = m_session.GetInputCount();
    for (size_t i = 0; i < numInputNodes; ++i) {
        Ort::AllocatedStringPtr namePtr = m_session.GetInputNameAllocated(i, m_allocator);
        m_inputNames.push_back(namePtr.release()); // нужно будет освободить, но для простоты оставим
    }
    // Получить имена выхода
    size_t numOutputNodes = m_session.GetOutputCount();
    for (size_t i = 0; i < numOutputNodes; ++i) {
        Ort::AllocatedStringPtr namePtr = m_session.GetOutputNameAllocated(i, m_allocator);
        m_outputNames.push_back(namePtr.release());
    }

    // Узнать размеры (будем считать, что вход [1, m_inputSize])
    Ort::TypeInfo typeInfo = m_session.GetInputTypeInfo(0);
    auto tensorInfo = typeInfo.GetTensorTypeAndShapeInfo();
    auto inputShape = tensorInfo.GetShape(); // например [1, 11]
    if (inputShape.size() != 2 || inputShape[0] != 1)
        throw std::runtime_error("Модель ожидает вход формы [1, N]");
    m_inputSize = inputShape[1]; // N

    // Аналогично для выхода
    typeInfo = m_session.GetOutputTypeInfo(0);
    tensorInfo = typeInfo.GetTensorTypeAndShapeInfo();
    auto outputShape = tensorInfo.GetShape(); // [1, num_classes]
    m_outputSize = outputShape[1];
}

std::vector<float> NeuralEngine::Infer(const std::vector<float>& input) {
    if (input.size() != m_inputSize)
        throw std::runtime_error("Размер входного вектора не совпадает с ожидаемым моделью");

    std::vector<int64_t> inputShape = { 1, static_cast<int64_t>(m_inputSize) };
    Ort::MemoryInfo memInfo = Ort::MemoryInfo::CreateCpu(OrtArenaAllocator, OrtMemTypeDefault);
    Ort::Value inputTensor = Ort::Value::CreateTensor<float>(
        memInfo,
        const_cast<float*>(input.data()), // модель не изменяет данные
        input.size(),
        inputShape.data(),
        inputShape.size());

    auto outputTensors = m_session.Run(Ort::RunOptions{ nullptr },
        m_inputNames.data(), &inputTensor, 1,
        m_outputNames.data(), m_outputNames.size());
    float* outputData = outputTensors.front().GetTensorMutableData<float>();
    size_t outputCount = outputTensors.front().GetTensorTypeAndShapeInfo().GetElementCount();
    return std::vector<float>(outputData, outputData + outputCount);
}