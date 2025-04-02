#pragma once

#include "undergroundBeats/ml/ONNXModelLoader.h"
#include <juce_audio_basics/juce_audio_basics.h>
#include <memory>
#include <string>
#include <vector>

namespace undergroundBeats {
namespace ml {

/**
 * @class StyleTransfer
 * @brief Applies style transfer between audio samples
 * 
 * This class provides functionality to apply the style characteristics of one
 * audio sample (the style reference) to another audio sample (the content).
 */
class StyleTransfer {
public:
    /**
     * @brief Constructor
     */
    StyleTransfer();
    
    /**
     * @brief Destructor
     */
    ~StyleTransfer();
    
    /**
     * @brief Initialize the style transfer with a model
     * @param modelPath Path to the ONNX model file
     * @return True if initialization succeeds, false otherwise
     */
    bool initialize(const std::string& modelPath);
    
    /**
     * @brief Set the style reference audio
     * @param styleBuffer Audio buffer containing the style reference
     */
    void setStyleReference(const juce::AudioBuffer<float>& styleBuffer);
    
    /**
     * @brief Apply style transfer to content audio
     * @param contentBuffer The audio buffer to apply style to
     * @param styleIntensity The intensity of style transfer (0.0-1.0)
     * @return The styled audio buffer
     */
    juce::AudioBuffer<float> applyStyleTransfer(
        const juce::AudioBuffer<float>& contentBuffer,
        float styleIntensity = 0.8f);
    
    /**
     * @brief Check if ready for style transfer
     * @return True if initialized with a model and style reference
     */
    bool isReady() const;
    
private:
    /**
     * @brief Extract style features from an audio buffer
     * @param styleBuffer The audio buffer to extract features from
     * @return The extracted style features
     */
    std::vector<float> extractStyleFeatures(const juce::AudioBuffer<float>& styleBuffer);
    
    /**
     * @brief Extract content features from an audio buffer
     * @param contentBuffer The audio buffer to extract features from
     * @return The extracted content features
     */
    std::vector<float> extractContentFeatures(const juce::AudioBuffer<float>& contentBuffer);
    
    /**
     * @brief Synthesize audio from combined features
     * @param contentFeatures The content features
     * @param styleFeatures The style features
     * @param styleIntensity The intensity of style transfer
     * @param originalContent The original content buffer (for reference)
     * @return The synthesized audio buffer
     */
    juce::AudioBuffer<float> synthesizeAudio(
        const std::vector<float>& contentFeatures,
        const std::vector<float>& styleFeatures,
        float styleIntensity,
        const juce::AudioBuffer<float>& originalContent);
    
    /**
     * @brief Generate a simulated style transfer using DSP
     * @param contentBuffer The original content buffer
     * @param styleBuffer The style reference buffer
     * @param styleIntensity The intensity of style transfer
     * @return The processed audio buffer
     */
    juce::AudioBuffer<float> simulateStyleTransfer(
        const juce::AudioBuffer<float>& contentBuffer,
        const juce::AudioBuffer<float>& styleBuffer,
        float styleIntensity);
    
    // Model and state
    ONNXModelLoader modelLoader;
    bool modelInitialized;
    
    // Style reference
    juce::AudioBuffer<float> styleReferenceBuffer;
    std::vector<float> cachedStyleFeatures;
    bool styleReferenceSet;
};

} // namespace ml
} // namespace undergroundBeats
