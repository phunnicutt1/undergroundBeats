#pragma once

#include "JuceHeader.h"

/**
 * A custom file filter that checks for specific audio file extensions.
 * This is needed because WildcardFileFilter seems unreliable in this context.
 */
class AudioFileFilter : public juce::FileFilter
{
public:
    /** Constructor - Restore descriptive text */
    AudioFileFilter() 
        : juce::FileFilter("Audio Files") // Restore description
    {}

    /** Checks if a file extension matches the allowed audio formats. */
    bool isFileSuitable (const juce::File& file) const override
    {
        // Use the reliable direct check with semicolons
        return file.hasFileExtension(".wav;.mp3;.aiff;.ogg;.flac");
    }

    /** Allow all directories to be shown. */
    bool isDirectorySuitable (const juce::File& /*directory*/) const override
    {
        return true;
    }
}; 