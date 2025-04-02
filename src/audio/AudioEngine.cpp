#include "undergroundBeats/audio/AudioEngine.h"
#include "undergroundBeats/audio/file/AudioFile.h"
#include <juce_audio_devices/juce_audio_devices.h>

namespace undergroundBeats {
namespace audio {

AudioEngine::AudioEngine()
    : running(false),
      testToneActive(false),
      testToneFrequency(440.0f), // A4 note
      testToneAmplitude(0.5f),
      sampleRate(0.0),
      bufferSize(0)
{
    // Set up the oscillator with default values
    oscillator.initialise([](float x) { return std::sin(x); }, 128);
    gain.setGainLinear(0.0f);
}

AudioEngine::~AudioEngine()
{
    stop();
    deviceManager.removeAudioCallback(this);
    deviceManager.closeAudioDevice();
}

bool AudioEngine::initialize()
{
    // Initialize the device manager with default audio device settings
    juce::String audioError = deviceManager.initialiseWithDefaultDevices(2, 2);
    
    if (audioError.isNotEmpty())
    {
        juce::Logger::writeToLog("Audio device error: " + audioError);
        return false;
    }
    
    // Register this class as the audio callback
    deviceManager.addAudioCallback(this);
    
    // Create the processor graph for audio routing
    processorGraph = std::make_unique<juce::AudioProcessorGraph>();
    
    return true;
}

void AudioEngine::start()
{
    if (!running)
    {
        running = true;
    }
}

void AudioEngine::stop()
{
    if (running)
    {
        stopTestTone();
        running = false;
    }
}

bool AudioEngine::isRunning() const
{
    return running;
}

void AudioEngine::generateTestTone(float frequency, float amplitude)
{
    testToneFrequency = frequency;
    testToneAmplitude = juce::jlimit(0.0f, 1.0f, amplitude); // Limit amplitude to 0.0-1.0
    
    oscillator.setFrequency(testToneFrequency);
    gain.setGainLinear(testToneAmplitude);
    
    testToneActive = true;
}

void AudioEngine::stopTestTone()
{
    testToneActive = false;
    gain.setGainLinear(0.0f);
}

double AudioEngine::getSampleRate() const
{
    return sampleRate;
}

int AudioEngine::getBufferSize() const
{
    return bufferSize;
}

SamplePlayer* AudioEngine::loadSample(const juce::String& filePath)
{
    // Create a new sample player
    auto newPlayer = std::make_unique<SamplePlayer>(sampleRate, bufferSize);
    
    // Try to load the sample
    if (newPlayer->loadSample(filePath))
    {
        // If successful, add to our list of sample players
        samplePlayers.push_back(std::move(newPlayer));
        return samplePlayers.back().get();
    }
    
    // Loading failed
    return nullptr;
}

SamplePlayer* AudioEngine::getSamplePlayer(size_t index)
{
    if (index < samplePlayers.size())
    {
        return samplePlayers[index].get();
    }
    return nullptr;
}

size_t AudioEngine::getNumSamplePlayers() const
{
    return samplePlayers.size();
}

void AudioEngine::audioDeviceIOCallback(const float** inputChannelData,
                                    int numInputChannels,
                                    float** outputChannelData,
                                    int numOutputChannels,
                                    int numSamples)
{
    // Clear output buffers first (in case we don't generate any audio)
    for (int channel = 0; channel < numOutputChannels; ++channel)
    {
        if (outputChannelData[channel] != nullptr)
        {
            std::memset(outputChannelData[channel], 0, sizeof(float) * numSamples);
        }
    }
    
    if (!running)
    {
        return;
    }
    
    // Create a buffer for processing
    juce::AudioBuffer<float> tempBuffer(numOutputChannels, numSamples);
    tempBuffer.clear();
    
    // Generate test tone if active
    if (testToneActive && numOutputChannels > 0)
    {
        // Create a buffer for audio processing
        juce::dsp::AudioBlock<float> audioBlock(outputChannelData, numOutputChannels, numSamples);
        juce::dsp::ProcessContextReplacing<float> context(audioBlock);
        
        // Generate the tone
        oscillator.process(context);
        gain.process(context);
    }
    
    // Process each sample player
    for (auto& player : samplePlayers)
    {
        if (player && player->isPlaying())
        {
            // Process into temporary buffer
            juce::AudioBuffer<float> playerBuffer(numOutputChannels, numSamples);
            playerBuffer.clear();
            
            player->processBlock(playerBuffer, numSamples);
            
            // Mix into main output buffer
            for (int channel = 0; channel < numOutputChannels; ++channel)
            {
                // Manually add the player buffer to the output buffer
                const float* playerData = playerBuffer.getReadPointer(channel);
                float* outputData = outputChannelData[channel];
                
                for (int i = 0; i < numSamples; ++i)
                {
                    outputData[i] += playerData[i];
                }
            }
        }
    }
}

void AudioEngine::audioDeviceAboutToStart(juce::AudioIODevice* device)
{
    if (device != nullptr)
    {
        sampleRate = device->getCurrentSampleRate();
        bufferSize = device->getCurrentBufferSizeSamples();
        
        // Initialize oscillator with the correct sample rate
        oscillator.prepare({sampleRate, static_cast<juce::uint32>(bufferSize), 2});
        gain.prepare({sampleRate, static_cast<juce::uint32>(bufferSize), 2});
        
        oscillator.setFrequency(testToneFrequency);
        
        // Prepare all sample players
        for (auto& player : samplePlayers)
        {
            if (player)
            {
                player->prepareToPlay(sampleRate, bufferSize);
            }
        }
    }
}

void AudioEngine::audioDeviceStopped()
{
    // Reset state when audio device stops
    sampleRate = 0.0;
    bufferSize = 0;
}

} // namespace audio
} // namespace undergroundBeats