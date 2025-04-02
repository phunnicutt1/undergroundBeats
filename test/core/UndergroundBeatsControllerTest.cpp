#include <catch2/catch.hpp>
#include "undergroundBeats/core/UndergroundBeatsController.h"
#include <juce_audio_formats/juce_audio_formats.h>

TEST_CASE("UndergroundBeatsController initialization", "[core][controller]") {
    undergroundBeats::core::UndergroundBeatsController controller;
    
    SECTION("Basic initialization") {
        // Should initialize without error
        bool initialized = controller.initialize();
        REQUIRE(initialized);
        
        // Should have no components at start
        auto componentNames = controller.getComponentNames();
        REQUIRE(componentNames.empty());
    }
    
    SECTION("Available effects") {
        auto effects = controller.getAvailableEffects();
        REQUIRE(!effects.empty());
        
        // Check for some expected effects
        REQUIRE(std::find(effects.begin(), effects.end(), "reverb") != effects.end());
        REQUIRE(std::find(effects.begin(), effects.end(), "delay") != effects.end());
    }
}

// Helper to create a test audio file
void createTestAudioFile(const juce::File& file, int numChannels = 2, int numSamples = 44100) {
    // Create a buffer with a simple sine wave
    juce::AudioBuffer<float> buffer(numChannels, numSamples);
    
    for (int channel = 0; channel < numChannels; ++channel) {
        float* data = buffer.getWritePointer(channel);
        for (int sample = 0; sample < numSamples; ++sample) {
            data[sample] = 0.5f * std::sin(2.0f * juce::MathConstants<float>::pi * 440.0f * sample / 44100.0f);
        }
    }
    
    // Write to WAV file
    juce::WavAudioFormat wavFormat;
    std::unique_ptr<juce::AudioFormatWriter> writer(
        wavFormat.createWriterFor(
            new juce::FileOutputStream(file),
            44100,
            numChannels,
            16,
            {},
            0
        ));
    
    if (writer) {
        writer->writeFromAudioSampleBuffer(buffer, 0, numSamples);
    }
}

TEST_CASE("UndergroundBeatsController audio loading and processing", "[core][controller]") {
    undergroundBeats::core::UndergroundBeatsController controller;
    controller.initialize();
    
    // Create a temporary test audio file
    juce::File tempFile = juce::File::createTempFile("test_audio.wav");
    createTestAudioFile(tempFile);
    
    SECTION("Load audio file") {
        bool loaded = controller.loadAudioFile(tempFile.getFullPathName().toStdString());
        REQUIRE(loaded);
        
        // After loading, we should have components
        auto componentNames = controller.getComponentNames();
        REQUIRE(!componentNames.empty());
    }
    
    SECTION("Component controls") {
        // Load audio first
        controller.loadAudioFile(tempFile.getFullPathName().toStdString());
        auto componentNames = controller.getComponentNames();
        
        if (!componentNames.empty()) {
            const std::string& firstComponent = componentNames[0];
            
            // Set gain
            controller.setComponentGain(firstComponent, -6.0f);
            
            // Set pitch shift
            controller.setComponentPitchShift(firstComponent, 2.0f);
            
            // Set tempo adjustment
            controller.setComponentTempoAdjustment(firstComponent, 1.2f);
            
            // Add an effect
            bool effectAdded = controller.addComponentEffect(firstComponent, "reverb");
            REQUIRE(effectAdded);
            
            // Set effect parameter
            bool paramSet = controller.setComponentEffectParameter(
                firstComponent, "reverb", "roomSize", 0.8f);
            REQUIRE(paramSet);
            
            // Remove effect
            bool effectRemoved = controller.removeComponentEffect(firstComponent, "reverb");
            REQUIRE(effectRemoved);
            
            // Enable/disable component
            controller.setComponentEnabled(firstComponent, false);
            controller.setComponentEnabled(firstComponent, true);
        }
    }
    
    SECTION("Variation generation") {
        // Load audio first
        controller.loadAudioFile(tempFile.getFullPathName().toStdString());
        auto componentNames = controller.getComponentNames();
        
        if (!componentNames.empty()) {
            const std::string& firstComponent = componentNames[0];
            
            // Generate variations
            auto variationIds = controller.generateVariations(
                firstComponent, undergroundBeats::ml::VariationMethod::ALGORITHMIC, 2);
            
            REQUIRE(variationIds.size() == 2);
            
            // Apply a variation
            bool applied = controller.applyVariation(firstComponent, variationIds[0]);
            REQUIRE(applied);
        }
    }
    
    SECTION("Save audio file") {
        // Load audio first
        controller.loadAudioFile(tempFile.getFullPathName().toStdString());
        
        // Save to a new file
        juce::File outputFile = juce::File::createTempFile("output_audio.wav");
        bool saved = controller.saveAudioFile(outputFile.getFullPathName().toStdString());
        REQUIRE(saved);
        REQUIRE(outputFile.existsAsFile());
        
        // Clean up
        outputFile.deleteFile();
    }
    
    // Clean up
    tempFile.deleteFile();
}

TEST_CASE("UndergroundBeatsController UI callback", "[core][controller]") {
    undergroundBeats::core::UndergroundBeatsController controller;
    controller.initialize();
    
    SECTION("Register callback") {
        bool callbackCalled = false;
        
        controller.registerUIUpdateCallback([&callbackCalled]() {
            callbackCalled = true;
        });
        
        // Create a temporary test audio file and load it (should trigger callback)
        juce::File tempFile = juce::File::createTempFile("test_audio.wav");
        createTestAudioFile(tempFile);
        
        controller.loadAudioFile(tempFile.getFullPathName().toStdString());
        REQUIRE(callbackCalled);
        
        // Clean up
        tempFile.deleteFile();
    }
}
