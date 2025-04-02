#include <catch2/catch.hpp>
#include "undergroundBeats/audio/AudioComponentProcessor.h"

TEST_CASE("AudioComponentProcessor basic operations", "[audio][processor]") {
    const std::string componentName = "test_component";
    undergroundBeats::audio::AudioComponentProcessor processor(componentName);
    
    SECTION("Initialization") {
        // Verify initial state
        REQUIRE(processor.getName() == componentName);
        REQUIRE(processor.isEnabled());
    }
    
    SECTION("Enable/Disable") {
        processor.setEnabled(false);
        REQUIRE_FALSE(processor.isEnabled());
        
        processor.setEnabled(true);
        REQUIRE(processor.isEnabled());
    }
}

TEST_CASE("AudioComponentProcessor audio processing", "[audio][processor]") {
    const std::string componentName = "test_component";
    undergroundBeats::audio::AudioComponentProcessor processor(componentName);
    
    SECTION("Process with gain adjustment") {
        // Create test buffers
        const int numChannels = 2;
        const int numSamples = 1000;
        
        juce::AudioBuffer<float> inputBuffer(numChannels, numSamples);
        juce::AudioBuffer<float> outputBuffer(numChannels, numSamples);
        
        // Fill input with 1.0 values
        for (int channel = 0; channel < numChannels; ++channel) {
            float* data = inputBuffer.getWritePointer(channel);
            for (int sample = 0; sample < numSamples; ++sample) {
                data[sample] = 1.0f;
            }
        }
        
        // Set audio data and process
        processor.setAudioData(inputBuffer);
        
        // Test with unity gain
        processor.setGain(0.0f); // 0dB = unity gain
        outputBuffer.clear();
        processor.processAudio(outputBuffer, numSamples);
        
        // Verify output has the expected level
        for (int channel = 0; channel < numChannels; ++channel) {
            const float* data = outputBuffer.getReadPointer(channel);
            for (int sample = 0; sample < numSamples; ++sample) {
                REQUIRE(data[sample] == Approx(1.0f).margin(0.01f));
            }
        }
        
        // Test with negative gain (-6dB = 0.5 amplitude)
        processor.setGain(-6.0f);
        outputBuffer.clear();
        processor.processAudio(outputBuffer, numSamples);
        
        // Verify output has the expected level
        for (int channel = 0; channel < numChannels; ++channel) {
            const float* data = outputBuffer.getReadPointer(channel);
            for (int sample = 0; sample < numSamples; ++sample) {
                REQUIRE(data[sample] == Approx(0.5f).margin(0.01f));
            }
        }
    }
}

TEST_CASE("AudioComponentProcessor effects", "[audio][processor][effects]") {
    const std::string componentName = "test_component";
    undergroundBeats::audio::AudioComponentProcessor processor(componentName);
    
    SECTION("Add/Remove effects") {
        // Add a reverb effect
        bool added = processor.addEffect("reverb");
        REQUIRE(added);
        
        // Try adding the same effect again (should fail)
        added = processor.addEffect("reverb");
        REQUIRE_FALSE(added);
        
        // Remove the effect
        bool removed = processor.removeEffect("reverb");
        REQUIRE(removed);
        
        // Try removing a non-existent effect
        removed = processor.removeEffect("reverb");
        REQUIRE_FALSE(removed);
    }
    
    SECTION("Effect parameters") {
        // Add a reverb effect
        processor.addEffect("reverb");
        
        // Set parameters
        bool paramSet = processor.setEffectParameter("reverb", "roomSize", 0.75f);
        REQUIRE(paramSet);
        
        // Invalid effect name
        paramSet = processor.setEffectParameter("non_existent", "roomSize", 0.75f);
        REQUIRE_FALSE(paramSet);
    }
}
