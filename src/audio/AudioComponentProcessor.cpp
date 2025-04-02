#include "undergroundBeats/audio/AudioComponentProcessor.h"
#include <juce_dsp/juce_dsp.h>

namespace undergroundBeats {
namespace audio {

AudioComponentProcessor::AudioComponentProcessor(const std::string& name)
    : name(name),
      enabled(true),
      gain(1.0f),
      pitchShift(0.0f),
      tempoRatio(1.0f) {
}

AudioComponentProcessor::~AudioComponentProcessor() = default;

void AudioComponentProcessor::setAudioData(const juce::AudioBuffer<float>& audioData) {
    this->audioData = audioData;
}

void AudioComponentProcessor::processAudio(juce::AudioBuffer<float>& outputBuffer, int numSamples) {
    if (!enabled || audioData.getNumSamples() == 0) {
        return;
    }
    
    // Create a temporary buffer for processing
    juce::AudioBuffer<float> tempBuffer(
        audioData.getNumChannels(),
        numSamples);
    
    // Copy from audio data, handling bounds correctly
    const int samplesToProcess = std::min(numSamples, audioData.getNumSamples());
    
    for (int channel = 0; channel < tempBuffer.getNumChannels(); ++channel) {
        tempBuffer.copyFrom(
            channel, 0,
            audioData, 
            channel, 0,
            samplesToProcess);
    }
    
    // Apply pitch shift if needed
    if (std::abs(pitchShift) > 0.01f) {
        applyPitchShift(tempBuffer);
    }
    
    // Apply tempo adjustment if needed
    if (std::abs(tempoRatio - 1.0f) > 0.01f) {
        applyTempoAdjustment(tempBuffer);
    }
    
    // Apply effects
    applyEffects(tempBuffer);
    
    // Apply gain
    tempBuffer.applyGain(juce::Decibels::decibelsToGain(gain));
    
    // Add to output buffer
    for (int channel = 0; channel < std::min(tempBuffer.getNumChannels(), outputBuffer.getNumChannels()); ++channel) {
        outputBuffer.addFrom(
            channel, 0,
            tempBuffer,
            channel, 0,
            samplesToProcess);
    }
}

void AudioComponentProcessor::setGain(float gainDB) {
    this->gain = gainDB;
}

void AudioComponentProcessor::setPitchShift(float semitones) {
    this->pitchShift = semitones;
}

void AudioComponentProcessor::setTempoAdjustment(float tempoRatio) {
    this->tempoRatio = tempoRatio;
}

bool AudioComponentProcessor::addEffect(const std::string& effectName) {
    // Check if effect already exists
    if (effects.find(effectName) != effects.end()) {
        return false;
    }
    
    Effect effect;
    
    // Create the appropriate effect processor
    if (effectName == "reverb") {
        auto reverb = std::make_unique<ReverbProcessor>();
        juce::Reverb::Parameters params;
        params.roomSize = 0.5f;
        params.damping = 0.5f;
        params.wetLevel = 0.33f;
        params.dryLevel = 0.7f;
        params.width = 1.0f;
        params.freezeMode = 0.0f;
        reverb->setParameters(params);
        
        effect.processor = std::move(reverb);
        effect.parameters["roomSize"] = params.roomSize;
        effect.parameters["damping"] = params.damping;
        effect.parameters["wetLevel"] = params.wetLevel;
        effect.parameters["dryLevel"] = params.dryLevel;
        effect.parameters["width"] = params.width;
        effect.parameters["freezeMode"] = params.freezeMode;
    }
    else if (effectName == "delay") {
        auto delay = std::make_unique<DelayProcessor>(44100 * 2);  // 2 seconds max delay
        delay->setDelay(44100 * 0.25f);  // 0.25 second delay
        
        effect.processor = std::move(delay);
        effect.parameters["delayTime"] = 0.25f;
        effect.parameters["feedback"] = 0.3f;
        effect.parameters["mix"] = 0.5f;
    }
    else if (effectName == "distortion") {
        // Simple waveshaper distortion
        auto distortion = std::make_unique<WaveShaperProcessor>();
        distortion->setDistortionFactor(2.0f);
        
        effect.processor = std::move(distortion);
        effect.parameters["drive"] = 2.0f;
        effect.parameters["mix"] = 0.5f;
    }
    else if (effectName == "compressor") {
        auto compressor = std::make_unique<CompressorProcessor>();
        compressor->setThreshold(-10.0f);
        compressor->setRatio(4.0f);
        compressor->setAttack(5.0f);
        compressor->setRelease(100.0f);
        
        effect.processor = std::move(compressor);
        effect.parameters["threshold"] = -10.0f;
        effect.parameters["ratio"] = 4.0f;
        effect.parameters["attack"] = 5.0f;
        effect.parameters["release"] = 100.0f;
    }
    else if (effectName == "eq") {
        auto eq = std::make_unique<FilterProcessor>();
        
        // Create a simple low-shelf filter
        eq->setCoefficients(juce::dsp::IIR::Coefficients<float>::makeLowShelf(
            44100, 200.0f, 1.0f, 1.0f));
        
        effect.processor = std::move(eq);
        effect.parameters["frequency"] = 200.0f;
        effect.parameters["q"] = 1.0f;
        effect.parameters["gain"] = 0.0f;
    }
    else {
        // Unknown effect
        return false;
    }
    
    effects[effectName] = std::move(effect);
    return true;
}

bool AudioComponentProcessor::removeEffect(const std::string& effectName) {
    auto it = effects.find(effectName);
    if (it == effects.end()) {
        return false;
    }
    
    effects.erase(it);
    return true;
}

bool AudioComponentProcessor::setEffectParameter(
    const std::string& effectName,
    const std::string& paramName,
    float value) {
    auto it = effects.find(effectName);
    if (it == effects.end()) {
        return false;
    }
    
    auto& effect = it->second;
    
    // Update parameter value
    effect.parameters[paramName] = value;
    
    // Apply parameter to the effect processor
    if (effectName == "reverb") {
        auto reverb = dynamic_cast<ReverbProcessor*>(effect.processor.get());
        if (reverb) {
            juce::Reverb::Parameters params;
            params.roomSize = effect.parameters["roomSize"];
            params.damping = effect.parameters["damping"];
            params.wetLevel = effect.parameters["wetLevel"];
            params.dryLevel = effect.parameters["dryLevel"];
            params.width = effect.parameters["width"];
            params.freezeMode = effect.parameters["freezeMode"];
            reverb->setParameters(params);
        }
    }
    else if (effectName == "delay") {
        auto delay = dynamic_cast<DelayProcessor*>(effect.processor.get());
        if (delay && paramName == "delayTime") {
            delay->setDelay(value * 44100.0f);  // Convert seconds to samples
        }
    }
    else if (effectName == "distortion") {
        auto distortion = dynamic_cast<WaveShaperProcessor*>(effect.processor.get());
        if (distortion && paramName == "drive") {
            distortion->setDistortionFactor(value);
        }
    }
    else if (effectName == "compressor") {
        auto compressor = dynamic_cast<CompressorProcessor*>(effect.processor.get());
        if (compressor) {
            if (paramName == "threshold") compressor->setThreshold(value);
            else if (paramName == "ratio") compressor->setRatio(value);
            else if (paramName == "attack") compressor->setAttack(value);
            else if (paramName == "release") compressor->setRelease(value);
        }
    }
    else if (effectName == "eq") {
        auto eq = dynamic_cast<FilterProcessor*>(effect.processor.get());
        if (eq) {
            float freq = effect.parameters["frequency"];
            float q = effect.parameters["q"];
            float gain = effect.parameters["gain"];
            
            if (paramName == "frequency") freq = value;
            else if (paramName == "q") q = value;
            else if (paramName == "gain") gain = value;
            
            // Update filter coefficients
            eq->setCoefficients(juce::dsp::IIR::Coefficients<float>::makeLowShelf(
                44100, freq, q, juce::Decibels::decibelsToGain(gain)));
        }
    }
    
    return true;
}

void AudioComponentProcessor::setEnabled(bool enabled) {
    this->enabled = enabled;
}

bool AudioComponentProcessor::isEnabled() const {
    return enabled;
}

std::string AudioComponentProcessor::getName() const {
    return name;
}

void AudioComponentProcessor::applyEffects(juce::AudioBuffer<float>& buffer) {
    if (effects.empty()) {
        return;
    }
    
    // Create a block and context for processing
    juce::dsp::AudioBlock<float> block(buffer);
    juce::dsp::ProcessContextReplacing<float> context(block);
    
    // Process each effect
    for (auto& [name, effect] : effects) {
        if (effect.processor) {
            effect.processor->process(context);
        }
    }
}

void AudioComponentProcessor::applyPitchShift(juce::AudioBuffer<float>& buffer) {
    // Simple pitch shift implementation
    // Note: This is a very simple approach, a more sophisticated algorithm
    // would be needed for high-quality pitch shifting
    
    const int numChannels = buffer.getNumChannels();
    const int numSamples = buffer.getNumSamples();
    
    // Resample buffer with simple linear interpolation
    double ratio = std::pow(2.0, pitchShift / 12.0);  // Convert semitones to ratio
    
    // Create a temporary buffer
    juce::AudioBuffer<float> tempBuffer(numChannels, numSamples);
    tempBuffer.clear();
    
    for (int channel = 0; channel < numChannels; ++channel) {
        float* sourceData = buffer.getWritePointer(channel);
        float* destData = tempBuffer.getWritePointer(channel);
        
        for (int i = 0; i < numSamples; ++i) {
            double sourcePos = i * ratio;
            
            if (sourcePos < numSamples - 1) {
                int pos1 = static_cast<int>(sourcePos);
                int pos2 = pos1 + 1;
                float alpha = static_cast<float>(sourcePos - pos1);
                
                destData[i] = sourceData[pos1] * (1.0f - alpha) + sourceData[pos2] * alpha;
            } else {
                destData[i] = 0.0f;  // Out of bounds
            }
        }
    }
    
    // Copy back to original buffer
    buffer.copyFrom(0, 0, tempBuffer, 0, 0, numSamples);
}

void AudioComponentProcessor::applyTempoAdjustment(juce::AudioBuffer<float>& buffer) {
    // Simple time-stretching implementation
    // Note: This is a very simple approach, a more sophisticated algorithm
    // would be needed for high-quality time-stretching
    
    const int numChannels = buffer.getNumChannels();
    const int numSamples = buffer.getNumSamples();
    
    // Create a temporary buffer
    juce::AudioBuffer<float> tempBuffer(numChannels, numSamples);
    tempBuffer.clear();
    
    for (int channel = 0; channel < numChannels; ++channel) {
        float* sourceData = buffer.getWritePointer(channel);
        float* destData = tempBuffer.getWritePointer(channel);
        
        for (int i = 0; i < numSamples; ++i) {
            double sourcePos = i / tempoRatio;
            
            if (sourcePos < numSamples - 1) {
                int pos1 = static_cast<int>(sourcePos);
                int pos2 = pos1 + 1;
                float alpha = static_cast<float>(sourcePos - pos1);
                
                destData[i] = sourceData[pos1] * (1.0f - alpha) + sourceData[pos2] * alpha;
            } else {
                destData[i] = 0.0f;  // Out of bounds
            }
        }
    }
    
    // Copy back to original buffer
    buffer.copyFrom(0, 0, tempBuffer, 0, 0, numSamples);
}

} // namespace audio
} // namespace undergroundBeats
