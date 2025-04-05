#pragma once

#include <juce_audio_devices/juce_audio_devices.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>
#include <memory>
#include "undergroundBeats/audio/SamplePlayer.h"

namespace undergroundBeats {
namespace audio {

/**
 * @class AudioEngine
 * @brief Core audio engine for the Underground Beats application.
 * 
 * Handles audio input/output, processing, and device management.
 */
class AudioEngine : public juce::AudioIODeviceCallback
{
public:
    /**
     * @brief Constructor for the AudioEngine class.
     * Initializes the audio device manager and sets up the audio callback.
     */
    AudioEngine();
    
    /**
     * @brief Destructor for the AudioEngine class.
     * Properly shuts down audio devices and releases resources.
     */
    ~AudioEngine() override;
    
    /**
     * @brief Initialize the audio engine.
     * @return True if initialization was successful, false otherwise.
     */
    bool initialize();
    
    /**
     * @brief Start audio processing.
     */
    void start();
    
    /**
     * @brief Stop audio processing.
     */
    void stop();
    
    /**
     * @brief Check if the audio engine is currently running.
     * @return True if the engine is running, false otherwise.
     */
    bool isRunning() const;
    
    /**
     * @brief Generate a test tone to verify audio output.
     * @param frequency The frequency of the test tone in Hz.
     * @param amplitude The amplitude of the test tone (0.0 - 1.0).
     */
    void generateTestTone(float frequency, float amplitude);
    
    /**
     * @brief Stop the test tone.
     */
    void stopTestTone();
    
    /**
     * @brief Load a sample from a file.
     * @param filePath Path to the audio file.
     * @return Pointer to the created SamplePlayer, or nullptr if loading failed.
     */
    SamplePlayer* loadSample(const juce::String& filePath);
    
    /**
     * @brief Get a sample player by index.
     * @param index Index of the sample player to retrieve.
     * @return Pointer to the SamplePlayer, or nullptr if index is out of bounds.
     */
    SamplePlayer* getSamplePlayer(size_t index);
    
    /**
     * @brief Get the number of loaded sample players.
     * @return The number of sample players.
     */
    size_t getNumSamplePlayers() const;
    
    /**
     * @brief Get the current sample rate.
     * @return The current sample rate in Hz.
     */
    double getSampleRate() const;
    
    /**
     * @brief Get the current buffer size.
     * @return The current buffer size in samples.
     */
    int getBufferSize() const;

    // AudioIODeviceCallback implementation
    void audioDeviceIOCallback(const float** inputChannelData,
                              int numInputChannels,
                              float** outputChannelData,
                              int numOutputChannels,
                              int numSamples);
   
   void audioDeviceAboutToStart(juce::AudioIODevice* device) override; // Added override
   void audioDeviceStopped() override; // Added override
   
private:
    juce::AudioDeviceManager deviceManager;
    std::unique_ptr<juce::AudioProcessorGraph> processorGraph;
    
    bool running;
    bool testToneActive;
    float testToneFrequency;
    float testToneAmplitude;
    
    std::vector<std::unique_ptr<SamplePlayer>> samplePlayers;
    
    double sampleRate;
    int bufferSize;
    
    juce::dsp::Oscillator<float> oscillator;
    juce::dsp::Gain<float> gain;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioEngine)
};

} // namespace audio
} // namespace undergroundBeats