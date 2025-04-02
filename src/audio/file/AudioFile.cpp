#include "undergroundBeats/audio/file/AudioFile.h"

namespace undergroundBeats {
namespace audio {
namespace file {

AudioFile::AudioFile()
    : loaded(false)
{
    initializeFormatManager();
}

AudioFile::~AudioFile()
{
    clear();
}

bool AudioFile::loadFromFile(const juce::String& path)
{
    // Clear any previously loaded file
    clear();
    
    // Create a File object to handle the path
    juce::File file(path);
    
    // Check if the file exists
    if (!file.existsAsFile())
    {
        juce::Logger::writeToLog("Audio file not found: " + path);
        return false;
    }
    
    // Try to create a reader for the file
    audioReader.reset(formatManager.createReaderFor(file));
    
    if (audioReader == nullptr)
    {
        juce::Logger::writeToLog("Could not read audio file: " + path);
        return false;
    }
    
    // Set the file info
    fileName = file.getFileName();
    filePath = file.getFullPathName();
    
    // Create an audio buffer with the appropriate size
    audioBuffer.setSize(audioReader->numChannels, static_cast<int>(audioReader->lengthInSamples));
    
    // Read the entire file into the buffer
    audioReader->read(&audioBuffer, 0, static_cast<int>(audioReader->lengthInSamples), 0, true, true);
    
    loaded = true;
    return true;
}

bool AudioFile::loadFromMemory(const void* data, size_t dataSize, const juce::String& format)
{
    // Clear any previously loaded file
    clear();
    
    // Create a memory input stream from the data
    std::unique_ptr<juce::MemoryInputStream> memoryStream(new juce::MemoryInputStream(data, dataSize, false));
    
    // Create a reader based on format or auto-detect
    if (format.isNotEmpty())
    {
        // Try to create a reader for the specified format
        juce::AudioFormat* formatReader = formatManager.findFormatForFileExtension(format);
        if (formatReader != nullptr)
        {
            audioReader.reset(formatReader->createReaderFor(memoryStream.release(), true));
        }
    }
    else
    {
        // Auto-detect format
        audioReader.reset(formatManager.createReaderFor(std::move(memoryStream)));
    }
    
    if (audioReader == nullptr)
    {
        juce::Logger::writeToLog("Could not read audio data from memory");
        return false;
    }
    
    // No file name for memory-loaded data
    fileName = "Memory Buffer";
    filePath = "";
    
    // Create an audio buffer with the appropriate size
    audioBuffer.setSize(audioReader->numChannels, static_cast<int>(audioReader->lengthInSamples));
    
    // Read the entire data into the buffer
    audioReader->read(&audioBuffer, 0, static_cast<int>(audioReader->lengthInSamples), 0, true, true);
    
    loaded = true;
    return true;
}

bool AudioFile::isLoaded() const
{
    return loaded;
}

int AudioFile::getNumChannels() const
{
    return loaded ? audioBuffer.getNumChannels() : 0;
}

double AudioFile::getSampleRate() const
{
    return loaded && audioReader != nullptr ? audioReader->sampleRate : 0.0;
}

int64_t AudioFile::getLengthInSamples() const
{
    return loaded ? audioBuffer.getNumSamples() : 0;
}

double AudioFile::getLengthInSeconds() const
{
    if (loaded && audioReader != nullptr && audioReader->sampleRate > 0)
    {
        return static_cast<double>(audioBuffer.getNumSamples()) / audioReader->sampleRate;
    }
    return 0.0;
}

juce::String AudioFile::getFileName() const
{
    return fileName;
}

juce::String AudioFile::getFilePath() const
{
    return filePath;
}

const juce::AudioBuffer<float>& AudioFile::getAudioBuffer() const
{
    return audioBuffer;
}

void AudioFile::clear()
{
    audioReader.reset();
    audioBuffer.clear();
    fileName = juce::String();
    filePath = juce::String();
    loaded = false;
}

void AudioFile::initializeFormatManager()
{
    formatManager.registerBasicFormats();  // Register basic formats like wav, aiff
}

} // namespace file
} // namespace audio
} // namespace undergroundBeats