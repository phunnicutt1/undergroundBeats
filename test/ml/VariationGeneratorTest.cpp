#include <catch2/catch.hpp>
#include "undergroundBeats/ml/VariationGenerator.h"

TEST_CASE("VariationGenerator initialization", "[ml][generator]") {
    undergroundBeats::ml::VariationGenerator generator;
    
    SECTION("Initialize with algorithmic method") {
        bool initialized = generator.initialize(undergroundBeats::ml::VariationMethod::ALGORITHMIC);
        REQUIRE(initialized);
    }
    
    SECTION("Initialize with advanced methods (fallback expected)") {
        // These should fall back to algorithmic in the current implementation
        REQUIRE(generator.initialize(undergroundBeats::ml::VariationMethod::GAN));
        REQUIRE(generator.initialize(undergroundBeats::ml::VariationMethod::VAE));
        REQUIRE(generator.initialize(undergroundBeats::ml::VariationMethod::STYLE_TRANSFER));
    }
}

TEST_CASE("VariationGenerator variation creation", "[ml][generator]") {
    undergroundBeats::ml::VariationGenerator generator;
    generator.initialize(undergroundBeats::ml::VariationMethod::ALGORITHMIC);
    
    SECTION("Generate variations with different seeds") {
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
        
        // Generate variations with seed 0
        generator.setSeed(0);
        auto variations1 = generator.generateVariations(testBuffer, 4);
        REQUIRE(variations1.size() == 4);
        
        // Generate variations with a different seed
        generator.setSeed(42);
        auto variations2 = generator.generateVariations(testBuffer, 4);
        REQUIRE(variations2.size() == 4);
        
        // Variations should be different for different seeds
        bool foundDifference = false;
        
        // Check the first variation from each set
        if (variations1[0].getNumSamples() == variations2[0].getNumSamples() && 
            variations1[0].getNumChannels() == variations2[0].getNumChannels()) {
            
            for (int channel = 0; channel < variations1[0].getNumChannels(); ++channel) {
                const float* data1 = variations1[0].getReadPointer(channel);
                const float* data2 = variations2[0].getReadPointer(channel);
                
                // Check a subset of samples
                for (int sample = 0; sample < variations1[0].getNumSamples(); sample += 1000) {
                    if (std::abs(data1[sample] - data2[sample]) > 0.01f) {
                        foundDifference = true;
                        break;
                    }
                }
                
                if (foundDifference) break;
            }
        }
        
        REQUIRE(foundDifference);
    }
    
    SECTION("Variation amount affects output") {
        // Create a test audio buffer
        const int numChannels = 1;
        const int numSamples = 1000;
        juce::AudioBuffer<float> testBuffer(numChannels, numSamples);
        
        // Fill with constant values
        for (int sample = 0; sample < numSamples; ++sample) {
            testBuffer.setSample(0, sample, 1.0f);
        }
        
        // Set a fixed seed for reproducibility
        generator.setSeed(123);
        
        // Generate with minimal variation
        generator.setVariationAmount(0.1f);
        auto minVariations = generator.generateVariations(testBuffer, 1);
        
        // Generate with maximal variation
        generator.setVariationAmount(1.0f);
        auto maxVariations = generator.generateVariations(testBuffer, 1);
        
        // Calculate the average difference from the original for both variations
        float minTotalDiff = 0.0f;
        float maxTotalDiff = 0.0f;
        
        const float* originalData = testBuffer.getReadPointer(0);
        const float* minVarData = minVariations[0].getReadPointer(0);
        const float* maxVarData = maxVariations[0].getReadPointer(0);
        
        for (int sample = 0; sample < numSamples; ++sample) {
            minTotalDiff += std::abs(originalData[sample] - minVarData[sample]);
            maxTotalDiff += std::abs(originalData[sample] - maxVarData[sample]);
        }
        
        // Average differences
        float minAvgDiff = minTotalDiff / numSamples;
        float maxAvgDiff = maxTotalDiff / numSamples;
        
        // The max variation should have a larger average difference
        REQUIRE(maxAvgDiff > minAvgDiff);
    }
}
