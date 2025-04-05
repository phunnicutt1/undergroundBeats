#pragma once

#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_dsp/juce_dsp.h>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include <functional>
#include <cmath>

namespace undergroundBeats {
namespace audio {

/**
 * Forward declaration of the EffectProcessor base class
 */
class EffectProcessor;

/**
 * @class AudioComponentProcessor
 * @brief Processes individual audio components
 * 
 * This class provides functionality to manipulate individual audio components
 * (e.g., adjust volume, pitch, apply effects).
 */
class AudioComponentProcessor {
public:
    /**
     * @brief Constructor
     * @param name The name of the component
     */
    AudioComponentProcessor(const std::string& name);
    
    /**
     * @brief Destructor
     */
    ~AudioComponentProcessor();
    
    /**
     * @brief Set the audio data for this component
     * @param audioData The audio buffer containing component data
     */
    void setAudioData(const juce::AudioBuffer<float>& audioData);
    
    /**
     * @brief Process a segment of the audio data with current settings and add it to an output buffer.
     * @param outputBuffer The buffer to write processed audio into.
     * @param outputStartSample The starting sample index in the output buffer.
     * @param componentStartSample The starting sample index within this component's internal audioData.
     * @param numSamplesToProcess The number of samples to process from the component.
     */
    void processAudio(juce::AudioBuffer<float>& outputBuffer, 
                      int outputStartSample, 
                      int64_t componentStartSample, 
                      int numSamplesToProcess);
    
    /**
     * @brief Set the gain for this component
     * @param gainDB The gain in decibels
     */
    void setGain(float gainDB);
    
    /**
     * @brief Set the pitch shift for this component
     * @param semitones The pitch shift in semitones
     */
    void setPitchShift(float semitones);
    
    /**
     * @brief Set the tempo adjustment for this component
     * @param tempoRatio The tempo ratio (1.0 = original tempo)
     */
    void setTempoAdjustment(float tempoRatio);
    
    /**
     * @brief Add an effect to this component
     * @param effectName The name of the effect to add
     * @return True if the effect was added successfully
     */
    bool addEffect(const std::string& effectName);
    
    /**
     * @brief Remove an effect from this component
     * @param effectName The name of the effect to remove
     * @return True if the effect was removed successfully
     */
    bool removeEffect(const std::string& effectName);
    
    /**
     * @brief Set a parameter for an effect
     * @param effectName The name of the effect
     * @param paramName The name of the parameter
     * @param value The value to set
     * @return True if the parameter was set successfully
     */
    bool setEffectParameter(const std::string& effectName, 
                           const std::string& paramName, 
                           float value);
    
    /**
     * @brief Enable or disable this component
     * @param enabled Whether this component should be enabled
     */
    void setEnabled(bool enabled);
    
    /**
     * @brief Check if this component is enabled
     * @return True if this component is enabled
     */
    bool isEnabled() const;
    
    /**
     * @brief Get the name of this component
     * @return The name of this component
     */
    std::string getName() const;
    
    /**
     * @brief Prepare the processor for playback with a given sample rate and block size.
     * @param sampleRate The sample rate.
     * @param maximumExpectedSamplesPerBlock The maximum block size.
     */
    void prepare(double sampleRate, int maximumExpectedSamplesPerBlock);
    
    /**
     * @brief Reset the internal state of the processor (e.g., effects).
     */
    void reset();

private:
    // Component properties
    std::string name;
    bool enabled;
    float gain;
    float pitchShift;
    float tempoRatio;
    
    // Audio data
    juce::AudioBuffer<float> audioData;
    
    // Effect processors
    struct Effect {
        std::unique_ptr<EffectProcessor> processor;
        std::unordered_map<std::string, float> parameters;
    };
    
    std::unordered_map<std::string, Effect> effects;
    
    // Internal processing
    // void applyEffects(juce::AudioBuffer<float>& buffer); // Defer segmented effects
    // void applyPitchShift(juce::AudioBuffer<float>& buffer); // Defer segmented pitch/tempo
    // void applyTempoAdjustment(juce::AudioBuffer<float>& buffer); // Defer segmented pitch/tempo
    
    double currentSampleRate = 44100.0;
    int currentBlockSize = 512;
};

/**
 * @class EffectProcessor
 * @brief Base class for all effect processors
 * 
 * This class provides a common interface for all effect processors
 */
class EffectProcessor {
public:
    virtual ~EffectProcessor() = default;
    
    /**
     * @brief Process the audio data with current settings
     * @param context The processing context
     */
    virtual void process(juce::dsp::ProcessContextReplacing<float>& context) = 0;
    
    /**
     * @brief Prepare the processor for playback
     * @param spec The processing specification
     */
    virtual void prepare(const juce::dsp::ProcessSpec& spec) = 0;
    
    /**
     * @brief Reset the processor state
     */
    virtual void reset() = 0;
};

/**
 * @class ReverbProcessor
 * @brief Wrapper for JUCE's Reverb processor
 */
class ReverbProcessor : public EffectProcessor {
public:
    ReverbProcessor() = default;
    
    void process(juce::dsp::ProcessContextReplacing<float>& context) override {
        reverb.process(context);
    }
    
    void prepare(const juce::dsp::ProcessSpec& spec) override {
        reverb.prepare(spec);
    }
    
    void reset() override {
        reverb.reset();
    }
    
    void setParameters(const juce::Reverb::Parameters& params) {
        reverb.setParameters(params);
    }
    
    juce::dsp::Reverb reverb;
};

/**
 * @class DelayProcessor
 * @brief Wrapper for JUCE's DelayLine processor
 */
class DelayProcessor : public EffectProcessor {
public:
    DelayProcessor(int maxDelaySamples) {
        delay.setMaximumDelayInSamples(maxDelaySamples);
    }
    
    void process(juce::dsp::ProcessContextReplacing<float>& context) override {
        delay.process(context);
    }
    
    void prepare(const juce::dsp::ProcessSpec& spec) override {
        delay.prepare(spec);
    }
    
    void reset() override {
        delay.reset();
    }
    
    void setDelay(float delaySamples) {
        delay.setDelay(delaySamples);
    }
    
    juce::dsp::DelayLine<float> delay;
};

// Static distortion functions for WaveShaper
namespace DistortionFunctions {
    // Simple tanh distortion with variable drive
    static float driveValue = 2.0f;
    
    static float tanhDistortion(float x) {
        return std::tanh(x * driveValue);
    }
}

/**
 * @class WaveShaperProcessor
 * @brief Wrapper for JUCE's WaveShaper processor
 */
class WaveShaperProcessor : public EffectProcessor {
public:
    WaveShaperProcessor() {
        // Initialize with default distortion
        shaper.functionToUse = DistortionFunctions::tanhDistortion;
    }
    
    void process(juce::dsp::ProcessContextReplacing<float>& context) override {
        shaper.process(context);
    }
    
    void prepare(const juce::dsp::ProcessSpec& spec) override {
        shaper.prepare(spec);
    }
    
    void reset() override {
        shaper.reset();
    }
    
    void setDistortionFactor(float factor) {
        // Update the global drive value used by the static function
        DistortionFunctions::driveValue = factor;
    }
    
    juce::dsp::WaveShaper<float> shaper;
};

/**
 * @class CompressorProcessor
 * @brief Wrapper for JUCE's Compressor processor
 */
class CompressorProcessor : public EffectProcessor {
public:
    CompressorProcessor() = default;
    
    void process(juce::dsp::ProcessContextReplacing<float>& context) override {
        compressor.process(context);
    }
    
    void prepare(const juce::dsp::ProcessSpec& spec) override {
        compressor.prepare(spec);
    }
    
    void reset() override {
        compressor.reset();
    }
    
    void setThreshold(float thresholdDb) {
        compressor.setThreshold(thresholdDb);
    }
    
    void setRatio(float ratio) {
        compressor.setRatio(ratio);
    }
    
    void setAttack(float attackMs) {
        compressor.setAttack(attackMs);
    }
    
    void setRelease(float releaseMs) {
        compressor.setRelease(releaseMs);
    }
    
    juce::dsp::Compressor<float> compressor;
};

/**
 * @class FilterProcessor
 * @brief Wrapper for JUCE's IIR Filter processor
 */
class FilterProcessor : public EffectProcessor {
public:
    FilterProcessor() = default;
    
    void process(juce::dsp::ProcessContextReplacing<float>& context) override {
        filter.process(context);
    }
    
    void prepare(const juce::dsp::ProcessSpec& spec) override {
        filter.prepare(spec);
    }
    
    void reset() override {
        filter.reset();
    }
    
    void setCoefficients(const juce::dsp::IIR::Coefficients<float>::Ptr& coefficients) {
        filter.coefficients = coefficients;
    }
    
    juce::dsp::IIR::Filter<float> filter;
};

} // namespace audio
} // namespace undergroundBeats
