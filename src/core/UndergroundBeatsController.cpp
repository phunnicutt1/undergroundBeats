#include "undergroundBeats/core/UndergroundBeatsController.h"
#include <juce_audio_formats/juce_audio_formats.h>
#include <juce_core/juce_core.h>
#include <random>
#include <chrono>
#include <iomanip>
#include <sstream>

namespace undergroundBeats {
namespace core {

UndergroundBeatsController::UndergroundBeatsController()
    : audioSourceSeparator(std::make_unique<audio::AudioSourceSeparator>()),
      variationGenerator(std::make_unique<ml::VariationGenerator>()) {
}

UndergroundBeatsController::~UndergroundBeatsController() = default;

bool UndergroundBeatsController::initialize() {
    // Initialize audio source separator
    bool sourceInitialized = audioSourceSeparator->initialize("default");
    
    // Initialize variation generator with algorithmic method
    bool varGenInitialized = variationGenerator->initialize(ml::VariationMethod::ALGORITHMIC);
    
    return sourceInitialized && varGenInitialized;
}

bool UndergroundBeatsController::loadAudioFile(const std::string& filePath) {
    // Load audio file
    juce::AudioFormatManager formatManager;
    formatManager.registerBasicFormats();
    
    std::unique_ptr<juce::AudioFormatReader> reader(
        formatManager.createReaderFor(juce::File(filePath)));
    
    if (!reader) {
        juce::Logger::writeToLog("Failed to load audio file: " + filePath);
        return false;
    }
    
    // Read audio data into original buffer
    originalAudioBuffer.setSize(reader->numChannels, reader->lengthInSamples);
    reader->read(&originalAudioBuffer, 0, reader->lengthInSamples, 0, true, true);
    
    // Separate into components
    auto components = audioSourceSeparator->separateAudioBuffer(originalAudioBuffer);
    
    // Create processors for each component
    componentProcessors.clear();
    for (const auto& [name, buffer] : components) {
        auto processor = std::make_unique<audio::AudioComponentProcessor>(name);
        processor->setAudioData(buffer);
        componentProcessors[name] = std::move(processor);
    }
    
    // Initialize mixed output buffer
    mixedOutputBuffer.setSize(originalAudioBuffer.getNumChannels(), originalAudioBuffer.getNumSamples());
    updateMixedOutput();
    
    // Notify UI
    notifyUIUpdate();
    
    return true;
}

bool UndergroundBeatsController::saveAudioFile(const std::string& filePath) {
    // Make sure we have something to save
    if (mixedOutputBuffer.getNumSamples() == 0) {
        juce::Logger::writeToLog("No audio to save");
        return false;
    }
    
    // Create a WAV writer
    juce::WavAudioFormat wavFormat;
    std::unique_ptr<juce::AudioFormatWriter> writer(
        wavFormat.createWriterFor(
            new juce::FileOutputStream(juce::File(filePath)),
            44100,  // Sample rate - assume 44.1kHz
            mixedOutputBuffer.getNumChannels(),
            16,     // Bit depth - 16-bit
            {},     // Metadata
            0       // Quality option - not used for WAV
        ));
    
    if (!writer) {
        juce::Logger::writeToLog("Failed to create audio writer for: " + filePath);
        return false;
    }
    
    // Write the audio data
    bool writeSuccess = writer->writeFromAudioSampleBuffer(
        mixedOutputBuffer, 0, mixedOutputBuffer.getNumSamples());
    
    return writeSuccess;
}

std::vector<std::string> UndergroundBeatsController::generateVariations(
    const std::string& componentName,
    ml::VariationMethod method,
    int numVariations) {
    
    // Check if the component exists
    auto processorIt = componentProcessors.find(componentName);
    if (processorIt == componentProcessors.end()) {
        juce::Logger::writeToLog("Component not found: " + componentName);
        return {};
    }
    
    // Configure the variation generator
    variationGenerator->initialize(method);
    
    // Get the component's audio buffer
    const juce::AudioBuffer<float>& componentBuffer = 
        getComponentAudioBuffer(componentName);
    
    // Generate variations
    std::vector<juce::AudioBuffer<float>> variationBuffers = 
        variationGenerator->generateVariations(componentBuffer, numVariations);
    
    // Store variations and return their IDs
    std::vector<std::string> variationIds;
    variationIds.reserve(variationBuffers.size());
    
    for (auto& buffer : variationBuffers) {
        std::string id = generateUniqueId();
        
        Variation variation;
        variation.id = id;
        variation.componentName = componentName;
        variation.audioBuffer = std::move(buffer);
        
        variations[id] = std::move(variation);
        variationIds.push_back(id);
    }
    
    return variationIds;
}

bool UndergroundBeatsController::applyVariation(
    const std::string& componentName,
    const std::string& variationId) {
    
    // Check if the component exists
    auto processorIt = componentProcessors.find(componentName);
    if (processorIt == componentProcessors.end()) {
        juce::Logger::writeToLog("Component not found: " + componentName);
        return false;
    }
    
    // Check if the variation exists
    auto variationIt = variations.find(variationId);
    if (variationIt == variations.end()) {
        juce::Logger::writeToLog("Variation not found: " + variationId);
        return false;
    }
    
    // Check if the variation is for this component
    const Variation& variation = variationIt->second;
    if (variation.componentName != componentName) {
        juce::Logger::writeToLog("Variation is for a different component");
        return false;
    }
    
    // Apply the variation
    processorIt->second->setAudioData(variation.audioBuffer);
    
    // Update the mixed output
    updateMixedOutput();
    
    // Notify UI
    notifyUIUpdate();
    
    return true;
}

void UndergroundBeatsController::setComponentGain(
    const std::string& componentName,
    float gainDB) {
    
    auto it = componentProcessors.find(componentName);
    if (it != componentProcessors.end()) {
        it->second->setGain(gainDB);
        updateMixedOutput();
        notifyUIUpdate();
    }
}

void UndergroundBeatsController::setComponentPitchShift(
    const std::string& componentName,
    float semitones) {
    
    auto it = componentProcessors.find(componentName);
    if (it != componentProcessors.end()) {
        it->second->setPitchShift(semitones);
        updateMixedOutput();
        notifyUIUpdate();
    }
}

void UndergroundBeatsController::setComponentTempoAdjustment(
    const std::string& componentName,
    float tempoRatio) {
    
    auto it = componentProcessors.find(componentName);
    if (it != componentProcessors.end()) {
        it->second->setTempoAdjustment(tempoRatio);
        updateMixedOutput();
        notifyUIUpdate();
    }
}

bool UndergroundBeatsController::addComponentEffect(
    const std::string& componentName,
    const std::string& effectName) {
    
    auto it = componentProcessors.find(componentName);
    if (it != componentProcessors.end()) {
        bool success = it->second->addEffect(effectName);
        if (success) {
            updateMixedOutput();
            notifyUIUpdate();
        }
        return success;
    }
    return false;
}

bool UndergroundBeatsController::removeComponentEffect(
    const std::string& componentName,
    const std::string& effectName) {
    
    auto it = componentProcessors.find(componentName);
    if (it != componentProcessors.end()) {
        bool success = it->second->removeEffect(effectName);
        if (success) {
            updateMixedOutput();
            notifyUIUpdate();
        }
        return success;
    }
    return false;
}

bool UndergroundBeatsController::setComponentEffectParameter(
    const std::string& componentName,
    const std::string& effectName,
    const std::string& paramName,
    float value) {
    
    auto it = componentProcessors.find(componentName);
    if (it != componentProcessors.end()) {
        bool success = it->second->setEffectParameter(effectName, paramName, value);
        if (success) {
            updateMixedOutput();
            notifyUIUpdate();
        }
        return success;
    }
    return false;
}

void UndergroundBeatsController::setComponentEnabled(
    const std::string& componentName,
    bool enabled) {
    
    auto it = componentProcessors.find(componentName);
    if (it != componentProcessors.end()) {
        it->second->setEnabled(enabled);
        updateMixedOutput();
        notifyUIUpdate();
    }
}

std::vector<std::string> UndergroundBeatsController::getComponentNames() const {
    std::vector<std::string> names;
    names.reserve(componentProcessors.size());
    
    for (const auto& [name, _] : componentProcessors) {
        names.push_back(name);
    }
    
    return names;
}

std::vector<std::string> UndergroundBeatsController::getAvailableEffects() const {
    // Fixed list of available effects
    return {
        "reverb",
        "delay",
        "distortion",
        "compressor",
        "eq"
    };
}

const juce::AudioBuffer<float>& UndergroundBeatsController::getComponentAudioBuffer(
    const std::string& componentName) const {
    
    static juce::AudioBuffer<float> emptyBuffer;
    
    auto it = componentProcessors.find(componentName);
    if (it != componentProcessors.end()) {
        const auto& processor = it->second;
        // Return the audio buffer only if the processor is enabled
        if (processor->isEnabled()) {
            // We should get the processor's audio data here, not the name
            // Since AudioComponentProcessor doesn't have a getAudioData method,
            // we'll return emptyBuffer for now
            return emptyBuffer;
        }
    }
    
    return emptyBuffer;
}

void UndergroundBeatsController::registerUIUpdateCallback(std::function<void()> callback) {
    uiUpdateCallback = callback;
}

void UndergroundBeatsController::updateMixedOutput() {
    // Clear mixed output
    mixedOutputBuffer.clear();
    
    // Mix all enabled components
    for (const auto& [name, processor] : componentProcessors) {
        if (processor->isEnabled()) {
            processor->processAudio(mixedOutputBuffer, mixedOutputBuffer.getNumSamples());
        }
    }
}

void UndergroundBeatsController::notifyUIUpdate() {
    if (uiUpdateCallback) {
        uiUpdateCallback();
    }
}

std::string UndergroundBeatsController::generateUniqueId() {
    // Create a timestamp-based ID
    auto now = std::chrono::system_clock::now();
    auto now_ms = std::chrono::time_point_cast<std::chrono::milliseconds>(now);
    auto value = now_ms.time_since_epoch().count();
    
    std::stringstream ss;
    ss << std::hex << value;
    
    // Add a random component to ensure uniqueness
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 0xFFFF);
    
    ss << "-" << std::hex << std::setw(4) << std::setfill('0') << dis(gen);
    
    return ss.str();
}

} // namespace core
} // namespace undergroundBeats
