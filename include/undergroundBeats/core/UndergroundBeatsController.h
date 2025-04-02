#pragma once

#include "undergroundBeats/audio/AudioSourceSeparator.h"
#include "undergroundBeats/audio/AudioComponentProcessor.h"
#include "undergroundBeats/ml/VariationGenerator.h"
#include <juce_audio_basics/juce_audio_basics.h>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include <functional>

namespace undergroundBeats {
namespace core {

/**
 * @class UndergroundBeatsController
 * @brief Main controller for the Underground Beats application
 * 
 * This class coordinates all the components of the application, including
 * audio processing, ML-based generation, and the UI.
 */
class UndergroundBeatsController {
public:
    /**
     * @brief Constructor
     */
    UndergroundBeatsController();
    
    /**
     * @brief Destructor
     */
    ~UndergroundBeatsController();
    
    /**
     * @brief Initialize the controller
     * @return True if initialization succeeds, false otherwise
     */
    bool initialize();
    
    /**
     * @brief Load an audio file for processing
     * @param filePath Path to the audio file
     * @return True if loading succeeds, false otherwise
     */
    bool loadAudioFile(const std::string& filePath);
    
    /**
     * @brief Save the processed audio to a file
     * @param filePath Path to save the audio file
     * @return True if saving succeeds, false otherwise
     */
    bool saveAudioFile(const std::string& filePath);
    
    /**
     * @brief Generate variations for a component
     * @param componentName The name of the component to generate variations for
     * @param method The variation method to use
     * @param numVariations The number of variations to generate
     * @return Vector of variation IDs
     */
    std::vector<std::string> generateVariations(
        const std::string& componentName,
        ml::VariationMethod method,
        int numVariations);
    
    /**
     * @brief Apply a variation to a component
     * @param componentName The name of the component
     * @param variationId The ID of the variation to apply
     * @return True if the variation was applied successfully
     */
    bool applyVariation(const std::string& componentName, const std::string& variationId);
    
    /**
     * @brief Set the gain for a component
     * @param componentName The name of the component
     * @param gainDB The gain in decibels
     */
    void setComponentGain(const std::string& componentName, float gainDB);
    
    /**
     * @brief Set the pitch shift for a component
     * @param componentName The name of the component
     * @param semitones The pitch shift in semitones
     */
    void setComponentPitchShift(const std::string& componentName, float semitones);
    
    /**
     * @brief Set the tempo adjustment for a component
     * @param componentName The name of the component
     * @param tempoRatio The tempo ratio (1.0 = original tempo)
     */
    void setComponentTempoAdjustment(const std::string& componentName, float tempoRatio);
    
    /**
     * @brief Add an effect to a component
     * @param componentName The name of the component
     * @param effectName The name of the effect to add
     * @return True if the effect was added successfully
     */
    bool addComponentEffect(
        const std::string& componentName,
        const std::string& effectName);
    
    /**
     * @brief Remove an effect from a component
     * @param componentName The name of the component
     * @param effectName The name of the effect to remove
     * @return True if the effect was removed successfully
     */
    bool removeComponentEffect(
        const std::string& componentName,
        const std::string& effectName);
    
    /**
     * @brief Set a parameter for an effect
     * @param componentName The name of the component
     * @param effectName The name of the effect
     * @param paramName The name of the parameter
     * @param value The value to set
     * @return True if the parameter was set successfully
     */
    bool setComponentEffectParameter(
        const std::string& componentName,
        const std::string& effectName,
        const std::string& paramName,
        float value);
    
    /**
     * @brief Enable or disable a component
     * @param componentName The name of the component
     * @param enabled Whether the component should be enabled
     */
    void setComponentEnabled(const std::string& componentName, bool enabled);
    
    /**
     * @brief Get the available component names
     * @return Vector of component names
     */
    std::vector<std::string> getComponentNames() const;
    
    /**
     * @brief Get the available effect names
     * @return Vector of effect names
     */
    std::vector<std::string> getAvailableEffects() const;
    
    /**
     * @brief Get the audio buffer for a component
     * @param componentName The name of the component
     * @return Reference to the component's audio buffer
     */
    const juce::AudioBuffer<float>& getComponentAudioBuffer(
        const std::string& componentName) const;
    
    /**
     * @brief Register a callback for UI updates
     * @param callback The callback function
     */
    void registerUIUpdateCallback(
        std::function<void()> callback);
    
private:
    // Core components
    std::unique_ptr<audio::AudioSourceSeparator> audioSourceSeparator;
    std::unordered_map<std::string, std::unique_ptr<audio::AudioComponentProcessor>> componentProcessors;
    std::unique_ptr<ml::VariationGenerator> variationGenerator;
    
    // Audio state
    juce::AudioBuffer<float> originalAudioBuffer;
    juce::AudioBuffer<float> mixedOutputBuffer;
    
    // Variations
    struct Variation {
        std::string id;
        std::string componentName;
        juce::AudioBuffer<float> audioBuffer;
    };
    
    std::unordered_map<std::string, Variation> variations;
    
    // UI callback
    std::function<void()> uiUpdateCallback;
    
    // Internal methods
    void updateMixedOutput();
    void notifyUIUpdate();
    std::string generateUniqueId();
};

} // namespace core
} // namespace undergroundBeats
