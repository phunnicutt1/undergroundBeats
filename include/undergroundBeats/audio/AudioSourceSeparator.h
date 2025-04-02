#pragma once

#include "undergroundBeats/ml/ONNXModelLoader.h"
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_dsp/juce_dsp.h>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace undergroundBeats {
namespace audio {

/**
 * @enum SeparationModel
 * @brief Types of source separation models
 */
enum class SeparationModel {
    DUMMY,        // Dummy model for testing
    SPLEETER_2STEMS, // Spleeter 2-stem model (vocals/accompaniment)
    SPLEETER_4STEMS, // Spleeter 4-stem model (vocals/drums/bass/other)
    DEMUCS,       // Demucs model
    CUSTOM        // Custom model
};

/**
 * @class AudioSourceSeparator
 * @brief Separates audio sources using machine learning
 * 
 * This class provides functionality to separate an audio file into its component parts
 * (e.g., bass, drums, vocals, etc.) using machine learning models.
 */
class AudioSourceSeparator {
public:
    /**
     * @brief Constructor
     */
    AudioSourceSeparator();
    
    /**
     * @brief Destructor
     */
    ~AudioSourceSeparator();
    
    /**
     * @brief Initialize the separator with a model
     * @param modelType Type of model to use
     * @param customModelPath Path to a custom model file (if using CUSTOM type)
     * @return True if initialization succeeds, false otherwise
     */
    bool initialize(SeparationModel modelType, const std::string& customModelPath = "");
    
    /**
     * @brief Separate an audio file into components
     * @param audioFile Path to the audio file to separate
     * @return Map of component names to audio buffers
     */
    std::unordered_map<std::string, juce::AudioBuffer<float>> 
    separateAudioFile(const std::string& audioFile);
    
    /**
     * @brief Separate an audio buffer into components
     * @param inputBuffer The audio buffer to separate
     * @return Map of component names to audio buffers
     */
    std::unordered_map<std::string, juce::AudioBuffer<float>> 
    separateAudioBuffer(const juce::AudioBuffer<float>& inputBuffer);
    
    /**
     * @brief Get the available component names
     * @return Vector of component names
     */
    std::vector<std::string> getComponentNames() const;
    
    /**
     * @brief Set the normalization parameters
     * @param enableNormalization Whether to normalize input audio
     * @param normalizationLevel Target normalization level
     */
    void setNormalization(bool enableNormalization, float normalizationLevel = 0.5f);
    
private:
    /**
     * @brief Preprocess audio data for the model
     * @param inputBuffer The audio buffer to preprocess
     * @return Preprocessed data ready for model input
     */
    std::vector<float> preprocessAudio(const juce::AudioBuffer<float>& inputBuffer);
    
    /**
     * @brief Postprocess model output into audio buffers
     * @param modelOutput The output from the ML model
     * @return Map of component names to audio buffers
     */
    std::unordered_map<std::string, juce::AudioBuffer<float>> 
    postprocessOutput(const std::unordered_map<std::string, std::vector<float>>& modelOutput);
    
    /**
     * @brief Initialize model specific parameters
     * @param modelType Type of model to initialize
     */
    void initializeModelParameters(SeparationModel modelType);
    
    /**
     * @brief Generate a dummy separation for testing
     * @param inputBuffer The input audio buffer
     * @return Map of component names to audio buffers
     */
    std::unordered_map<std::string, juce::AudioBuffer<float>> 
    generateDummySeparation(const juce::AudioBuffer<float>& inputBuffer);
    
    // Model state
    ml::ONNXModelLoader modelLoader;
    SeparationModel currentModelType;
    
    // Component names for the current model
    std::vector<std::string> componentNames;
    
    // Input/output configuration
    std::vector<int64_t> inputShape;
    std::vector<std::string> outputTensorNames;
    
    // Audio parameters
    int sampleRate;
    int modelInputSampleRate;
    bool needsResampling;
    
    // Window parameters for processing
    int windowSize;
    int hopSize;
    
    // Normalization parameters
    bool normalizeInput;
    float normalizationLevel;
    
    // Initialization flag
    bool isInitialized;
};

} // namespace audio
} // namespace undergroundBeats
