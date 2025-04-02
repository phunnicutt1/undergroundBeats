#pragma once

#include "undergroundBeats/ml/ONNXModelLoader.h"
#include <juce_audio_basics/juce_audio_basics.h>
#include <memory>
#include <string>
#include <vector>
#include <random>

namespace undergroundBeats {
namespace ml {

/**
 * @class GANVariationGenerator
 * @brief Generates audio variations using Generative Adversarial Networks
 * 
 * This class provides functionality to generate variations of audio components
 * using pre-trained GAN models.
 */
class GANVariationGenerator {
public:
    /**
     * @brief Constructor
     */
    GANVariationGenerator();
    
    /**
     * @brief Destructor
     */
    ~GANVariationGenerator();
    
    /**
     * @brief Initialize the generator with a model
     * @param modelPath Path to the GAN model file
     * @return True if initialization succeeds, false otherwise
     */
    bool initialize(const std::string& modelPath);
    
    /**
     * @brief Generate variations of an audio component
     * @param inputBuffer The audio buffer to generate variations from
     * @param numVariations The number of variations to generate
     * @param creativityFactor How creative the variations should be (0.0-1.0)
     * @return Vector of generated audio buffers
     */
    std::vector<juce::AudioBuffer<float>> generateVariations(
        const juce::AudioBuffer<float>& inputBuffer,
        int numVariations,
        float creativityFactor = 0.5f);
    
    /**
     * @brief Set the random seed for reproducible variations
     * @param seed The seed value
     */
    void setSeed(unsigned int seed);
    
    /**
     * @brief Interpolate between two variations
     * @param variation1 The first variation buffer
     * @param variation2 The second variation buffer
     * @param interpolationFactor The interpolation factor (0.0-1.0)
     * @return The interpolated audio buffer
     */
    juce::AudioBuffer<float> interpolateVariations(
        const juce::AudioBuffer<float>& variation1,
        const juce::AudioBuffer<float>& variation2,
        float interpolationFactor);
    
private:
    /**
     * @brief Preprocess audio data for the GAN model
     * @param inputBuffer The audio buffer to preprocess
     * @return Preprocessed data ready for model input
     */
    std::vector<float> preprocessAudio(const juce::AudioBuffer<float>& inputBuffer);
    
    /**
     * @brief Postprocess model output into audio buffer
     * @param modelOutput The output from the GAN model
     * @param originalBuffer The original audio buffer for reference
     * @return The processed audio buffer
     */
    juce::AudioBuffer<float> postprocessOutput(
        const std::vector<float>& modelOutput,
        const juce::AudioBuffer<float>& originalBuffer);
    
    /**
     * @brief Generate random latent vectors for the GAN
     * @param numVectors Number of vectors to generate
     * @param dimensions Dimensions of each vector
     * @param creativity How far from the base vector to explore
     * @return Vector of latent vectors
     */
    std::vector<std::vector<float>> generateLatentVectors(
        int numVectors,
        int dimensions,
        float creativity);
    
    // Model and audio parameters
    ONNXModelLoader modelLoader;
    int latentDimension;
    int sampleRate;
    int windowSize;
    
    // Random number generation
    std::mt19937 randomEngine;
    std::normal_distribution<float> normalDistribution;
    
    // Current state
    bool isInitialized;
};

} // namespace ml
} // namespace undergroundBeats
