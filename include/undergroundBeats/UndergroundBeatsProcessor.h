#pragma once

#include "core/UndergroundBeatsController.h"
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_audio_formats/juce_audio_formats.h>
#include <juce_audio_utils/juce_audio_utils.h>
#include <memory>

namespace undergroundBeats {

/**
 * @class UndergroundBeatsProcessor
 * @brief Main audio processor for the Underground Beats application
 * 
 * This class provides the audio processing functionality for the application
 * and creates the editor.
 */
class UndergroundBeatsProcessor : public juce::AudioProcessor {
public:
    /**
     * @brief Constructor
     */
    UndergroundBeatsProcessor();
    
    /**
     * @brief Destructor
     */
    ~UndergroundBeatsProcessor() override;
    
    // AudioProcessor overrides (required)
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;
    
    const juce::String getName() const override;
    
    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;
    
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override;
    void changeProgramName(int index, const juce::String& newName) override;
    
    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;
    
    /**
     * @brief Get the format manager
     * @return Reference to the format manager
     */
    juce::AudioFormatManager* getFormatManager() { return &formatManager; }
    
    /**
     * @brief Get the audio thumbnail cache
     * @return Reference to the audio thumbnail cache
     */
    juce::AudioThumbnailCache* getAudioThumbnailCache() { return thumbnailCache.get(); }
    
private:
    core::UndergroundBeatsController controller;
    juce::AudioFormatManager formatManager;
    std::unique_ptr<juce::AudioThumbnailCache> thumbnailCache;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(UndergroundBeatsProcessor)
};

} // namespace undergroundBeats
