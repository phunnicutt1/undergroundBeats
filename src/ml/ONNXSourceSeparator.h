#ifndef ONNX_SOURCE_SEPARATOR_H
#define ONNX_SOURCE_SEPARATOR_H

#include <juce_audio_basics/juce_audio_basics.h>
#include "ONNXModelLoader.h"

class ONNXSourceSeparator
{
public:
    ONNXSourceSeparator(const std::string& modelPath);
    ~ONNXSourceSeparator();

    // Main separation method
    std::vector<juce::AudioBuffer<float>> separate(const juce::File& inputFile);

private:
    // Helper methods
    juce::AudioBuffer<float> loadAudio(const juce::File& file);
    Ort::Value preprocess(const juce::AudioBuffer<float>& buffer);
    std::vector<Ort::Value> infer(Ort::Value& inputTensor);
    std::vector<juce::AudioBuffer<float>> postprocess(const std::vector<Ort::Value>& outputTensors);

    ONNXModelLoader modelLoader;
};

#endif // ONNX_SOURCE_SEPARATOR_H