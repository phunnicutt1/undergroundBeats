/*
 * Underground Beats
 * ProjectManager.cpp
 */

#include "ProjectManager.h"
#include "ProjectState.h"

namespace UndergroundBeats {

ProjectManager::ProjectManager()
    : projectState(std::make_unique<ProjectState>()),
      unsavedChanges(false)
{
    // Initialize with an untitled project
    createNewProject("Untitled");
}

ProjectManager::~ProjectManager()
{
    // Clean up any listeners
    changeNotifier.removeAllChangeListeners();
}

bool ProjectManager::createNewProject(const juce::String& name, double sampleRate)
{
    // Check if we need to prompt to save the current project
    if (unsavedChanges)
    {
        // In a real implementation, we would prompt the user here
        // For now, we'll just assume they don't want to save
    }
    
    // Create a new project state
    projectState = std::make_unique<ProjectState>();
    projectState->initializeDefault(name, sampleRate);
    
    // Update project info
    projectName = name;
    projectFile = juce::File();
    unsavedChanges = false;
    
    // Notify listeners
    changeNotifier.sendChangeMessage();
    
    return true;
}

bool ProjectManager::saveProject(bool saveAs)
{
    // If we haven't saved before, or saveAs is true, prompt for a file
    if (projectFile == juce::File() || saveAs)
    {
        juce::FileChooser fileChooser("Save Project",
                                      juce::File::getSpecialLocation(juce::File::userDocumentsDirectory),
                                      "*.ubp");
        
        if (fileChooser.browseForFileToSave(true))
        {
            juce::File file = fileChooser.getResult();
            return saveProjectAs(file);
        }
        
        return false;
    }
    
    // Save to the existing file
    return saveProjectAs(projectFile);
}

bool ProjectManager::saveProjectAs(const juce::File& file)
{
    // Create XML from project state
    std::unique_ptr<juce::XmlElement> xml = projectState->createXml();
    
    // Save XML to file
    if (xml->writeToFile(file, ""))
    {
        projectFile = file;
        unsavedChanges = false;
        
        // Update project name from filename
        projectName = file.getFileNameWithoutExtension();
        
        // Notify listeners
        changeNotifier.sendChangeMessage();
        
        return true;
    }
    
    return false;
}

bool ProjectManager::loadProject(const juce::File& file)
{
    // Check if we need to prompt to save the current project
    if (unsavedChanges)
    {
        // In a real implementation, we would prompt the user here
        // For now, we'll just assume they don't want to save
    }
    
    // Load XML from file
    std::unique_ptr<juce::XmlElement> xml = juce::XmlDocument::parse(file);
    
    if (xml != nullptr)
    {
        // Create a new project state
        std::unique_ptr<ProjectState> newState = std::make_unique<ProjectState>();
        
        // Restore from XML
        if (newState->restoreFromXml(xml.get()))
        {
            // Update project state
            projectState = std::move(newState);
            projectFile = file;
            projectName = file.getFileNameWithoutExtension();
            unsavedChanges = false;
            
            // Notify listeners
            changeNotifier.sendChangeMessage();
            
            return true;
        }
    }
    
    return false;
}

ProjectState& ProjectManager::getProjectState()
{
    return *projectState;
}

bool ProjectManager::hasUnsavedChanges() const
{
    return unsavedChanges || projectState->isModified();
}

juce::File ProjectManager::getProjectFile() const
{
    return projectFile;
}

juce::String ProjectManager::getProjectName() const
{
    return projectName;
}

void ProjectManager::addListener(juce::ChangeListener* listener)
{
    changeNotifier.addChangeListener(listener);
}

void ProjectManager::removeListener(juce::ChangeListener* listener)
{
    changeNotifier.removeChangeListener(listener);
}

} // namespace UndergroundBeats
