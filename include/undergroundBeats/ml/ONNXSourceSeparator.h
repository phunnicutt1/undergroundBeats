#pragma once

#include "undergroundBeats/ml/AudioSourceSeparator.h"
#include <juce_audio_basics/juce_audio_basics.h>
#include <memory>
#include <string>
#include <vector>

// Forward declarations for ONNX Runtime
namespace Ort {
    class Session;
    class Env;
    class MemoryInfo;
}

namespace undergroundBeats {
namespace ml {

/**
 * @class ONNXSourceSeparator
 * @brief Audio source separator using ONNX Runtime for inference.
 * 
 * This class implements audio source separation using pre-trained models
 * in ONNX format through the ONNX Runtime.
 */
class ONNXSourceSeparator : public AudioSourceSeparator
{
public:
    /**
     * @brief Constructor for the ONNXSourceSeparator class.
     * @param modelPath Path to the ONNX model file.
     */
    ONNXSourceSeparator(const std::string& modelPath);
    
    /**
     * @brief Destructor for the ONNXSourceSeparator class.
     */
    ~ONNXSourceSeparator() override;
    
    /**
     * @brief Initialize the source separator.
     * @return True if initialization was successful, false otherwise.
     */
    bool initialize() override;
    
    /**
     * @brief Check if the separator is initialized and ready to use.
     * @return True if the separator is initialized, false otherwise.
     */
    bool isInitialized() const override;
    
    /**
     * @brief Separate audio into its component parts.
     * @param inputAudio The mixed audio to separate.
     * @param sampleRate The sample rate of the input audio.
     * @return A SeparatedAudio structure containing the separated components.
     */
    SeparatedAudio separateAudio(const juce::AudioBuffer<float>& inputAudio, double sampleRate) override;
    
    /**
     * @brief Get the name of the model.
     * @return The name of the source separation model.
     */
    std::string getModelName() const override;
    
private:
    std::string modelPath;
    std::string modelName;
    bool initialized;
    
    // ONNX Runtime objects
    std::unique_ptr<Ort::Env> ortEnvironment;
    std::unique_ptr<Ort::Session> ortSession;
    std::unique_ptr<Ort::MemoryInfo> ortMemoryInfo;
    
    // Model parameters
    int inputSampleRate;
    int modelSampleRate;
    int chunkSize;
    std::vector<const char*> inputNames;
    std::vector<const char*> outputNames;
    
    // Private methods
    void loadModel();
    void preprocessAudio(const juce::AudioBuffer<float>& inputAudio, std::vector<float>& processedData);
    juce::AudioBuffer<float> postprocessAudio(const std::vector<float>& outputData, int numChannels, int numSamples);
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ONNXSourceSeparator)
};

} // namespace ml
} // namespace undergroundBeats