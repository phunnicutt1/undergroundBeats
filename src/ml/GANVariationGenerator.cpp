#include "undergroundBeats/ml/GANVariationGenerator.h"
#include <juce_audio_formats/juce_audio_formats.h>
#include <juce_dsp/juce_dsp.h>
#include <cmath>
#include <algorithm>

namespace undergroundBeats {
namespace ml {

GANVariationGenerator::GANVariationGenerator()
    : latentDimension(128),  // Common latent dimension size for audio GANs
      sampleRate(44100),
      windowSize(4096),
      normalDistribution(0.0f, 1.0f),
      isInitialized(false) {
    
    // Initialize random engine with time-based seed
    std::random_device rd;
    randomEngine.seed(rd());
}

GANVariationGenerator::~GANVariationGenerator() = default;

bool GANVariationGenerator::initialize(const std::string& modelPath) {
    juce::Logger::writeToLog("GANVariationGenerator: Initializing with model: " + modelPath);
    
    // Load the ONNX model
    isInitialized = modelLoader.loadModel(modelPath);
    
    if (isInitialized) {
        juce::Logger::writeToLog("GANVariationGenerator: Model loaded successfully");
        
        // Get model parameters from loaded model
        auto inputNames = modelLoader.getInputNames();
        if (!inputNames.empty()) {
            auto inputShape = modelLoader.getInputShape(inputNames[0]);
            
            // Update latent dimension if specified by the model
            if (inputShape.size() >= 2) {
                latentDimension = static_cast<int>(inputShape[1]);
                juce::Logger::writeToLog("GANVariationGenerator: Latent dimension set to " + 
                                      juce::String(latentDimension));
            }
        }
    }
    else {
        juce::Logger::writeToLog("GANVariationGenerator: Failed to load model");
    }
    
    return isInitialized;
}

std::vector<juce::AudioBuffer<float>> GANVariationGenerator::generateVariations(
    const juce::AudioBuffer<float>& inputBuffer,
    int numVariations,
    float creativityFactor) {
    
    std::vector<juce::AudioBuffer<float>> variations;
    
    if (!isInitialized) {
        juce::Logger::writeToLog("GANVariationGenerator: Not initialized");
        return variations;
    }
    
    if (inputBuffer.getNumSamples() == 0) {
        juce::Logger::writeToLog("GANVariationGenerator: Empty input buffer");
        return variations;
    }
    
    // Preprocess the input audio
    std::vector<float> preprocessedInput = preprocessAudio(inputBuffer);
    
    // Generate latent vectors based on the input
    std::vector<std::vector<float>> latentVectors = generateLatentVectors(
        numVariations, latentDimension, creativityFactor);
    
    // Get model input/output information
    auto inputNames = modelLoader.getInputNames();
    auto outputNames = modelLoader.getOutputNames();
    
    if (inputNames.empty() || outputNames.empty()) {
        juce::Logger::writeToLog("GANVariationGenerator: Model input/output names not available");
        return variations;
    }
    
    // Process each latent vector to generate variations
    for (const auto& latentVector : latentVectors) {
        // Prepare the model inputs
        std::vector<float> combinedInput;
        
        // Combine preprocessed audio with latent vector
        // The exact format depends on the specific GAN model
        combinedInput.insert(combinedInput.end(), preprocessedInput.begin(), preprocessedInput.end());
        combinedInput.insert(combinedInput.end(), latentVector.begin(), latentVector.end());
        
        // Prepare input shape
        std::vector<int64_t> inputShape = {1, static_cast<int64_t>(combinedInput.size())};
        
        // Run the model
        auto modelOutputs = modelLoader.runInference(combinedInput, inputShape, outputNames);
        
        // Check if we have output
        if (modelOutputs.empty() || modelOutputs.find(outputNames[0]) == modelOutputs.end()) {
            juce::Logger::writeToLog("GANVariationGenerator: No output from model");
            continue;
        }
        
        // Get the output data
        auto& outputData = modelOutputs[outputNames[0]];
        
        // Postprocess the output
        auto variationBuffer = postprocessOutput(outputData, inputBuffer);
        
        // Add to variations
        variations.push_back(std::move(variationBuffer));
    }
    
    // If we failed to generate variations with the model, fall back to simulated variations
    if (variations.empty()) {
        juce::Logger::writeToLog("GANVariationGenerator: Falling back to simulated variations");
        
        // Generate simulated variations by applying different processing to the input
        for (int i = 0; i < numVariations; ++i) {
            juce::AudioBuffer<float> variation(inputBuffer.getNumChannels(), inputBuffer.getNumSamples());
            
            // Copy the original audio
            for (int channel = 0; channel < inputBuffer.getNumChannels(); ++channel) {
                variation.copyFrom(channel, 0, inputBuffer, channel, 0, inputBuffer.getNumSamples());
            }
            
            // Apply variation based on index
            float variationAmount = creativityFactor * 0.5f;
            
            switch (i % 4) {
                case 0:
                    // Spectral variation - simulate spectral changes
                    {
                        // Apply a simple filter
                        juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, 
                                                    juce::dsp::IIR::Coefficients<float>> filter;
                        
                        // Random filter parameters
                        float frequency = 200.0f + 5000.0f * (randomEngine() % 100) / 100.0f;
                        float q = 0.5f + variationAmount * (randomEngine() % 100) / 100.0f;
                        float gain = -6.0f + 12.0f * (randomEngine() % 100) / 100.0f;
                        
                        // Set filter type based on variation
                        if (i % 3 == 0) {
                            *filter.state = *juce::dsp::IIR::Coefficients<float>::makeLowShelf(
                                sampleRate, frequency, q, juce::Decibels::decibelsToGain(gain));
                        } else if (i % 3 == 1) {
                            *filter.state = *juce::dsp::IIR::Coefficients<float>::makePeakFilter(
                                sampleRate, frequency, q, juce::Decibels::decibelsToGain(gain));
                        } else {
                            *filter.state = *juce::dsp::IIR::Coefficients<float>::makeHighShelf(
                                sampleRate, frequency, q, juce::Decibels::decibelsToGain(gain));
                        }
                        
                        // Process with the filter
                        juce::dsp::AudioBlock<float> block(variation);
                        juce::dsp::ProcessContextReplacing<float> context(block);
                        filter.process(context);
                    }
                    break;
                    
                case 1:
                    // Temporal variation - simulate timing changes
                    {
                        // Create a temporary buffer
                        juce::AudioBuffer<float> tempBuffer(variation.getNumChannels(), variation.getNumSamples());
                        tempBuffer.clear();
                        
                        // Apply slight time stretching/compression in segments
                        const int numSegments = 8;
                        const int segmentSize = variation.getNumSamples() / numSegments;
                        
                        for (int segment = 0; segment < numSegments; ++segment) {
                            // Randomize time stretch factor per segment
                            float stretchFactor = 1.0f + variationAmount * (randomEngine() % 200 - 100) / 100.0f;
                            stretchFactor = juce::jlimit(0.8f, 1.2f, stretchFactor);
                            
                            const int sourceStart = segment * segmentSize;
                            const int sourceEnd = (segment + 1) * segmentSize;
                            const int sourceLength = sourceEnd - sourceStart;
                            
                            const int destStart = segment * segmentSize;
                            const int destLength = static_cast<int>(sourceLength * stretchFactor);
                            const int destEnd = destStart + destLength;
                            
                            // Ensure we don't go out of bounds
                            if (destEnd > tempBuffer.getNumSamples()) {
                                continue;
                            }
                            
                            // Simple resampling
                            for (int channel = 0; channel < variation.getNumChannels(); ++channel) {
                                const float* sourceData = variation.getReadPointer(channel, sourceStart);
                                float* destData = tempBuffer.getWritePointer(channel, destStart);
                                
                                for (int destSample = 0; destSample < destLength; ++destSample) {
                                    float sourceSample = destSample / stretchFactor;
                                    int sourceSampleIndex = static_cast<int>(sourceSample);
                                    float alpha = sourceSample - sourceSampleIndex;
                                    
                                    if (sourceSampleIndex < sourceLength - 1) {
                                        destData[destSample] = (1.0f - alpha) * sourceData[sourceSampleIndex] +
                                                             alpha * sourceData[sourceSampleIndex + 1];
                                    } else if (sourceSampleIndex < sourceLength) {
                                        destData[destSample] = sourceData[sourceSampleIndex];
                                    }
                                }
                            }
                        }
                        
                        // Copy back to variation buffer
                        for (int channel = 0; channel < variation.getNumChannels(); ++channel) {
                            variation.copyFrom(channel, 0, tempBuffer, channel, 0, variation.getNumSamples());
                        }
                    }
                    break;
                    
                case 2:
                    // Dynamic variation - simulate amplitude changes
                    {
                        // Apply dynamic envelope changes
                        const int numEnvelopePoints = 8;
                        std::vector<float> envelopePoints(numEnvelopePoints);
                        
                        // Generate random envelope points
                        for (int point = 0; point < numEnvelopePoints; ++point) {
                            envelopePoints[point] = 0.7f + variationAmount * (randomEngine() % 200 - 100) / 100.0f;
                            envelopePoints[point] = juce::jlimit(0.4f, 1.6f, envelopePoints[point]);
                        }
                        
                        // Apply envelope to audio
                        const int pointDistance = variation.getNumSamples() / (numEnvelopePoints - 1);
                        
                        for (int channel = 0; channel < variation.getNumChannels(); ++channel) {
                            float* data = variation.getWritePointer(channel);
                            
                            for (int sample = 0; sample < variation.getNumSamples(); ++sample) {
                                int pointIndex = sample / pointDistance;
                                float alpha = static_cast<float>(sample % pointDistance) / pointDistance;
                                
                                if (pointIndex < numEnvelopePoints - 1) {
                                    float gain = (1.0f - alpha) * envelopePoints[pointIndex] +
                                               alpha * envelopePoints[pointIndex + 1];
                                    data[sample] *= gain;
                                } else if (pointIndex < numEnvelopePoints) {
                                    data[sample] *= envelopePoints[pointIndex];
                                }
                            }
                        }
                    }
                    break;
                    
                case 3:
                    // Harmonic variation - simulate harmonic changes
                    {
                        // Add harmonic distortion
                        float drive = variationAmount * 2.0f;
                        drive = juce::jlimit(0.1f, 2.0f, drive);
                        
                        // Apply waveshaping
                        for (int channel = 0; channel < variation.getNumChannels(); ++channel) {
                            float* data = variation.getWritePointer(channel);
                            
                            for (int sample = 0; sample < variation.getNumSamples(); ++sample) {
                                // Simple waveshaping formula
                                data[sample] = std::tanh(data[sample] * drive) / std::tanh(drive);
                            }
                        }
                        
                        // Apply subtle resonant filter
                        juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, 
                                                    juce::dsp::IIR::Coefficients<float>> filter;
                        
                        float frequency = 300.0f + 3000.0f * (randomEngine() % 100) / 100.0f;
                        float q = 1.0f + variationAmount * 4.0f;
                        float gain = 3.0f + variationAmount * 6.0f;
                        
                        *filter.state = *juce::dsp::IIR::Coefficients<float>::makePeakFilter(
                            sampleRate, frequency, q, juce::Decibels::decibelsToGain(gain));
                        
                        juce::dsp::AudioBlock<float> block(variation);
                        juce::dsp::ProcessContextReplacing<float> context(block);
                        filter.process(context);
                    }
                    break;
            }
            
            variations.push_back(std::move(variation));
        }
    }
    
    return variations;
}

void GANVariationGenerator::setSeed(unsigned int seed) {
    randomEngine.seed(seed);
}

juce::AudioBuffer<float> GANVariationGenerator::interpolateVariations(
    const juce::AudioBuffer<float>& variation1,
    const juce::AudioBuffer<float>& variation2,
    float interpolationFactor) {
    
    // Clamp interpolation factor
    interpolationFactor = juce::jlimit(0.0f, 1.0f, interpolationFactor);
    
    // Check buffer compatibility
    if (variation1.getNumChannels() != variation2.getNumChannels() ||
        variation1.getNumSamples() != variation2.getNumSamples()) {
        
        juce::Logger::writeToLog("GANVariationGenerator: Cannot interpolate buffers of different sizes");
        
        // Return a copy of the first variation
        juce::AudioBuffer<float> result(variation1.getNumChannels(), variation1.getNumSamples());
        
        for (int channel = 0; channel < result.getNumChannels(); ++channel) {
            result.copyFrom(channel, 0, variation1, channel, 0, result.getNumSamples());
        }
        
        return result;
    }
    
    // Create result buffer
    juce::AudioBuffer<float> result(variation1.getNumChannels(), variation1.getNumSamples());
    
    // Perform linear interpolation
    for (int channel = 0; channel < result.getNumChannels(); ++channel) {
        const float* data1 = variation1.getReadPointer(channel);
        const float* data2 = variation2.getReadPointer(channel);
        float* resultData = result.getWritePointer(channel);
        
        for (int sample = 0; sample < result.getNumSamples(); ++sample) {
            resultData[sample] = (1.0f - interpolationFactor) * data1[sample] + 
                               interpolationFactor * data2[sample];
        }
    }
    
    return result;
}

std::vector<float> GANVariationGenerator::preprocessAudio(const juce::AudioBuffer<float>& inputBuffer) {
    std::vector<float> result;
    
    // Simple flattening of the audio buffer
    // In a real implementation, you might want to extract features or convert to frequency domain
    
    // Resize result to hold all samples
    result.reserve(inputBuffer.getNumChannels() * inputBuffer.getNumSamples());
    
    // Copy all channels
    for (int channel = 0; channel < inputBuffer.getNumChannels(); ++channel) {
        const float* channelData = inputBuffer.getReadPointer(channel);
        result.insert(result.end(), channelData, channelData + inputBuffer.getNumSamples());
    }
    
    return result;
}

juce::AudioBuffer<float> GANVariationGenerator::postprocessOutput(
    const std::vector<float>& modelOutput,
    const juce::AudioBuffer<float>& originalBuffer) {
    
    // Create result buffer
    juce::AudioBuffer<float> result(originalBuffer.getNumChannels(), originalBuffer.getNumSamples());
    
    // For real model output, this would translate model-specific output format back to audio
    // For now, we'll assume the model outputs a flattened buffer of audio samples
    
    if (modelOutput.size() >= result.getNumChannels() * result.getNumSamples()) {
        // De-interleave the samples back into the audio buffer
        for (int channel = 0; channel < result.getNumChannels(); ++channel) {
            float* destData = result.getWritePointer(channel);
            
            for (int sample = 0; sample < result.getNumSamples(); ++sample) {
                int sourceIndex = channel * result.getNumSamples() + sample;
                destData[sample] = modelOutput[sourceIndex];
            }
        }
    } else {
        // If the model output doesn't match our expected size, just copy the original
        juce::Logger::writeToLog("GANVariationGenerator: Model output size mismatch");
        result.copyFrom(0, 0, originalBuffer, 0, 0, originalBuffer.getNumSamples());
    }
    
    return result;
}

std::vector<std::vector<float>> GANVariationGenerator::generateLatentVectors(
    int numVectors,
    int dimensions,
    float creativity) {
    
    std::vector<std::vector<float>> latentVectors;
    
    // Clamp creativity factor
    creativity = juce::jlimit(0.0f, 1.0f, creativity);
    
    // Scale for normal distribution
    float scale = creativity * 2.0f;
    
    // Generate a base latent vector
    std::vector<float> baseVector(dimensions);
    for (int i = 0; i < dimensions; ++i) {
        baseVector[i] = normalDistribution(randomEngine) * 0.1f;
    }
    
    // Generate variations of the base vector
    for (int v = 0; v < numVectors; ++v) {
        std::vector<float> vector = baseVector;
        
        // Add random variations based on creativity
        for (int i = 0; i < dimensions; ++i) {
            vector[i] += normalDistribution(randomEngine) * scale;
        }
        
        latentVectors.push_back(std::move(vector));
    }
    
    return latentVectors;
}

} // namespace ml
} // namespace undergroundBeats
