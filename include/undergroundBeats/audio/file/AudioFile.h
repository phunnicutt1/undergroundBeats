#pragma once

#include <juce_audio_formats/juce_audio_formats.h>
#include <memory>
#include <vector>
#include <string>

namespace undergroundBeats {
namespace audio {
namespace file {

/**
 * @class AudioFile
 * @brief Class for loading and managing audio samples and files.
 * 
 * This class handles loading, basic processing, and playback of audio files.
 * It supports various formats through JUCE's AudioFormatManager.
 */
class AudioFile
{
public:
    /**
     * @brief Constructor for the AudioFile class.
     */
    AudioFile();
    
    /**
     * @brief Destructor for the AudioFile class.
     */
    ~AudioFile();
    
    /**
     * @brief Load an audio file from a specified path.
     * @param filePath Path to the audio file.
     * @return True if the file was loaded successfully, false otherwise.
     */
    bool loadFromFile(const juce::String& filePath);
    
    /**
     * @brief Load audio data from a memory block.
     * @param data Memory block containing the audio data.
     * @param dataSize Size of the memory block in bytes.
     * @param format Format of the audio data (e.g., "wav", "mp3"). If empty, format will be detected automatically.
     * @return True if the data was loaded successfully, false otherwise.
     */
    bool loadFromMemory(const void* data, size_t dataSize, const juce::String& format = juce::String());
    
    /**
     * @brief Check if an audio file is currently loaded.
     * @return True if an audio file is loaded, false otherwise.
     */
    bool isLoaded() const;
    
    /**
     * @brief Get the number of channels in the loaded audio file.
     * @return Number of channels, or 0 if no file is loaded.
     */
    int getNumChannels() const;
    
    /**
     * @brief Get the sample rate of the loaded audio file.
     * @return Sample rate in Hz, or 0 if no file is loaded.
     */
    double getSampleRate() const;
    
    /**
     * @brief Get the length of the loaded audio file in samples.
     * @return Length in samples, or 0 if no file is loaded.
     */
    int64_t getLengthInSamples() const;
    
    /**
     * @brief Get the length of the loaded audio file in seconds.
     * @return Length in seconds, or 0 if no file is loaded.
     */
    double getLengthInSeconds() const;
    
    /**
     * @brief Get the file name of the loaded audio file.
     * @return File name, or an empty string if no file is loaded.
     */
    juce::String getFileName() const;
    
    /**
     * @brief Get the full file path of the loaded audio file.
     * @return File path, or an empty string if no file is loaded.
     */
    juce::String getFilePath() const;
    
    /**
     * @brief Get a read-only reference to the audio buffer.
     * @return Reference to the audio buffer.
     */
    const juce::AudioBuffer<float>& getAudioBuffer() const;
    
    /**
     * @brief Clear the currently loaded audio file and free memory.
     */
    void clear();
    
private:
    juce::AudioFormatManager formatManager;
    std::unique_ptr<juce::AudioFormatReader> audioReader;
    juce::AudioBuffer<float> audioBuffer;
    juce::String fileName;
    juce::String filePath;
    bool loaded;
    
    // Initialize the format manager with standard formats
    void initializeFormatManager();
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioFile)
};

} // namespace file
} // namespace audio
} // namespace undergroundBeats