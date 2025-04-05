#include <catch2/catch.hpp>
#include "undergroundBeats/audio/AudioSourceSeparator.h"
#include <juce_audio_formats/juce_audio_formats.h>

TEST_CASE("AudioSourceSeparator initialization", "[audio][separator]") {
    undergroundBeats::audio::AudioSourceSeparator separator;
    
    SECTION("Default initialization") {
        // Verify component names are set by default
        auto componentNames = separator.getComponentNames();
        REQUIRE(!componentNames.empty());
        
        // Expecting standard components like bass, drums, vocals, etc.
        REQUIRE(std::find(componentNames.begin(), componentNames.end(), "bass") != componentNames.end());
        REQUIRE(std::find(componentNames.begin(), componentNames.end(), "drums") != componentNames.end());
    }
    
    SECTION("Initialize with dummy model") {
        // Without a real model, it should still initialize in test mode
        bool initialized = separator.initialize(undergroundBeats::audio::SeparationModel::DUMMY);
        REQUIRE(initialized);
    }
}

TEST_CASE("AudioSourceSeparator audio processing", "[audio][separator]") {
    undergroundBeats::audio::AudioSourceSeparator separator;
    separator.initialize(undergroundBeats::audio::SeparationModel::DUMMY);
    
    SECTION("Separate audio buffer") {
        // Create a test audio buffer
        const int numChannels = 2;
        const int numSamples = 44100; // 1 second at 44.1kHz
        juce::AudioBuffer<float> testBuffer(numChannels, numSamples);
        
        // Fill with a simple sine wave
        for (int channel = 0; channel < numChannels; ++channel) {
            float* channelData = testBuffer.getWritePointer(channel);
            for (int sample = 0; sample < numSamples; ++sample) {
                channelData[sample] = std::sin(2.0f * juce::MathConstants<float>::pi * 440.0f * sample / 44100.0f);
            }
        }
        
        // Separate the buffer
        auto components = separator.separateAudioBuffer(testBuffer);
        
        // Verify we have all the expected components
        auto componentNames = separator.getComponentNames();
        REQUIRE(components.size() == componentNames.size());
        
        // Verify each component has the right dimensions
        for (const auto& name : componentNames) {
            REQUIRE(components.find(name) != components.end());
            REQUIRE(components[name].getNumChannels() == numChannels);
            REQUIRE(components[name].getNumSamples() == numSamples);
        }
    }
}
