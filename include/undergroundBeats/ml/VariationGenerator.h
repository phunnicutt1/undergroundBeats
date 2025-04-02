#pragma once

#include <juce_audio_basics/juce_audio_basics.h>
#include <memory>
#include <string>
#include <vector>

namespace undergroundBeats {
namespace ml {

/**
 * @enum VariationMethod
 * @brief Methods for generating variations
 */
enum class VariationMethod {
    ALGORITHMIC,  // Simple algorithmic variations
    GAN,          // Generative Adversarial Network
    VAE,          // Variational Autoencoder
    STYLE_TRANSFER // Style transfer from another sample
};

/**
 * @class VariationGenerator
 * @brief Generates variations of audio components
 * 
 * This class provides functionality to generate variations of audio components
 * using algorithmic methods or machine learning.
 */
class VariationGenerator {
public:
    /**
     * @brief Constructor
     */
    VariationGenerator();
    
    /**
     * @brief Destructor
     */
    ~VariationGenerator();
    
    /**
     * @brief Initialize the generator with a model
     * @param method The variation method to use
     * @param modelPath Path to the model file (if applicable)
     * @return True if initialization succeeds, false otherwise
     */
    bool initialize(VariationMethod method, const std::string& modelPath = "");
    
    /**
     * @brief Generate variations of an audio component
     * @param inputBuffer The audio buffer to generate variations from
     * @param numVariations The number of variations to generate
     * @return Vector of generated audio buffers
     */
    std::vector<juce::AudioBuffer<float>> generateVariations(
        const juce::AudioBuffer<float>& inputBuffer,
        int numVariations);
    
    /**
     * @brief Set the seed for random generation
     * @param seed The seed value
     */
    void setSeed(int seed);
    
    /**
     * @brief Set the amount of variation
     * @param amount The variation amount (0.0 to 1.0)
     */
    void setVariationAmount(float amount);
    
    /**
     * @brief Set the style reference for style transfer
     * @param styleBuffer The buffer containing style reference
     */
    void setStyleReference(const juce::AudioBuffer<float>& styleBuffer);
    
private:
    // Variation methods
    std::vector<juce::AudioBuffer<float>> generateAlgorithmicVariations(
        const juce::AudioBuffer<float>& inputBuffer,
        int numVariations);
    
    std::vector<juce::AudioBuffer<float>> generateGANVariations(
        const juce::AudioBuffer<float>& inputBuffer,
        int numVariations);
    
    std::vector<juce::AudioBuffer<float>> generateVAEVariations(
        const juce::AudioBuffer<float>& inputBuffer,
        int numVariations);
    
    std::vector<juce::AudioBuffer<float>> generateStyleTransferVariations(
        const juce::AudioBuffer<float>& inputBuffer,
        int numVariations);
    
    // Internal state
    class Impl;
    std::unique_ptr<Impl> impl; // PIMPL to hide ML dependencies
    
    VariationMethod method;
    float variationAmount;
    int seed;
    juce::AudioBuffer<float> styleReferenceBuffer;
    bool isInitialized;
};

} // namespace ml
} // namespace undergroundBeats
