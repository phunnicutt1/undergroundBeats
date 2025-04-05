#pragma once

#include <vector>
#include <string>
#include <map>
#include <juce_audio_basics/juce_audio_basics.h> // For juce::AudioBuffer

namespace undergroundBeats {
namespace ml {

/**
 * @class AudioSourceSeparator
 * @brief Abstract base class for audio source separation algorithms.
 *
 * Defines the interface for separating an audio signal into its constituent sources (e.g., drums, bass, vocals, other).
 */
class AudioSourceSeparator {
public:
    /**
     * @brief Virtual destructor.
     */
    virtual ~AudioSourceSeparator() = default;

    /**
     * @brief Processes an input audio buffer and separates it into sources.
     * @param inputBuffer The input audio buffer containing the mixed signal.
     * @return A map where keys are source names (e.g., "drums", "bass") and values are juce::AudioBuffer containing the separated audio for that source.
     *         The structure and content depend on the specific implementation.
     */
    virtual std::map<std::string, juce::AudioBuffer<float>> process(const juce::AudioBuffer<float>& inputBuffer) = 0;

    /**
     * @brief Gets the names of the sources that this separator can produce.
     * @return A vector of strings representing the source names (e.g., {"drums", "bass", "vocals", "other"}).
     */
    virtual std::vector<std::string> getSourceNames() const = 0;

    /**
     * @brief Checks if the separator is ready for processing (e.g., model loaded).
     * @return True if ready, false otherwise.
     */
    virtual bool isReady() const = 0;
};

} // namespace ml
} // namespace undergroundBeats