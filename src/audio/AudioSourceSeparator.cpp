#include "undergroundBeats/audio/AudioSourceSeparator.h"
#include <juce_audio_formats/juce_audio_formats.h>
#include <juce_core/juce_core.h>
#include <juce_dsp/juce_dsp.h>
#include <cmath>

namespace undergroundBeats {
namespace audio {

AudioSourceSeparator::AudioSourceSeparator() 
    : currentModelType(SeparationModel::DUMMY),
      sampleRate(44100),
      modelInputSampleRate(44100),
      needsResampling(false),
      windowSize(4096),
      hopSize(1024),
      normalizeInput(true),
      normalizationLevel(0.5f),
      isInitialized(false) {
    
    // Initialize with default component names for dummy model
    componentNames = {"bass", "drums", "vocals", "other"};
}

AudioSourceSeparator::~AudioSourceSeparator() = default;

bool AudioSourceSeparator::initialize(SeparationModel modelType, const std::string& customModelPath) {
    currentModelType = modelType;
    
    // Initialize model-specific parameters
    initializeModelParameters(modelType);
    
    // For dummy model, no need to load an actual model
    if (modelType == SeparationModel::DUMMY) {
        juce::Logger::writeToLog("AudioSourceSeparator: Using dummy separation model");
        isInitialized = true;
        return true;
    }
    
    // Determine model path
    std::string modelPath;
    if (modelType == SeparationModel::CUSTOM) {
        modelPath = customModelPath;
    } else {
        // Get path to the built-in models
        juce::File modelsDir = juce::File::getSpecialLocation(juce::File::currentApplicationFile)
            .getParentDirectory()
            .getChildFile("Resources")
            .getChildFile("models");
        
        // Ensure models directory exists
        if (!modelsDir.exists()) {
            modelsDir.createDirectory();
        }
        
        // Select the appropriate model
        switch (modelType) {
            case SeparationModel::SPLEETER_2STEMS:
                modelPath = modelsDir.getChildFile("spleeter_2stems.onnx").getFullPathName().toStdString();
                break;
                
            case SeparationModel::SPLEETER_4STEMS:
                modelPath = modelsDir.getChildFile("spleeter_4stems.onnx").getFullPathName().toStdString();
                break;
                
            case SeparationModel::DEMUCS:
                modelPath = modelsDir.getChildFile("demucs.onnx").getFullPathName().toStdString();
                break;
                
            default:
                juce::Logger::writeToLog("AudioSourceSeparator: Unknown model type");
                return false;
        }
    }
    
    // Load the model
    juce::Logger::writeToLog("AudioSourceSeparator: Loading model from " + juce::String(modelPath));
    isInitialized = modelLoader.loadModel(modelPath);
    
    if (isInitialized) {
        juce::Logger::writeToLog("AudioSourceSeparator: Model loaded successfully");
    } else {
        juce::Logger::writeToLog("AudioSourceSeparator: Failed to load model, falling back to dummy separation");
        currentModelType = SeparationModel::DUMMY;
        initializeModelParameters(SeparationModel::DUMMY);
        isInitialized = true;
    }
    
    return isInitialized;
}

std::unordered_map<std::string, juce::AudioBuffer<float>> 
AudioSourceSeparator::separateAudioFile(const std::string& audioFile) {
    // Load audio file
    juce::AudioFormatManager formatManager;
    formatManager.registerBasicFormats();
    
    std::unique_ptr<juce::AudioFormatReader> reader(
        formatManager.createReaderFor(juce::File(audioFile)));
    
    if (!reader) {
        juce::Logger::writeToLog("AudioSourceSeparator: Failed to load audio file: " + audioFile);
        return {};
    }
    
    // Set sample rate based on audio file
    sampleRate = static_cast<int>(reader->sampleRate);
    needsResampling = (sampleRate != modelInputSampleRate);
    
    // Read audio data
    juce::AudioBuffer<float> buffer(reader->numChannels, reader->lengthInSamples);
    reader->read(&buffer, 0, reader->lengthInSamples, 0, true, true);
    
    // Separate audio buffer
    return separateAudioBuffer(buffer);
}

std::unordered_map<std::string, juce::AudioBuffer<float>> 
AudioSourceSeparator::separateAudioBuffer(const juce::AudioBuffer<float>& inputBuffer) {
    if (!isInitialized) {
        juce::Logger::writeToLog("AudioSourceSeparator: Not initialized");
        return {};
    }
    
    // For dummy model, just return a simple separation
    if (currentModelType == SeparationModel::DUMMY) {
        return generateDummySeparation(inputBuffer);
    }
    
    // Preprocess audio for the model
    std::vector<float> preprocessedData = preprocessAudio(inputBuffer);
    
    // Run inference with the model
    auto modelOutput = modelLoader.runInference(preprocessedData, inputShape, outputTensorNames);
    
    // Postprocess model output
    return postprocessOutput(modelOutput);
}

std::vector<std::string> AudioSourceSeparator::getComponentNames() const {
    return componentNames;
}

void AudioSourceSeparator::setNormalization(bool enableNormalization, float level) {
    normalizeInput = enableNormalization;
    normalizationLevel = juce::jlimit(0.1f, 1.0f, level);
}

void AudioSourceSeparator::initializeModelParameters(SeparationModel modelType) {
    switch (modelType) {
        case SeparationModel::DUMMY:
            componentNames = {"bass", "drums", "vocals", "other"};
            modelInputSampleRate = 44100;
            windowSize = 4096;
            hopSize = 1024;
            break;
            
        case SeparationModel::SPLEETER_2STEMS:
            componentNames = {"vocals", "accompaniment"};
            modelInputSampleRate = 44100;
            windowSize = 4096;
            hopSize = 1024;
            // Set input shape for Spleeter 2stems (batch, channels, time)
            inputShape = {1, 2, -1}; // -1 will be replaced with actual time dimension
            outputTensorNames = {"vocals", "accompaniment"};
            break;
            
        case SeparationModel::SPLEETER_4STEMS:
            componentNames = {"vocals", "drums", "bass", "other"};
            modelInputSampleRate = 44100;
            windowSize = 4096;
            hopSize = 1024;
            // Set input shape for Spleeter 4stems
            inputShape = {1, 2, -1};
            outputTensorNames = {"vocals", "drums", "bass", "other"};
            break;
            
        case SeparationModel::DEMUCS:
            componentNames = {"vocals", "drums", "bass", "other"};
            modelInputSampleRate = 44100;
            windowSize = 4096;
            hopSize = 1024;
            // Set input shape for Demucs
            inputShape = {1, 2, -1};
            outputTensorNames = {"vocals", "drums", "bass", "other"};
            break;
            
        case SeparationModel::CUSTOM:
            // For custom models, we'll set parameters after loading the model
            componentNames = {}; // Will be populated based on model outputs
            modelInputSampleRate = 44100; // Default, can be overridden
            windowSize = 4096;
            hopSize = 1024;
            break;
    }
}

std::vector<float> AudioSourceSeparator::preprocessAudio(const juce::AudioBuffer<float>& inputBuffer) {
    std::vector<float> result;
    
    // Handle empty input buffer
    if (inputBuffer.getNumSamples() == 0) {
        return result;
    }
    
    // Determine the number of channels to use
    int numChannels = inputBuffer.getNumChannels();
    int modelChannels = 2; // Most models expect stereo input
    
    // Resample if needed
    juce::AudioBuffer<float> resampledBuffer;
    const juce::AudioBuffer<float>* bufferToProcess = &inputBuffer;
    
    if (needsResampling) {
        juce::Logger::writeToLog("AudioSourceSeparator: Resampling from " + 
                               juce::String(sampleRate) + " to " + 
                               juce::String(modelInputSampleRate));
        
        // Create a resampler
        double ratio = static_cast<double>(modelInputSampleRate) / sampleRate;
        int resampledLength = static_cast<int>(inputBuffer.getNumSamples() * ratio);
        
        resampledBuffer.setSize(numChannels, resampledLength);
        
        // Use JUCE's resampler
        juce::LagrangeInterpolator resampler;
        for (int channel = 0; channel < numChannels; ++channel) {
            resampler.reset();
            resampler.process(ratio,
                             inputBuffer.getReadPointer(channel),
                             resampledBuffer.getWritePointer(channel),
                             resampledLength);
        }
        
        bufferToProcess = &resampledBuffer;
    }
    
    // Convert to mono if needed or duplicate to stereo if mono input
    juce::AudioBuffer<float> processBuffer;
    if (numChannels == 1 && modelChannels == 2) {
        // Mono to stereo
        processBuffer.setSize(2, bufferToProcess->getNumSamples());
        processBuffer.copyFrom(0, 0, *bufferToProcess, 0, 0, bufferToProcess->getNumSamples());
        processBuffer.copyFrom(1, 0, *bufferToProcess, 0, 0, bufferToProcess->getNumSamples());
        bufferToProcess = &processBuffer;
    } else if (numChannels > 2) {
        // Multichannel to stereo (simple downmix)
        processBuffer.setSize(2, bufferToProcess->getNumSamples());
        processBuffer.clear();
        
        for (int channel = 0; channel < std::min(numChannels, 2); ++channel) {
            processBuffer.copyFrom(channel, 0, *bufferToProcess, channel, 0, bufferToProcess->getNumSamples());
        }
        
        bufferToProcess = &processBuffer;
    }
    
    // Normalize if enabled
    if (normalizeInput) {
        float maxLevel = 0.0f;
        
        // Find max level
        for (int channel = 0; channel < bufferToProcess->getNumChannels(); ++channel) {
            const float* data = bufferToProcess->getReadPointer(channel);
            for (int sample = 0; sample < bufferToProcess->getNumSamples(); ++sample) {
                maxLevel = std::max(maxLevel, std::abs(data[sample]));
            }
        }
        
        // Apply normalization if needed
        if (maxLevel > 0.0f && std::abs(maxLevel - normalizationLevel) > 0.01f) {
            float gain = normalizationLevel / maxLevel;
            processBuffer.setSize(bufferToProcess->getNumChannels(), bufferToProcess->getNumSamples());
            
            for (int channel = 0; channel < bufferToProcess->getNumChannels(); ++channel) {
                processBuffer.copyFrom(channel, 0, *bufferToProcess, channel, 0, bufferToProcess->getNumSamples());
                processBuffer.applyGain(channel, 0, processBuffer.getNumSamples(), gain);
            }
            
            bufferToProcess = &processBuffer;
        }
    }
    
    // Convert to model input format
    // Most models expect [batch, channels, time] or [batch, time, channels]
    // Adjust based on your specific model requirements
    
    // Update inputShape with actual time dimension
    if (inputShape.size() >= 3 && inputShape[2] == -1) {
        inputShape[2] = bufferToProcess->getNumSamples();
    }
    
    // Allocate result vector
    int totalSize = 1;
    for (auto dim : inputShape) {
        totalSize *= dim > 0 ? dim : bufferToProcess->getNumSamples();
    }
    result.resize(totalSize);
    
    // For [batch, channels, time] format (common in audio models)
    const int batchSize = inputShape[0];
    const int channels = inputShape[1];
    const int timeSteps = inputShape[2];
    
    // Only support batch size of 1 for now
    if (batchSize != 1) {
        juce::Logger::writeToLog("AudioSourceSeparator: Only batch size of 1 is supported");
        return {};
    }
    
    // Check if buffer has enough channels
    if (bufferToProcess->getNumChannels() < channels) {
        juce::Logger::writeToLog("AudioSourceSeparator: Buffer doesn't have enough channels");
        return {};
    }
    
    // Fill result buffer with interleaved audio data
    for (int c = 0; c < channels; ++c) {
        const float* channelData = bufferToProcess->getReadPointer(c);
        for (int t = 0; t < timeSteps; ++t) {
            int index = c * timeSteps + t;
            result[index] = t < bufferToProcess->getNumSamples() ? channelData[t] : 0.0f;
        }
    }
    
    return result;
}

std::unordered_map<std::string, juce::AudioBuffer<float>> 
AudioSourceSeparator::postprocessOutput(const std::unordered_map<std::string, std::vector<float>>& modelOutput) {
    std::unordered_map<std::string, juce::AudioBuffer<float>> result;
    
    // Handle empty model output
    if (modelOutput.empty()) {
        juce::Logger::writeToLog("AudioSourceSeparator: Empty model output");
        return result;
    }
    
    // Parse model output format
    // Most models output with shape [batch, channels, time] or [batch, stems, channels, time]
    // Adjust based on your specific model requirements
    
    // For each component
    for (const auto& componentName : componentNames) {
        // Check if this component exists in the model output
        auto it = modelOutput.find(componentName);
        if (it == modelOutput.end()) {
            juce::Logger::writeToLog("AudioSourceSeparator: Component not found in model output: " + componentName);
            continue;
        }
        
        const auto& outputData = it->second;
        
        // Determine output dimensions
        // This depends on the specific model output format
        int outputChannels = 2; // Assume stereo output for now
        int outputSamples = outputData.size() / outputChannels;
        
        // Create audio buffer for this component
        juce::AudioBuffer<float> componentBuffer(outputChannels, outputSamples);
        
        // Fill the buffer from the model output
        for (int channel = 0; channel < outputChannels; ++channel) {
            float* destData = componentBuffer.getWritePointer(channel);
            
            for (int sample = 0; sample < outputSamples; ++sample) {
                int index = channel * outputSamples + sample;
                
                if (index < outputData.size()) {
                    destData[sample] = outputData[index];
                } else {
                    destData[sample] = 0.0f;
                }
            }
        }
        
        // Resample back to original sample rate if needed
        if (needsResampling) {
            juce::AudioBuffer<float> resampledBuffer(outputChannels, 
                                                  static_cast<int>(outputSamples * static_cast<double>(sampleRate) / modelInputSampleRate));
            
            double ratio = static_cast<double>(sampleRate) / modelInputSampleRate;
            
            // Use JUCE's resampler
            juce::LagrangeInterpolator resampler;
            for (int channel = 0; channel < outputChannels; ++channel) {
                resampler.reset();
                resampler.process(ratio,
                                 componentBuffer.getReadPointer(channel),
                                 resampledBuffer.getWritePointer(channel),
                                 resampledBuffer.getNumSamples());
            }
            
            componentBuffer = std::move(resampledBuffer);
        }
        
        // Store the component buffer
        result[componentName] = std::move(componentBuffer);
    }
    
    return result;
}

std::unordered_map<std::string, juce::AudioBuffer<float>> 
AudioSourceSeparator::generateDummySeparation(const juce::AudioBuffer<float>& inputBuffer) {
    std::unordered_map<std::string, juce::AudioBuffer<float>> result;
    
    for (const auto& name : componentNames) {
        result[name] = juce::AudioBuffer<float>(
            inputBuffer.getNumChannels(), 
            inputBuffer.getNumSamples());
        
        // Create a simple filtered copy for testing
        for (int channel = 0; channel < inputBuffer.getNumChannels(); ++channel) {
            float* destData = result[name].getWritePointer(channel);
            const float* srcData = inputBuffer.getReadPointer(channel);
            
            for (int sample = 0; sample < inputBuffer.getNumSamples(); ++sample) {
                // Different components get different filtering for testing
                if (name == "bass") {
                    // Low-pass filter simulation
                    destData[sample] = srcData[sample] * (sample % 100 < 50 ? 1.0f : 0.3f);
                }
                else if (name == "drums") {
                    // Transient emphasis simulation
                    destData[sample] = srcData[sample] * (sample % 200 < 10 ? 1.0f : 0.1f);
                }
                else if (name == "vocals") {
                    // Mid-range emphasis simulation
                    destData[sample] = srcData[sample] * (sample % 150 < 100 ? 1.0f : 0.2f);
                }
                else {
                    // Everything else
                    destData[sample] = srcData[sample] * 0.4f;
                }
            }
        }
    }
    
    return result;
}

} // namespace audio
} // namespace undergroundBeats
