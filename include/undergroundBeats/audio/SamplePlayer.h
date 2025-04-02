#pragma once

#include "undergroundBeats/audio/file/AudioFile.h"
#include <juce_audio_utils/juce_audio_utils.h>
#include <memory>
#include <vector>

namespace undergroundBeats {
namespace audio {

/**
 * @class SamplePlayer
 * @brief Class for playing audio samples in the Underground Beats application.
 * 
 * This class handles playback of audio samples with control over
 * looping, speed, volume, and other parameters.
 */
class SamplePlayer
{
public:
    /**
     * @brief Constructor for the SamplePlayer class.
     * @param sampleRate The sample rate to use for playback.
     * @param blockSize The audio block size (buffer size) for processing.
     */
    SamplePlayer(double sampleRate, int blockSize);
    
    /**
     * @brief Destructor for the SamplePlayer class.
     */
    ~SamplePlayer();
    
    /**
     * @brief Load an audio sample from a file.
     * @param filePath Path to the audio file.
     * @return True if the sample was loaded successfully, false otherwise.
     */
    bool loadSample(const juce::String& filePath);
    
    /**
     * @brief Set the current playback position.
     * @param positionInSeconds Position in seconds.
     */
    void setPosition(double positionInSeconds);
    
    /**
     * @brief Get the current playback position.
     * @return Current position in seconds.
     */
    double getPosition() const;
    
    /**
     * @brief Set the playback speed factor.
     * @param speedFactor Speed factor (1.0 = normal speed, 0.5 = half speed, 2.0 = double speed).
     */
    void setSpeed(double speedFactor);
    
    /**
     * @brief Get the current playback speed factor.
     * @return Current speed factor.
     */
    double getSpeed() const;
    
    /**
     * @brief Set the playback volume.
     * @param volume Volume level (0.0 to 1.0).
     */
    void setVolume(float volume);
    
    /**
     * @brief Get the current playback volume.
     * @return Current volume level.
     */
    float getVolume() const;
    
    /**
     * @brief Set whether the sample should loop during playback.
     * @param shouldLoop True if the sample should loop, false otherwise.
     */
    void setLooping(bool shouldLoop);
    
    /**
     * @brief Check if the sample is set to loop during playback.
     * @return True if looping is enabled, false otherwise.
     */
    bool isLooping() const;
    
    /**
     * @brief Start playback of the loaded sample.
     */
    void play();
    
    /**
     * @brief Pause playback of the sample.
     */
    void pause();
    
    /**
     * @brief Stop playback and reset the position to the beginning.
     */
    void stop();
    
    /**
     * @brief Check if the sample is currently playing.
     * @return True if the sample is playing, false otherwise.
     */
    bool isPlaying() const;
    
    /**
     * @brief Process audio data for a block of samples.
     * @param buffer The audio buffer to fill with output samples.
     * @param numSamples The number of samples to process.
     */
    void processBlock(juce::AudioBuffer<float>& buffer, int numSamples);
    
    /**
     * @brief Prepare for playback with the given sample rate and block size.
     * @param sampleRate The sample rate to use for playback.
     * @param blockSize The audio block size (buffer size) for processing.
     */
    void prepareToPlay(double sampleRate, int blockSize);
    
private:
    std::unique_ptr<file::AudioFile> audioFile;
    double sourceSampleRate;
    double playbackSampleRate;
    int sourceBlockSize;
    
    double playbackSpeed;
    float playbackVolume;
    bool looping;
    bool playing;
    
    int64_t currentPlayPosition;
    double positionIncrement;  // Amount to increment the position each sample (accounts for speed)
    
    juce::LinearSmoothedValue<float> volumeSmoothed;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SamplePlayer)
};

} // namespace audio
} // namespace undergroundBeats