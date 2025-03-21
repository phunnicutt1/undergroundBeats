/*
 * Underground Beats
 * ProjectState.cpp
 */

#include "ProjectState.h"

namespace UndergroundBeats {

ProjectState::ProjectState()
    : projectData(std::make_unique<juce::XmlElement>("UndergroundBeatsProject")),
      modified(false)
{
    // Initialize settings with defaults
    settings.name = "Untitled";
    settings.sampleRate = 44100.0;
    settings.bitsPerSample = 24;
    settings.tempo = 120;
    settings.timeSignatureNumerator = 4;
    settings.timeSignatureDenominator = 4;
    settings.snapToGrid = true;
    settings.snapDivision = 16;
}

ProjectState::~ProjectState()
{
    // Nothing to clean up
}

void ProjectState::initializeDefault(const juce::String& name, double sampleRate)
{
    // Set up new project data
    projectData = std::make_unique<juce::XmlElement>("UndergroundBeatsProject");
    
    // Add version attribute
    projectData->setAttribute("version", "1.0");
    
    // Update settings
    settings.name = name;
    settings.sampleRate = sampleRate;
    
    // Add settings to XML
    auto* settingsXml = projectData->createNewChildElement("Settings");
    settingsXml->setAttribute("name", settings.name);
    settingsXml->setAttribute("sampleRate", settings.sampleRate);
    settingsXml->setAttribute("bitsPerSample", settings.bitsPerSample);
    settingsXml->setAttribute("tempo", settings.tempo);
    settingsXml->setAttribute("timeSignatureNumerator", settings.timeSignatureNumerator);
    settingsXml->setAttribute("timeSignatureDenominator", settings.timeSignatureDenominator);
    settingsXml->setAttribute("snapToGrid", settings.snapToGrid);
    settingsXml->setAttribute("snapDivision", settings.snapDivision);
    
    // Add empty sections for tracks, patterns, etc.
    projectData->createNewChildElement("Tracks");
    projectData->createNewChildElement("Patterns");
    projectData->createNewChildElement("Instruments");
    projectData->createNewChildElement("Effects");
    
    // Mark as unmodified
    modified = false;
}

ProjectSettings& ProjectState::getSettings()
{
    return settings;
}

void ProjectState::setSettings(const ProjectSettings& newSettings)
{
    settings = newSettings;
    
    // Update settings in XML
    if (auto* settingsXml = projectData->getChildByName("Settings"))
    {
        settingsXml->setAttribute("name", settings.name);
        settingsXml->setAttribute("sampleRate", settings.sampleRate);
        settingsXml->setAttribute("bitsPerSample", settings.bitsPerSample);
        settingsXml->setAttribute("tempo", settings.tempo);
        settingsXml->setAttribute("timeSignatureNumerator", settings.timeSignatureNumerator);
        settingsXml->setAttribute("timeSignatureDenominator", settings.timeSignatureDenominator);
        settingsXml->setAttribute("snapToGrid", settings.snapToGrid);
        settingsXml->setAttribute("snapDivision", settings.snapDivision);
    }
    
    // Mark as modified
    modified = true;
}

std::unique_ptr<juce::XmlElement> ProjectState::createXml() const
{
    // Create a copy of the project data
    return std::make_unique<juce::XmlElement>(*projectData);
}

bool ProjectState::restoreFromXml(const juce::XmlElement* xml)
{
    // Check if this is a valid project file
    if (xml == nullptr || xml->getTagName() != "UndergroundBeatsProject")
    {
        return false;
    }
    
    // Copy the XML data
    projectData = std::make_unique<juce::XmlElement>(*xml);
    
    // Extract settings
    if (auto* settingsXml = projectData->getChildByName("Settings"))
    {
        settings.name = settingsXml->getStringAttribute("name", "Untitled");
        settings.sampleRate = settingsXml->getDoubleAttribute("sampleRate", 44100.0);
        settings.bitsPerSample = settingsXml->getIntAttribute("bitsPerSample", 24);
        settings.tempo = settingsXml->getIntAttribute("tempo", 120);
        settings.timeSignatureNumerator = settingsXml->getIntAttribute("timeSignatureNumerator", 4);
        settings.timeSignatureDenominator = settingsXml->getIntAttribute("timeSignatureDenominator", 4);
        settings.snapToGrid = settingsXml->getBoolAttribute("snapToGrid", true);
        settings.snapDivision = settingsXml->getIntAttribute("snapDivision", 16);
    }
    
    // Ensure required sections exist
    if (!projectData->getChildByName("Tracks"))
    {
        projectData->createNewChildElement("Tracks");
    }
    
    if (!projectData->getChildByName("Patterns"))
    {
        projectData->createNewChildElement("Patterns");
    }
    
    if (!projectData->getChildByName("Instruments"))
    {
        projectData->createNewChildElement("Instruments");
    }
    
    if (!projectData->getChildByName("Effects"))
    {
        projectData->createNewChildElement("Effects");
    }
    
    // Mark as unmodified
    modified = false;
    
    return true;
}

void ProjectState::setModified(bool isModified)
{
    modified = isModified;
}

bool ProjectState::isModified() const
{
    return modified;
}

bool ProjectState::addPattern(const juce::XmlElement* patternXml)
{
    if (patternXml == nullptr || patternXml->getTagName() != "Pattern")
    {
        return false;
    }
    
    // Get the patterns section
    if (auto* patternsXml = projectData->getChildByName("Patterns"))
    {
        // Add a copy of the pattern
        patternsXml->addChildElement(new juce::XmlElement(*patternXml));
        
        // Mark as modified
        modified = true;
        
        return true;
    }
    
    return false;
}

juce::Array<juce::XmlElement*> ProjectState::getPatterns() const
{
    juce::Array<juce::XmlElement*> patterns;
    
    if (auto* patternsXml = projectData->getChildByName("Patterns"))
    {
        for (auto* pattern : patternsXml->getChildIterator())
        {
            if (pattern->getTagName() == "Pattern")
            {
                patterns.add(pattern);
            }
        }
    }
    
    return patterns;
}

bool ProjectState::addTrack(const juce::XmlElement* trackXml)
{
    if (trackXml == nullptr || trackXml->getTagName() != "Track")
    {
        return false;
    }
    
    // Get the tracks section
    if (auto* tracksXml = projectData->getChildByName("Tracks"))
    {
        // Add a copy of the track
        tracksXml->addChildElement(new juce::XmlElement(*trackXml));
        
        // Mark as modified
        modified = true;
        
        return true;
    }
    
    return false;
}

juce::Array<juce::XmlElement*> ProjectState::getTracks() const
{
    juce::Array<juce::XmlElement*> tracks;
    
    if (auto* tracksXml = projectData->getChildByName("Tracks"))
    {
        for (auto* track : tracksXml->getChildIterator())
        {
            if (track->getTagName() == "Track")
            {
                tracks.add(track);
            }
        }
    }
    
    return tracks;
}

} // namespace UndergroundBeats
