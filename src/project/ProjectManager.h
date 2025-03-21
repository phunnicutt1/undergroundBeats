/*
 * Underground Beats
 * ProjectManager.h
 * 
 * Manages project state, saving, and loading
 */

#pragma once

#include <JuceHeader.h>
#include <memory>
#include <string>
#include <vector>

namespace UndergroundBeats {

// Forward declarations
class ProjectState;

/**
 * @class ProjectManager
 * @brief Manages project state, saving, and loading
 * 
 * The ProjectManager class handles creation, saving, and loading of projects.
 * It maintains the current project state and provides methods for serializing
 * and deserializing project data.
 */
class ProjectManager
{
public:
    ProjectManager();
    ~ProjectManager();
    
    /**
     * @brief Create a new empty project
     * 
     * @param name The name for the new project
     * @param sampleRate The sample rate for the new project
     * @return true if successful
     */
    bool createNewProject(const juce::String& name, double sampleRate = 44100.0);
    
    /**
     * @brief Save the current project
     * 
     * @param saveAs Whether to prompt for a new filename
     * @return true if successful
     */
    bool saveProject(bool saveAs = false);
    
    /**
     * @brief Save the current project to a specific file
     * 
     * @param file The file to save to
     * @return true if successful
     */
    bool saveProjectAs(const juce::File& file);
    
    /**
     * @brief Load a project from a file
     * 
     * @param file The file to load from
     * @return true if successful
     */
    bool loadProject(const juce::File& file);
    
    /**
     * @brief Get the current project state
     * 
     * @return A reference to the current project state
     */
    ProjectState& getProjectState();
    
    /**
     * @brief Check if the current project has been modified
     * 
     * @return true if the project has unsaved changes
     */
    bool hasUnsavedChanges() const;
    
    /**
     * @brief Get the current project file
     * 
     * @return The current project file, or an empty File if the project is unsaved
     */
    juce::File getProjectFile() const;
    
    /**
     * @brief Get the name of the current project
     * 
     * @return The project name
     */
    juce::String getProjectName() const;
    
    /**
     * @brief Register a listener to be notified of project changes
     * 
     * @param listener The listener to register
     */
    void addListener(juce::ChangeListener* listener);
    
    /**
     * @brief Unregister a previously registered listener
     * 
     * @param listener The listener to unregister
     */
    void removeListener(juce::ChangeListener* listener);
    
private:
    std::unique_ptr<ProjectState> projectState;
    juce::File projectFile;
    juce::String projectName;
    bool unsavedChanges;
    
    juce::ChangeBroadcaster changeNotifier;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ProjectManager)
};

} // namespace UndergroundBeats
