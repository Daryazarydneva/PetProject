// NeuralEngine.h
#pragma once
#include <vector>
#include <string>
#include <onnxruntime_cxx_api.h>

class NeuralEngine {
public:
    explicit NeuralEngine(const std::wstring& modelPath);
    std::vector<float> Infer(const std::vector<float>& input);
private:
    Ort::Env m_env;
    Ort::Session m_session;
    Ort::AllocatorWithDefaultOptions m_allocator;
    std::vector<const char*> m_inputNames;
    std::vector<const char*> m_outputNames;
    size_t m_inputSize;
    size_t m_outputSize;
};