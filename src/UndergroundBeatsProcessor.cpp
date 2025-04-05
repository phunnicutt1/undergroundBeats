#include "../include/undergroundBeats/UndergroundBeatsProcessor.h"
#include "../include/undergroundBeats/gui/MainEditor.h"
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
    
    // In a real application, we'd now call methods to separate the audio into stems.
    // For now, we'll just signal success.

    // --- Placeholder: Populate separatedStemBuffers for UI testing --- (NEW)
    separatedStemBuffers.clear(); // Clear previous stems
    int numPlaceholderStems = 4; // Example: Simulate 4 stems
    for (int i = 0; i < numPlaceholderStems; ++i)
    {
        // Create a copy of the original buffer for each "stem"
        // Make sure audioBuffer is valid before copying
        if (audioBuffer.getNumSamples() > 0 && audioBuffer.getNumChannels() > 0)
        {
            separatedStemBuffers.emplace_back(audioBuffer);
            // Optionally modify each buffer slightly to make them look different
            // e.g., apply gain, invert phase, etc. For now, just copies.
        }
        else
        {
             // Handle case where audioBuffer is empty or invalid
             separatedStemBuffers.emplace_back(); // Add an empty buffer
             std::cerr << "Warning: audioBuffer is empty or invalid when creating placeholder stem " << i << std::endl;
        }
    }
    std::cout << "Placeholder: Created " << separatedStemBuffers.size() << " stem buffers." << std::endl;
    // --- End Placeholder ---

    parametersChanged = true; // Signal UI that parameters might need refreshing (NEW)
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
    std::cout << "Prepare to play called. Sample Rate: " << sampleRate << ", Block Size: " << samplesPerBlock << std::endl;

    // Initialize DSP chains for each stem
    const int numStems = static_cast<int>(separatedStemBuffers.size());
    stemEffectChains.resize(numStems);

    for (auto& chain : stemEffectChains)
    {
        chain.prepare({ sampleRate, static_cast<juce::uint32>(samplesPerBlock), 2 });
    }
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

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    buffer.clear();

    const int numStems = static_cast<int>(separatedStemBuffers.size());

    static juce::int64 playbackPosition = 0;
    int numSamples = buffer.getNumSamples();
    int outputChannels = buffer.getNumChannels();

    if (playing && !paused && numStems > 0)
    {
        buffer.clear();

        for (int stemIdx = 0; stemIdx < numStems; ++stemIdx)
        {
            if (stemIdx >= separatedStemBuffers.size() || separatedStemBuffers[stemIdx].getNumSamples() == 0)
                continue;

            const auto& stemBuffer = separatedStemBuffers[stemIdx];
            int stemChannels = stemBuffer.getNumChannels();
            juce::int64 stemLength = stemBuffer.getNumSamples();

            juce::int64 samplesAvailable = stemLength - playbackPosition;
            if (samplesAvailable <= 0) continue;

            int samplesToProcess = (int) juce::jmin((juce::int64)numSamples, samplesAvailable);

            juce::AudioBuffer<float> tempBuffer(stemChannels, samplesToProcess);
            for (int ch = 0; ch < stemChannels; ++ch)
                tempBuffer.copyFrom(ch, 0, stemBuffer, ch, (int)playbackPosition, samplesToProcess);

            // === Update DSP parameters ===
            auto& chain = stemEffectChains[stemIdx];

            for (int band = 1; band <= 3; ++band)
            {
                auto enable = valueTreeState.getRawParameterValue(getStemParameterID(stemIdx, "EQ" + juce::String(band) + "_Enable"))->load() > 0.5f;
                auto freq = valueTreeState.getRawParameterValue(getStemParameterID(stemIdx, "EQ" + juce::String(band) + "_Freq"))->load();
                auto gainDb = valueTreeState.getRawParameterValue(getStemParameterID(stemIdx, "EQ" + juce::String(band) + "_Gain"))->load();
                auto q = valueTreeState.getRawParameterValue(getStemParameterID(stemIdx, "EQ" + juce::String(band) + "_Q"))->load();

                // Create coefficients
                auto coefficients = juce::dsp::IIR::Coefficients<float>::makePeakFilter(
                    getSampleRate(), freq, q, juce::Decibels::decibelsToGain(gainDb));

                // Use switch statement to handle compile-time indices
                switch (band)
                {
                    case 1:
                    {
                        auto& filter = chain.get<0>();
                        filter.coefficients = coefficients; // This way we can access the coefficients
                        chain.setBypassed<0>(!enable);
                        break;
                    }
                    case 2:
                    {
                        auto& filter = chain.get<1>();
                        filter.coefficients = coefficients;
                        chain.setBypassed<1>(!enable);
                        break;
                    }
                    case 3:
                    {
                        auto& filter = chain.get<2>();
                        filter.coefficients = coefficients;
                        chain.setBypassed<2>(!enable);
                        break;
                    }
                }
            }

            auto compEnable = valueTreeState.getRawParameterValue(getStemParameterID(stemIdx, "Comp_Enable"))->load() > 0.5f;
            auto threshold = valueTreeState.getRawParameterValue(getStemParameterID(stemIdx, "Comp_Threshold"))->load();
            auto ratio = valueTreeState.getRawParameterValue(getStemParameterID(stemIdx, "Comp_Ratio"))->load();
            auto attack = valueTreeState.getRawParameterValue(getStemParameterID(stemIdx, "Comp_Attack"))->load();
            auto release = valueTreeState.getRawParameterValue(getStemParameterID(stemIdx, "Comp_Release"))->load();

            auto& comp = chain.get<3>();
            comp.setThreshold(threshold);
            comp.setRatio(ratio);
            comp.setAttack(attack);
            comp.setRelease(release);
            chain.setBypassed<3>(!compEnable);

            auto styleEnable = valueTreeState.getRawParameterValue(getStemParameterID(stemIdx, "Style_Enable"))->load() > 0.5f;
            auto& styleGain = chain.get<4>();
            styleGain.setGainLinear(1.0f); // Placeholder gain
            chain.setBypassed<4>(!styleEnable);

            auto volume = valueTreeState.getRawParameterValue(getStemParameterID(stemIdx, "Volume"))->load();
            auto gainDb = valueTreeState.getRawParameterValue(getStemParameterID(stemIdx, "Gain"))->load();
            float linearGain = volume * juce::Decibels::decibelsToGain(gainDb);

            juce::dsp::AudioBlock<float> block(tempBuffer);
            juce::dsp::ProcessContextReplacing<float> context(block);
            chain.process(context);

            for (int ch = 0; ch < outputChannels; ++ch)
            {
                int srcCh = juce::jmin(ch, stemChannels - 1);
                buffer.addFrom(ch, 0, tempBuffer, srcCh, 0, samplesToProcess, linearGain);
            }
        }

        // Update playback position
        juce::int64 minLength = -1;
        for (int stemIdx = 0; stemIdx < numStems; ++stemIdx)
        {
            if (stemIdx < separatedStemBuffers.size())
            {
                auto len = separatedStemBuffers[stemIdx].getNumSamples();
                if (minLength == -1 || len < minLength)
                    minLength = len;
            }
        }
        if (minLength > 0)
        {
            playbackPosition += numSamples;
            if (playbackPosition >= minLength)
                playbackPosition = 0;
        }
        else
        {
            playbackPosition = 0;
        }
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
    if (!playing.load()) // Use .load() for atomic read
    {
        playing = true;
        paused = false;
        // Reset position or start from current position depending on desired behavior
        // currentSamplePosition = 0; // Example: Reset position on play
        std::cout << "Processor: Playback Started" << std::endl;
        // TODO: Add logic here to actually start reading/generating audio samples
    }
    else if (paused.load()) // If paused, resume playback
    {
        paused = false;
        std::cout << "Processor: Playback Resumed" << std::endl;
        // TODO: Resume audio processing from current position
    }
}

void UndergroundBeatsProcessor::pausePlayback()
{
    // Pause only if currently playing and not already paused
    if (playing.load() && !paused.load())
    {
        paused = true;
        std::cout << "Processor: Playback Paused" << std::endl;
        // TODO: Add logic to halt audio processing but maintain position
    }
}

void UndergroundBeatsProcessor::stopPlayback()
{
    // Stop playback if playing or paused
    if (playing.load() || paused.load())
    {
        playing = false;
        paused = false;
        // Reset playback position
        // currentSamplePosition = 0; // Example: Reset position on stop
        std::cout << "Processor: Playback Stopped" << std::endl;
        // TODO: Add logic to fully stop audio processing and reset state
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
