#include "../include/undergroundBeats/UndergroundBeatsProcessor.h"
#include "../include/undergroundBeats/gui/MainEditor.h"
#include "../include/undergroundBeats/ml/ONNXSourceSeparator.h"
#include <juce_audio_basics/juce_audio_basics.h> // For NormalisableRange

// Include iostream for temporary debugging output (optional)
#include <iostream>

//==============================================================================
// Add namespace to match the header
namespace undergroundBeats {

UndergroundBeatsProcessor::UndergroundBeatsProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                      #endif
                        ),
    valueTreeState(*this, &undoManager, juce::Identifier("UndergroundBeatsParams"), createParameterLayout()) // Initialize valueTreeState (NEW)
#endif
{
    // Constructor initialization
    // Register basic audio formats (WAV, AIFF, etc.)
    formatManager.registerBasicFormats();
    
    std::cout << "UndergroundBeatsProcessor created." << std::endl;
}

UndergroundBeatsProcessor::~UndergroundBeatsProcessor()
{
    // Destructor
    std::cout << "UndergroundBeatsProcessor destroyed." << std::endl;
}

//==============================================================================
const juce::String UndergroundBeatsProcessor::getName() const
{
    // Return a fixed name instead of relying on JucePlugin_Name
    return "UndergroundBeats";
}

bool UndergroundBeatsProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool UndergroundBeatsProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool UndergroundBeatsProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double UndergroundBeatsProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int UndergroundBeatsProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int UndergroundBeatsProcessor::getCurrentProgram()
{
    return 0;
}

void UndergroundBeatsProcessor::setCurrentProgram (int index)
{
    juce::ignoreUnused (index);
}

const juce::String UndergroundBeatsProcessor::getProgramName (int index)
{
    juce::ignoreUnused (index);
    return {};
}

void UndergroundBeatsProcessor::changeProgramName (int index, const juce::String& newName)
{
    juce::ignoreUnused (index, newName);
}

//==============================================================================
// Parameter Management Implementation (NEW)
//==============================================================================
juce::String UndergroundBeatsProcessor::getStemParameterID(int stemIndex, const juce::String& paramType)
{
    // Creates IDs like "Stem_0_Volume", "Stem_1_Gain", etc.
    return "Stem_" + juce::String(stemIndex) + "_" + paramType;
}

juce::AudioProcessorValueTreeState::ParameterLayout UndergroundBeatsProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;
    const int maxStems = 8; // Define a maximum number of stems for parameter creation

    for (int i = 0; i < maxStems; ++i)
    {
        // Volume Parameter
        auto volumeParamID = getStemParameterID(i, "Volume");
        auto volumeParamName = "Stem " + juce::String(i) + " Volume";
        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            volumeParamID, volumeParamName,
            juce::NormalisableRange<float>(0.0f, 1.0f), 0.8f,
            juce::String(), juce::AudioProcessorParameter::genericParameter,
            [](float value, int) { return juce::String(value, 2); }
        ));

        // Gain Parameter (dB)
        auto gainParamID = getStemParameterID(i, "Gain");
        auto gainParamName = "Stem " + juce::String(i) + " Gain";
        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            gainParamID, gainParamName,
            juce::NormalisableRange<float>(-60.0f, 12.0f), 0.0f,
            juce::String(), juce::AudioProcessorParameter::genericParameter,
            [](float value, int) { return juce::String(value, 1) + " dB"; },
            [](const juce::String& text) { return text.removeCharacters(" dB").getFloatValue(); }
        ));

        // Mute Parameter
        auto muteParamID = getStemParameterID(i, "Mute");
        auto muteParamName = "Stem " + juce::String(i) + " Mute";
        params.push_back(std::make_unique<juce::AudioParameterBool>(
            muteParamID, muteParamName,
            false));

        // Solo Parameter
        auto soloParamID = getStemParameterID(i, "Solo");
        auto soloParamName = "Stem " + juce::String(i) + " Solo";
        params.push_back(std::make_unique<juce::AudioParameterBool>(
            soloParamID, soloParamName,
            false));

        // ===== EQ Parameters =====
        for (int band = 1; band <= 3; ++band)
        {
            auto prefix = "EQ" + juce::String(band);
            // Enable toggle per band (optional: could be global EQ enable)
            params.push_back(std::make_unique<juce::AudioParameterBool>(
                getStemParameterID(i, prefix + "_Enable"),
                "Stem " + juce::String(i) + " " + prefix + " Enable",
                true));
            // Frequency
            params.push_back(std::make_unique<juce::AudioParameterFloat>(
                getStemParameterID(i, prefix + "_Freq"),
                "Stem " + juce::String(i) + " " + prefix + " Frequency",
                juce::NormalisableRange<float>(20.0f, 20000.0f, 1.0f, 0.5f), // skewed
                (band == 1 ? 100.0f : (band == 2 ? 1000.0f : 5000.0f))));
            // Gain
            params.push_back(std::make_unique<juce::AudioParameterFloat>(
                getStemParameterID(i, prefix + "_Gain"),
                "Stem " + juce::String(i) + " " + prefix + " Gain",
                juce::NormalisableRange<float>(-24.0f, 24.0f), 0.0f));
            // Q
            params.push_back(std::make_unique<juce::AudioParameterFloat>(
                getStemParameterID(i, prefix + "_Q"),
                "Stem " + juce::String(i) + " " + prefix + " Q",
                juce::NormalisableRange<float>(0.1f, 10.0f), 1.0f));
        }

        // ===== Compressor Parameters =====
        params.push_back(std::make_unique<juce::AudioParameterBool>(
            getStemParameterID(i, "Comp_Enable"),
            "Stem " + juce::String(i) + " Compressor Enable",
            true));
        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            getStemParameterID(i, "Comp_Threshold"),
            "Stem " + juce::String(i) + " Compressor Threshold",
            juce::NormalisableRange<float>(-60.0f, 0.0f), -24.0f));
        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            getStemParameterID(i, "Comp_Ratio"),
            "Stem " + juce::String(i) + " Compressor Ratio",
            juce::NormalisableRange<float>(1.0f, 20.0f), 4.0f));
        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            getStemParameterID(i, "Comp_Attack"),
            "Stem " + juce::String(i) + " Compressor Attack",
            juce::NormalisableRange<float>(0.1f, 100.0f), 10.0f));
        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            getStemParameterID(i, "Comp_Release"),
            "Stem " + juce::String(i) + " Compressor Release",
            juce::NormalisableRange<float>(5.0f, 500.0f), 50.0f));

        // ===== Reverb Parameters =====
        params.push_back(std::make_unique<juce::AudioParameterBool>(
            getStemParameterID(i, "Reverb_Enable"),
            "Stem " + juce::String(i) + " Reverb Enable",
            false));
        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            getStemParameterID(i, "Reverb_RoomSize"),
            "Stem " + juce::String(i) + " Reverb Room Size",
            juce::NormalisableRange<float>(0.0f, 1.0f), 0.5f));
        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            getStemParameterID(i, "Reverb_Damping"),
            "Stem " + juce::String(i) + " Reverb Damping",
            juce::NormalisableRange<float>(0.0f, 1.0f), 0.5f));
        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            getStemParameterID(i, "Reverb_WetLevel"),
            "Stem " + juce::String(i) + " Reverb Wet Level",
            juce::NormalisableRange<float>(0.0f, 1.0f), 0.33f));
        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            getStemParameterID(i, "Reverb_DryLevel"),
            "Stem " + juce::String(i) + " Reverb Dry Level",
            juce::NormalisableRange<float>(0.0f, 1.0f), 0.4f));
        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            getStemParameterID(i, "Reverb_Width"),
            "Stem " + juce::String(i) + " Reverb Width",
            juce::NormalisableRange<float>(0.0f, 1.0f), 1.0f));
        params.push_back(std::make_unique<juce::AudioParameterBool>(
            getStemParameterID(i, "Reverb_Freeze"),
            "Stem " + juce::String(i) + " Reverb Freeze",
            false));

        // ===== Delay Parameters =====
        params.push_back(std::make_unique<juce::AudioParameterBool>(
            getStemParameterID(i, "Delay_Enable"),
            "Stem " + juce::String(i) + " Delay Enable",
            false));
        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            getStemParameterID(i, "Delay_Time"),
            "Stem " + juce::String(i) + " Delay Time (ms)",
            juce::NormalisableRange<float>(1.0f, 2000.0f), 500.0f));
        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            getStemParameterID(i, "Delay_Feedback"),
            "Stem " + juce::String(i) + " Delay Feedback",
            juce::NormalisableRange<float>(0.0f, 0.95f), 0.5f));
        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            getStemParameterID(i, "Delay_Mix"),
            "Stem " + juce::String(i) + " Delay Mix",
            juce::NormalisableRange<float>(0.0f, 1.0f), 0.5f));

        // ===== Chorus Parameters =====
        params.push_back(std::make_unique<juce::AudioParameterBool>(
            getStemParameterID(i, "Chorus_Enable"),
            "Stem " + juce::String(i) + " Chorus Enable",
            false));
        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            getStemParameterID(i, "Chorus_Rate"),
            "Stem " + juce::String(i) + " Chorus Rate",
            juce::NormalisableRange<float>(0.0f, 10.0f), 1.5f));
        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            getStemParameterID(i, "Chorus_Depth"),
            "Stem " + juce::String(i) + " Chorus Depth",
            juce::NormalisableRange<float>(0.0f, 1.0f), 0.5f));
        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            getStemParameterID(i, "Chorus_CentreDelay"),
            "Stem " + juce::String(i) + " Chorus Centre Delay",
            juce::NormalisableRange<float>(1.0f, 100.0f), 7.0f));
        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            getStemParameterID(i, "Chorus_Feedback"),
            "Stem " + juce::String(i) + " Chorus Feedback",
            juce::NormalisableRange<float>(-1.0f, 1.0f), 0.0f));
        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            getStemParameterID(i, "Chorus_Mix"),
            "Stem " + juce::String(i) + " Chorus Mix",
            juce::NormalisableRange<float>(0.0f, 1.0f), 0.5f));

        // ===== Saturation Parameters =====
        params.push_back(std::make_unique<juce::AudioParameterBool>(
            getStemParameterID(i, "Saturation_Enable"),
            "Stem " + juce::String(i) + " Saturation Enable",
            false));
        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            getStemParameterID(i, "Saturation_Amount"),
            "Stem " + juce::String(i) + " Saturation Amount",
            juce::NormalisableRange<float>(0.0f, 10.0f), 1.0f));

        // ===== Style Transfer Placeholder =====
        params.push_back(std::make_unique<juce::AudioParameterBool>(
            getStemParameterID(i, "Style_Enable"),
            "Stem " + juce::String(i) + " Style Transfer Enable",
            true));
    }

    return { params.begin(), params.end() };
}

juce::AudioProcessorValueTreeState& UndergroundBeatsProcessor::getValueTreeState()
{
    return valueTreeState;
}

std::atomic<bool>& UndergroundBeatsProcessor::getParametersChangedFlag()
{
    return parametersChanged;
}

//==============================================================================
// File Loading Method Implementation
//==============================================================================
bool UndergroundBeatsProcessor::loadAudioFile(const juce::File& audioFile)
{
    // Check if the file exists
    if (!audioFile.existsAsFile())
    {
        std::cout << "Error: File does not exist: " << audioFile.getFullPathName() << std::endl;
        return false;
    }
    
    // Try to create a reader for the file
    std::unique_ptr<juce::AudioFormatReader> reader(formatManager.createReaderFor(audioFile));
    
    if (reader == nullptr)
    {
        std::cout << "Error: Could not create reader for file: " << audioFile.getFullPathName() << std::endl;
        return false;
    }
    
    // File loaded successfully, store it
    currentAudioFile = audioFile;
    
    // Create an audio buffer with the correct size
    int numChannels = reader->numChannels;
    int numSamples = static_cast<int>(reader->lengthInSamples);
    
    // Make sure we don't exceed reasonable limits
    numChannels = juce::jmin(numChannels, 2); // Limit to stereo
    
    // Allocate the audio buffer
    audioBuffer.setSize(numChannels, numSamples);
    
    // Read the entire file into the buffer
    reader->read(&audioBuffer, 0, numSamples, 0, true, true);
    
    // Store the reader for later use if needed
    currentAudioReader = std::move(reader);
    
    // Reset playback state
    playing = false;
    paused = false;
    
    std::cout << "Audio file loaded: " << audioFile.getFullPathName() << std::endl;
    std::cout << "Channels: " << numChannels << ", Samples: " << numSamples << std::endl;
    std::cout << "Sample Rate: " << currentAudioReader->sampleRate << " Hz" << std::endl;
    DBG("Processor: loadAudioFile - Starting separation process for: " + audioFile.getFullPathName());
    
    // --- Run ONNX Source Separation ---
    bool separationSuccessful = false;
    try
    {
        DBG("Processor: Creating ONNXSourceSeparator...");
        // Use correct namespace and constructor
        ml::ONNXSourceSeparator separator("models/source_separation.onnx", modelLoader);
        
        DBG("Processor: Calling separator.loadAndSeparate...");
        // Load and separate
        separationSuccessful = separator.loadAndSeparate(audioFile);
        // Convert bool to string for DBG
        DBG("Processor: separator.loadAndSeparate returned: " + juce::String(separationSuccessful ? "true" : "false"));

        if (separationSuccessful)
        {
            separatedStemBuffers.clear();
            int numStems = separator.getNumberOfStems();
            DBG("Processor: Separation successful. Number of stems: " + juce::String(numStems));
            separatedStemBuffers.reserve(numStems); // Reserve space
            for (int i = 0; i < numStems; ++i)
            {
                auto stemBuffer = separator.getStemBuffer(i);
                DBG("  Stem " + juce::String(i) + " buffer size: " 
                    + juce::String(stemBuffer.getNumChannels()) + " channels, " 
                    + juce::String(stemBuffer.getNumSamples()) + " samples.");
                separatedStemBuffers.push_back(std::move(stemBuffer)); // Use move if possible
            }
            DBG("Processor: Finished retrieving stem buffers.");
        }
        else
        {
            DBG("Processor: ONNX separation reported failure (loadAndSeparate returned false).");
        }
    }
    catch (const std::exception& e)
    {
        DBG("Processor: ONNX separation failed with exception: " + juce::String(e.what()));
        separationSuccessful = false;
    }

    // Fallback: create placeholder stems if separation failed
    if (!separationSuccessful)
    {
        DBG("Processor: Falling back to placeholder stems.");
        separatedStemBuffers.clear();
        int numPlaceholderStems = 4;
        separatedStemBuffers.reserve(numPlaceholderStems);
        for (int i = 0; i < numPlaceholderStems; ++i)
        {
            if (audioBuffer.getNumSamples() > 0 && audioBuffer.getNumChannels() > 0)
            {
                DBG("  Creating placeholder stem " + juce::String(i) + " from original buffer.");
                separatedStemBuffers.emplace_back(audioBuffer);
            }
            else
            {
                 DBG("  Creating empty placeholder stem " + juce::String(i) + " as original buffer is invalid.");
                separatedStemBuffers.emplace_back(); // Add empty buffer if original is invalid
            }
        }
        DBG("Processor: Created " + juce::String(separatedStemBuffers.size()) + " placeholder stems.");
    }
    
    DBG("Processor: Final separatedStemBuffers size: " + juce::String(separatedStemBuffers.size()));

    // Simply resize the vector. prepareToPlay will handle preparing the chains later.
    stemEffectChains.resize(separatedStemBuffers.size());
    
    parametersChanged = true; // Signal UI that parameters might need refreshing (NEW)
    // Reset playback position for the new stems
    playbackPosition = 0; 
    
    return true;
}


//==============================================================================
// Stem Access Implementation (NEW)
//==============================================================================
const std::vector<juce::AudioBuffer<float>>& UndergroundBeatsProcessor::getSeparatedStemBuffers() const
{
    return separatedStemBuffers;
}


//==============================================================================
void UndergroundBeatsProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    DBG("Processor::prepareToPlay - Sample Rate: " + juce::String(sampleRate) + 
        ", Block Size: " + juce::String(samplesPerBlock));

    const int numStems = separatedStemBuffers.size();
    const int numOutputChannels = getTotalNumOutputChannels();

    // Prepare DSP chains for each stem
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = 2; // Always prepare for stereo processing regardless of input channels

    // Resize the vector of unique_ptrs first
    stemEffectChains.resize(numStems);
    for (int i = 0; i < numStems; ++i)
    {
        // If the pointer at this index is null, create a new chain
        if (stemEffectChains[i] == nullptr)
        {
            stemEffectChains[i] = std::make_unique<StemEffectChain>();
            DBG("  Created new effect chain for stem " + juce::String(i));
        }
        
        try {
            // Initialize delay line - must be done before prepare
            auto& delay = stemEffectChains[i]->get<5>();
            delay.reset();
            delay.setMaximumDelayInSamples(sampleRate * 2.0); // 2 seconds max delay
            
            // Prepare the chain (dereference the pointer)
            stemEffectChains[i]->prepare(spec);
            
            // Initialize default EQ parameters - flat response curve
            for (int band = 0; band < 3; ++band) {
                auto coefficients = juce::dsp::IIR::Coefficients<float>::makePeakFilter(
                    sampleRate, 
                    band == 0 ? 100.0f : (band == 1 ? 1000.0f : 5000.0f), // Default frequencies
                    1.0f, // Default Q
                    1.0f  // Default gain (0dB)
                );
                
                switch (band) {
                    case 0: stemEffectChains[i]->get<0>().coefficients = coefficients; break;
                    case 1: stemEffectChains[i]->get<1>().coefficients = coefficients; break;
                    case 2: stemEffectChains[i]->get<2>().coefficients = coefficients; break;
                }
            }
            
            // Initialize compressor
            auto& comp = stemEffectChains[i]->get<3>();
            comp.setThreshold(-24.0f);
            comp.setRatio(4.0f);
            comp.setAttack(10.0f);
            comp.setRelease(100.0f);
            
            // Initialize chorus
            auto& chorus = stemEffectChains[i]->get<6>();
            chorus.setRate(1.0f);
            chorus.setDepth(0.25f);
            chorus.setCentreDelay(7.0f);
            chorus.setFeedback(0.0f);
            chorus.setMix(0.5f);
            
            // Initialize saturation
            auto& saturator = stemEffectChains[i]->get<7>();
            saturator.functionToUse = [](float x) { return std::tanh(x); }; // Default saturation
            
            // Initialize final gain
            auto& finalGain = stemEffectChains[i]->get<8>();
            finalGain.setGainLinear(1.0f);
            
            // Reset the effects in the chain
            stemEffectChains[i]->reset();
            
            DBG("  Successfully prepared effect chain for stem " + juce::String(i));
        }
        catch (const std::exception& e) {
            DBG("  ERROR preparing effect chain for stem " + juce::String(i) + ": " + juce::String(e.what()));
        }
    }
    
    // Reset playback state
    playbackPosition = 0;
    
    DBG("Processor::prepareToPlay - Successfully prepared for " + juce::String(numStems) + " stems");
}

void UndergroundBeatsProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
    std::cout << "Release resources called." << std::endl;
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool UndergroundBeatsProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void UndergroundBeatsProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ignoreUnused(midiMessages);
    juce::ScopedNoDenormals noDenormals;

    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // Clear any input channels that didn't contain input data
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // Always clear the output buffer at the start
    buffer.clear();

    // Get the number of available stems
    const int numStems = separatedStemBuffers.size();
    
    // If we have no stems, just return (nothing to play)
    if (numStems == 0) {
        DBG("Processor::processBlock - No stems available to play");
        return;
    }

    // Debug output - current playback state
    DBG("Processor::processBlock - Playing: " + juce::String(playing.load() ? "Yes" : "No") + 
        ", Paused: " + juce::String(paused.load() ? "Yes" : "No") +
        ", Position: " + juce::String(playbackPosition));

    // Only process audio if we're in playing state and not paused
    if (playing && !paused && numStems > 0)
    {
        // Get buffer parameters
        int numSamples = buffer.getNumSamples();
        int outputChannels = buffer.getNumChannels();

        // First, check if any stem has solo enabled
        bool anySoloActive = false;
        for (int stemIdx = 0; stemIdx < numStems; ++stemIdx) {
            auto soloParamID = getStemParameterID(stemIdx, "Solo");
            auto* soloParam = valueTreeState.getRawParameterValue(soloParamID);
            if (soloParam && soloParam->load() > 0.5f) {
                anySoloActive = true;
                break;
            }
        }

        // Process each stem and add it to the output buffer
        for (int stemIdx = 0; stemIdx < numStems; ++stemIdx)
        {
            DBG("  Processing Stem Index: " + juce::String(stemIdx));

            // Skip if effect chain not initialized
            if (stemIdx >= stemEffectChains.size() || stemEffectChains[stemIdx] == nullptr) {
                DBG("    Stem chain is null, skipping.");
                continue;
            }
            
            // Skip if stem buffer not available
            if (stemIdx >= separatedStemBuffers.size() || separatedStemBuffers[stemIdx].getNumSamples() == 0) {
                DBG("    Stem buffer invalid or empty, skipping.");
                continue;
            }
            
            // Check mute status
            auto muteParamID = getStemParameterID(stemIdx, "Mute");
            auto* muteParam = valueTreeState.getRawParameterValue(muteParamID);
            bool isMuted = muteParam && muteParam->load() > 0.5f;
            
            // Check solo status
            auto soloParamID = getStemParameterID(stemIdx, "Solo");
            auto* soloParam = valueTreeState.getRawParameterValue(soloParamID);
            bool isSoloed = soloParam && soloParam->load() > 0.5f;
            
            // Skip if muted or if any solo is active but this stem is not soloed
            if (isMuted || (anySoloActive && !isSoloed)) {
                DBG("    Stem is muted or not soloed, skipping.");
                continue;
            }

            // Get stem buffer
            const auto& stemBuffer = separatedStemBuffers[stemIdx];
            int stemChannels = stemBuffer.getNumChannels();
            juce::int64 stemLength = stemBuffer.getNumSamples();

            DBG("    Stem Length: " + juce::String(stemLength) + ", Current Playback Pos: " + juce::String(playbackPosition));

            // Calculate samples available from current position
            juce::int64 samplesAvailable = stemLength - playbackPosition;
            if (samplesAvailable <= 0) {
                DBG("    No samples available at current position, skipping stem.");
                continue;
            }

            // Determine how many samples to process in this block
            int samplesToProcess = (int) juce::jmin((juce::int64)numSamples, samplesAvailable);
            DBG("    Samples to process for this block: " + juce::String(samplesToProcess));

            // Create a temporary buffer for processing this stem's block
            // IMPORTANT: Ensure tempBuffer matches the number of channels expected by the chain (stereo)
            int chainChannels = 2; // Assuming chains are prepared for stereo
            juce::AudioBuffer<float> tempBuffer(chainChannels, samplesToProcess);

            // Copy (and potentially up-mix mono to stereo) from stemBuffer to tempBuffer
            for (int ch = 0; ch < chainChannels; ++ch) {
                // If stem is mono, copy channel 0 to both L/R of temp buffer
                // If stem is stereo, copy L->L, R->R
                int sourceChannel = juce::jmin(ch, stemChannels - 1); 
                tempBuffer.copyFrom(ch, 0, stemBuffer, sourceChannel, (int)playbackPosition, samplesToProcess);
            }
            DBG("    Copied stem data to temp buffer (handling mono->stereo).");

            // === Update DSP parameters ===
            auto& chain = stemEffectChains[stemIdx];

            // Update EQ params...
            for (int band = 1; band <= 3; ++band)
            {
                auto eqEnableParamId = getStemParameterID(stemIdx, "EQ" + juce::String(band) + "_Enable");
                auto eqFreqParamId = getStemParameterID(stemIdx, "EQ" + juce::String(band) + "_Freq");
                auto eqGainParamId = getStemParameterID(stemIdx, "EQ" + juce::String(band) + "_Gain");
                auto eqQParamId = getStemParameterID(stemIdx, "EQ" + juce::String(band) + "_Q");

                auto* enableParam = valueTreeState.getRawParameterValue(eqEnableParamId);
                auto* freqParam = valueTreeState.getRawParameterValue(eqFreqParamId);
                auto* gainParam = valueTreeState.getRawParameterValue(eqGainParamId);
                auto* qParam = valueTreeState.getRawParameterValue(eqQParamId);
                
                bool enable = enableParam ? enableParam->load() > 0.5f : true;
                float freq = freqParam ? freqParam->load() : (band == 1 ? 100.0f : (band == 2 ? 1000.0f : 5000.0f));
                float gainDb = gainParam ? gainParam->load() : 0.0f;
                float q = qParam ? qParam->load() : 1.0f;

                auto coefficients = juce::dsp::IIR::Coefficients<float>::makePeakFilter(getSampleRate(), freq, q, juce::Decibels::decibelsToGain(gainDb));
                
                switch (band)
                {
                    case 1: chain->get<0>().coefficients = coefficients; chain->setBypassed<0>(!enable); break;
                    case 2: chain->get<1>().coefficients = coefficients; chain->setBypassed<1>(!enable); break;
                    case 3: chain->get<2>().coefficients = coefficients; chain->setBypassed<2>(!enable); break;
                }
            }
            
            // Update Compressor params...
            auto* compEnableParam = valueTreeState.getRawParameterValue(getStemParameterID(stemIdx, "Comp_Enable"));
            auto* compThreshParam = valueTreeState.getRawParameterValue(getStemParameterID(stemIdx, "Comp_Threshold"));
            auto* compRatioParam = valueTreeState.getRawParameterValue(getStemParameterID(stemIdx, "Comp_Ratio"));
            auto* compAttackParam = valueTreeState.getRawParameterValue(getStemParameterID(stemIdx, "Comp_Attack"));
            auto* compReleaseParam = valueTreeState.getRawParameterValue(getStemParameterID(stemIdx, "Comp_Release"));
            bool compEnable = compEnableParam ? compEnableParam->load() > 0.5f : true;
            auto& comp = chain->get<3>();
            if(compThreshParam) comp.setThreshold(compThreshParam->load());
            if(compRatioParam) comp.setRatio(compRatioParam->load());
            if(compAttackParam) comp.setAttack(compAttackParam->load());
            if(compReleaseParam) comp.setRelease(compReleaseParam->load());
            chain->setBypassed<3>(!compEnable);
            
            // Update Reverb params...
            auto* reverbEnableParam = valueTreeState.getRawParameterValue(getStemParameterID(stemIdx, "Reverb_Enable"));
            auto* reverbRoomParam = valueTreeState.getRawParameterValue(getStemParameterID(stemIdx, "Reverb_RoomSize"));
            auto* reverbDampParam = valueTreeState.getRawParameterValue(getStemParameterID(stemIdx, "Reverb_Damping"));
            auto* reverbWetParam = valueTreeState.getRawParameterValue(getStemParameterID(stemIdx, "Reverb_WetLevel"));
            auto* reverbDryParam = valueTreeState.getRawParameterValue(getStemParameterID(stemIdx, "Reverb_DryLevel"));
            auto* reverbWidthParam = valueTreeState.getRawParameterValue(getStemParameterID(stemIdx, "Reverb_Width"));
            auto* reverbFreezeParam = valueTreeState.getRawParameterValue(getStemParameterID(stemIdx, "Reverb_Freeze"));
            bool reverbEnable = reverbEnableParam ? reverbEnableParam->load() > 0.5f : false;
            auto& reverb = chain->get<4>();
            juce::dsp::Reverb::Parameters reverbParams;
            if(reverbRoomParam) reverbParams.roomSize = reverbRoomParam->load(); else reverbParams.roomSize = 0.5f;
            if(reverbDampParam) reverbParams.damping = reverbDampParam->load(); else reverbParams.damping = 0.5f;
            if(reverbWetParam) reverbParams.wetLevel = reverbWetParam->load(); else reverbParams.wetLevel = 0.33f;
            if(reverbDryParam) reverbParams.dryLevel = reverbDryParam->load(); else reverbParams.dryLevel = 0.4f;
            if(reverbWidthParam) reverbParams.width = reverbWidthParam->load(); else reverbParams.width = 1.0f;
            if(reverbFreezeParam) reverbParams.freezeMode = reverbFreezeParam->load() > 0.5f ? 1.0f : 0.0f; else reverbParams.freezeMode = 0.0f;
            reverb.setParameters(reverbParams);
            chain->setBypassed<4>(!reverbEnable);
            
            // Update Delay params...
            // Delay parameters implementation skipped to avoid compatibility issues

            // Update Chorus params...
            auto* chorusEnableParam = valueTreeState.getRawParameterValue(getStemParameterID(stemIdx, "Chorus_Enable"));
            auto* chorusRateParam = valueTreeState.getRawParameterValue(getStemParameterID(stemIdx, "Chorus_Rate"));
            auto* chorusDepthParam = valueTreeState.getRawParameterValue(getStemParameterID(stemIdx, "Chorus_Depth"));
            auto* chorusCentreDelayParam = valueTreeState.getRawParameterValue(getStemParameterID(stemIdx, "Chorus_CentreDelay"));
            auto* chorusFeedbackParam = valueTreeState.getRawParameterValue(getStemParameterID(stemIdx, "Chorus_Feedback"));
            auto* chorusMixParam = valueTreeState.getRawParameterValue(getStemParameterID(stemIdx, "Chorus_Mix"));
            bool chorusEnable = chorusEnableParam ? chorusEnableParam->load() > 0.5f : false;
            auto& chorus = chain->get<6>();
            if(chorusRateParam) chorus.setRate(chorusRateParam->load());
            if(chorusDepthParam) chorus.setDepth(chorusDepthParam->load());
            if(chorusCentreDelayParam) chorus.setCentreDelay(chorusCentreDelayParam->load());
            if(chorusFeedbackParam) chorus.setFeedback(chorusFeedbackParam->load());
            if(chorusMixParam) chorus.setMix(chorusMixParam->load());
            chain->setBypassed<6>(!chorusEnable);
            
            // Update Saturation params...
            auto* satEnableParam = valueTreeState.getRawParameterValue(getStemParameterID(stemIdx, "Saturation_Enable"));
            auto* satAmountParam = valueTreeState.getRawParameterValue(getStemParameterID(stemIdx, "Saturation_Amount"));
            bool satEnable = satEnableParam ? satEnableParam->load() > 0.5f : false;
            auto& saturator = chain->get<7>();
            float satAmount = satAmountParam ? satAmountParam->load() : 1.0f; // Default amount
            saturator.functionToUse = [satAmount](float x) { return std::tanh(satAmount * x); };
            chain->setBypassed<7>(!satEnable);

            // Calculate Final Gain
            auto* volumeParam = valueTreeState.getRawParameterValue(getStemParameterID(stemIdx, "Volume"));
            auto* gainDbParam = valueTreeState.getRawParameterValue(getStemParameterID(stemIdx, "Gain"));
            float volume = volumeParam ? volumeParam->load() : 0.8f;
            float gainDb = gainDbParam ? gainDbParam->load() : 0.0f;
            float linearGain = volume * juce::Decibels::decibelsToGain(gainDb);
            
            DBG("    Stem " + juce::String(stemIdx) + " Volume: " + juce::String(volume) + 
                ", Gain(dB): " + juce::String(gainDb) + 
                ", Linear Gain: " + juce::String(linearGain));

            // Process the temporary buffer through the effect chain
            juce::dsp::AudioBlock<float> block(tempBuffer);
            juce::dsp::ProcessContextReplacing<float> context(block);
            DBG("    Processing audio block through effect chain...");
            chain->process(context);
            DBG("    Processing complete.");

            // Add the processed temporary buffer to the main output buffer
            DBG("    Adding processed stem to main output buffer...");
            for (int ch = 0; ch < outputChannels; ++ch)
            {
                // Ensure we read from the correct channel of the potentially stereo tempBuffer
                int sourceChannel = juce::jmin(ch, chainChannels - 1);
                buffer.addFrom(ch, 0, tempBuffer, sourceChannel, 0, samplesToProcess, linearGain);
            }
            DBG("    Stem added to output (gain: " + juce::String(linearGain) + ")");
        }

        // Update global playback position
        juce::int64 minLength = -1;
        for (int stemIdx = 0; stemIdx < numStems; ++stemIdx) {
            if (stemIdx < separatedStemBuffers.size()) {
                auto len = separatedStemBuffers[stemIdx].getNumSamples();
                if (minLength == -1 || len < minLength)
                    minLength = len;
            }
        }

        if (minLength > 0) {
            playbackPosition += numSamples;
            if (playbackPosition >= minLength) {
                DBG("  Playback position wrapped around. Resetting to beginning.");
                playbackPosition = 0; // Wrap around (loop)
            }
        } else {
            playbackPosition = 0; // Reset if no valid stems
        }
        
        DBG("  Updated Playback Position: " + juce::String(playbackPosition) + "/" + juce::String(minLength));
    }
}

//==============================================================================
bool UndergroundBeatsProcessor::hasEditor() const
{
    return true; // Provides an editor GUI
}

juce::AudioProcessorEditor* undergroundBeats::UndergroundBeatsProcessor::createEditor()
{
    return dynamic_cast<juce::AudioProcessorEditor*>(new undergroundBeats::MainEditor(*this));
}

//==============================================================================
void UndergroundBeatsProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    // juce::ignoreUnused (destData); // Original line
    // Save the value tree state (NEW)
    auto state = valueTreeState.copyState();
    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    copyXmlToBinary (*xml, destData);
}

void UndergroundBeatsProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    // juce::ignoreUnused (data, sizeInBytes); // Original line
    // Restore the value tree state (NEW)
    std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));

    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName (valueTreeState.state.getType()))
            valueTreeState.replaceState (juce::ValueTree::fromXml (*xmlState));

    parametersChanged = true; // Ensure UI updates after loading state
}

//==============================================================================
// Playback Control Method Implementations (NEW)
//==============================================================================
void UndergroundBeatsProcessor::startPlayback()
{
    // Start playback only if not already playing
    DBG("Processor: startPlayback() called."); // Log entry
    if (!playing.load()) // Use .load() for atomic read
    {
        playing = true;
        paused = false;
        playbackPosition = 0; // Reset position on play start
        DBG("Processor: Playback Started (playing=true, paused=false, position=0)");
    }
    else if (paused.load()) // If paused, resume playback
    {
        paused = false;
        DBG("Processor: Playback Resumed (paused=false)");
    }
    else
    {
        DBG("Processor: startPlayback() called but already playing and not paused.");
    }
}

void UndergroundBeatsProcessor::pausePlayback()
{
    DBG("Processor: pausePlayback() called."); // Log entry
    // Pause only if currently playing and not already paused
    if (playing.load() && !paused.load())
    {
        paused = true;
        DBG("Processor: Playback Paused (paused=true)");
    }
}

void UndergroundBeatsProcessor::stopPlayback()
{
    DBG("Processor: stopPlayback() called."); // Log entry
    // Stop playback if playing or paused
    if (playing.load() || paused.load())
    {
        playing = false;
        paused = false;
        playbackPosition = 0; // Reset position on stop
        DBG("Processor: Playback Stopped (playing=false, paused=false, position=0)");
    }
}

bool UndergroundBeatsProcessor::isPlaying() const
{
    // Return true only if playing and not paused
    return playing.load() && !paused.load();
}

bool UndergroundBeatsProcessor::isPaused() const
{
    // Return true if playing but currently paused
    return playing.load() && paused.load();
}

bool UndergroundBeatsProcessor::loadAndSwapStem(int stemIndex, const juce::File& file)
{
    if (!file.existsAsFile())
        return false;

    std::unique_ptr<juce::AudioFormatReader> reader(formatManager.createReaderFor(file));
    if (reader == nullptr)
        return false;

    int numChannels = juce::jmin(static_cast<int>(reader->numChannels), 2);
    int numSamples = static_cast<int>(reader->lengthInSamples);

    juce::AudioBuffer<float> newBuffer;
    newBuffer.setSize(numChannels, numSamples);
    reader->read(&newBuffer, 0, numSamples, 0, true, true);

    if (stemIndex >= separatedStemBuffers.size())
        separatedStemBuffers.resize(stemIndex + 1);

    separatedStemBuffers[stemIndex] = std::move(newBuffer);

    parametersChanged = true;
    return true;
}

} // namespace undergroundBeats

// This creates new instances of the plugin. Must be outside the namespace to be found by the JUCE plugin loader
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new undergroundBeats::UndergroundBeatsProcessor();
}
