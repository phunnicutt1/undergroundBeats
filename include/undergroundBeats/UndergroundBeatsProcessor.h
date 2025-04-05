
#pragma once

#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_audio_devices/juce_audio_devices.h>
#include <juce_audio_formats/juce_audio_formats.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_audio_utils/juce_audio_utils.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_gui_extra/juce_gui_extra.h>
#include <juce_dsp/juce_dsp.h>
#include <vector>
#include <atomic> // For atomic flag

// Add a namespace to match the namespace used in Main.cpp
namespace undergroundBeats {

//==============================================================================
/**
 * The main audio processing class for UndergroundBeats.
 * Handles audio processing, parameter management, and playback state.
*/
class UndergroundBeatsProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    UndergroundBeatsProcessor();
    ~UndergroundBeatsProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    
    //==============================================================================
    // File Loading Methods
    //==============================================================================
    /**
     * Load an audio file for processing
     * @param audioFile The file to load
     * @return true if the file was loaded successfully, false otherwise
     */
    bool loadAudioFile(const juce::File& audioFile);

    //==============================================================================

    /**
     * Load a new sample into a stem, replacing its buffer.
     * @param stemIndex Index of the stem to replace.
     * @param file Audio file to load.
     * @return true if successful, false otherwise.
     */
    bool loadAndSwapStem(int stemIndex, const juce::File& file);

    // Stem Access (NEW)
    //==============================================================================
    /** Returns a constant reference to the vector of separated stem audio buffers. */
    const std::vector<juce::AudioBuffer<float>>& getSeparatedStemBuffers() const;


    //==============================================================================
    // Playback Control Methods (NEW)
    //==============================================================================
    /** Starts audio playback from the current position or beginning. */
    void startPlayback();

    /** Pauses audio playback at the current position. */
    void pausePlayback();

    /** Stops audio playback and resets the position to the beginning. */
    void stopPlayback();

    /** Returns true if audio is currently playing (not paused or stopped). */
    bool isPlaying() const;

    /** Returns true if audio playback is currently paused. */
    bool isPaused() const;

    //==============================================================================
    // Parameter Management (NEW)
    //==============================================================================
    /** Returns a reference to the processor's value tree state. */
    juce::AudioProcessorValueTreeState& getValueTreeState();

    /** Returns a reference to the flag indicating parameter changes. */
    std::atomic<bool>& getParametersChangedFlag();

    /** Generates a unique parameter ID string for a given stem index and parameter type. */
    static juce::String getStemParameterID(int stemIndex, const juce::String& paramType); // e.g., "Volume", "Gain"

private:
    //==============================================================================
    // Parameter Management (NEW)
    juce::AudioProcessorValueTreeState valueTreeState;
    juce::UndoManager undoManager; // Optional but good practice for state management
    std::atomic<bool> parametersChanged { false }; // Flag for UI updates

    // Helper function to create the parameter layout (NEW)
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    //==============================================================================
    // DSP Effect Chains per Stem (NEW)
    using StemEffectChain = juce::dsp::ProcessorChain<
        juce::dsp::IIR::Filter<float>,  // EQ Band 1
        juce::dsp::IIR::Filter<float>,  // EQ Band 2
        juce::dsp::IIR::Filter<float>,  // EQ Band 3
        juce::dsp::Compressor<float>,   // Compressor
        juce::dsp::Gain<float>          // Placeholder Style Transfer
    >;

    std::vector<StemEffectChain> stemEffectChains;

    //==============================================================================
    // Playback State Variables (NEW)
    std::atomic<bool> playing { false }; // Use atomic for thread safety from UI calls
    std::atomic<bool> paused { false };
    // Add other necessary state variables like current position, sample rate etc.
    // double currentSampleRate = 0.0;
    // juce::int64 currentSamplePosition = 0; // Example position tracking
    
    // Audio file related members
    juce::AudioFormatManager formatManager;
    std::unique_ptr<juce::AudioFormatReader> currentAudioReader;
    juce::AudioBuffer<float> audioBuffer;
    juce::File currentAudioFile;

    // Stem separation related members (NEW)
    std::vector<juce::AudioBuffer<float>> separatedStemBuffers;


    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (UndergroundBeatsProcessor)
};

} // namespace undergroundBeats
