#pragma once

#include <juce_audio_basics/juce_audio_basics.h>
#include <memory>
#include <vector>
#include <string>

namespace undergroundBeats {
namespace ml {

/**
 * @enum AudioComponent
 * @brief Enumeration of audio components that can be separated.
 */
enum class AudioComponent {
    Bass,       ///< Bass instruments and low-frequency content
    Drums,      ///< Percussion and drum sounds
    Vocals,     ///< Human voice and vocals
    Other       ///< Other instruments and sounds
};

/**
 * @struct SeparatedAudio
 * @brief Structure containing separated audio components.
 */
struct SeparatedAudio {
    juce::AudioBuffer<float> bass;    ///< Separated bass component
    juce::AudioBuffer<float> drums;   ///< Separated drums component
    juce::AudioBuffer<float> vocals;  ///< Separated vocals component
    juce::AudioBuffer<float> other;   ///< Other separated components
    double sampleRate;                ///< Sample rate of the audio
    
    /**
     * @brief Get a specific audio component buffer.
     * @param component The component to retrieve.
     * @return Reference to the corresponding audio buffer.
     */
    juce::AudioBuffer<float>& getComponent(AudioComponent component) {
        switch (component) {
            case AudioComponent::Bass:   return bass;
            case AudioComponent::Drums:  return drums;
            case AudioComponent::Vocals: return vocals;
            case AudioComponent::Other:  return other;
            default:                     return other;
        }
    }
    
    /**
     * @brief Get a specific audio component buffer (const version).
     * @param component The component to retrieve.
     * @return Const reference to the corresponding audio buffer.
     */
    const juce::AudioBuffer<float>& getComponent(AudioComponent component) const {
        switch (component) {
            case AudioComponent::Bass:   return bass;
            case AudioComponent::Drums:  return drums;
            case AudioComponent::Vocals: return vocals;
            case AudioComponent::Other:  return other;
            default:                     return other;
        }
    }
};

/**
 * @class AudioSourceSeparator
 * @brief Abstract base class for audio source separation models.
 * 
 * This class defines the interface for audio source separation models
 * that can extract different components (bass, drums, vocals, other)
 * from mixed audio.
 */
class AudioSourceSeparator
{
public:
    /**
     * @brief Virtual destructor.
     */
    virtual ~AudioSourceSeparator() = default;
    
    /**
     * @brief Initialize the source separator.
     * @return True if initialization was successful, false otherwise.
     */
    virtual bool initialize() = 0;
    
    /**
     * @brief Check if the separator is initialized and ready to use.
     * @return True if the separator is initialized, false otherwise.
     */
    virtual bool isInitialized() const = 0;
    
    /**
     * @brief Separate audio into its component parts.
     * @param inputAudio The mixed audio to separate.
     * @param sampleRate The sample rate of the input audio.
     * @return A SeparatedAudio structure containing the separated components.
     */
    virtual SeparatedAudio separateAudio(const juce::AudioBuffer<float>& inputAudio, double sampleRate) = 0;
    
    /**
     * @brief Get the name of the model.
     * @return The name of the source separation model.
     */
    virtual std::string getModelName() const = 0;
    
    /**
     * @brief Create an instance of a source separator by name.
     * @param name The name of the source separator to create.
     * @return A unique pointer to the created source separator, or nullptr if not found.
     */
    static std::unique_ptr<AudioSourceSeparator> createByName(const std::string& name);
    
    /**
     * @brief Get a list of available source separator models.
     * @return A vector of strings containing the names of available models.
     */
    static std::vector<std::string> getAvailableModels();
};

} // namespace ml
} // namespace undergroundBeats