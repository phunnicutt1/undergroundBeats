/*
 * Underground Beats
 * ProjectState.h
 * 
 * Stores the complete state of a project
 */

#pragma once

#include <JuceHeader.h>
#include <memory>
#include <vector>
#include <string>

namespace UndergroundBeats {

/**
 * @class ProjectSettings
 * @brief Basic settings for a project
 */
struct ProjectSettings
{
    juce::String name;
    double sampleRate = 44100.0;
    int bitsPerSample = 24;
    int tempo = 120;
    int timeSignatureNumerator = 4;
    int timeSignatureDenominator = 4;
    bool snapToGrid = true;
    int snapDivision = 16; // 16th notes
};

/**
 * @class ProjectState
 * @brief Stores the complete state of a project
 * 
 * The ProjectState class holds all data for a project, including
 * patterns, tracks, instruments, effects, and project settings.
 * It provides methods for serializing and deserializing the
 * project data to/from XML.
 */
class ProjectState
{
public:
    ProjectState();
    ~ProjectState();
    
    /**
     * @brief Initialize with default settings
     * 
     * @param name The name for the new project
     * @param sampleRate The sample rate to use
     */
    void initializeDefault(const juce::String& name, double sampleRate = 44100.0);
    
    /**
     * @brief Get the project settings
     * 
     * @return A reference to the project settings
     */
    ProjectSettings& getSettings();
    
    /**
     * @brief Set the project settings
     * 
     * @param newSettings The new settings to use
     */
    void setSettings(const ProjectSettings& newSettings);
    
    /**
     * @brief Create an XML element representing the project state
     * 
     * @return XML element containing the project state
     */
    std::unique_ptr<juce::XmlElement> createXml() const;
    
    /**
     * @brief Restore the project state from an XML element
     * 
     * @param xml XML element containing project state
     * @return true if successful
     */
    bool restoreFromXml(const juce::XmlElement* xml);
    
    /**
     * @brief Mark the project as modified or unmodified
     * 
     * @param modified Whether the project has been modified
     */
    void setModified(bool modified);
    
    /**
     * @brief Check if the project has been modified
     * 
     * @return true if the project has been modified since the last save
     */
    bool isModified() const;
    
    /**
     * @brief Add a pattern to the project
     * 
     * @param patternXml XML element containing pattern data
     * @return true if successful
     */
    bool addPattern(const juce::XmlElement* patternXml);
    
    /**
     * @brief Get all patterns in the project
     * 
     * @return Array of XML elements representing patterns
     */
    juce::Array<juce::XmlElement*> getPatterns() const;
    
    /**
     * @brief Add a track to the project
     * 
     * @param trackXml XML element containing track data
     * @return true if successful
     */
    bool addTrack(const juce::XmlElement* trackXml);
    
    /**
     * @brief Get all tracks in the project
     * 
     * @return Array of XML elements representing tracks
     */
    juce::Array<juce::XmlElement*> getTracks() const;
    
private:
    ProjectSettings settings;
    std::unique_ptr<juce::XmlElement> projectData;
    bool modified;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ProjectState)
};

} // namespace UndergroundBeats
