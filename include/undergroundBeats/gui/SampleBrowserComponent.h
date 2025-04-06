#pragma once

#include "JuceHeader.h"
#include "AudioFileFilter.h" // Include the custom filter
#include <functional> // For std::function

// Forward declaration for the constructor argument
namespace juce { class FilePreviewComponent; }

namespace undergroundBeats {

/**
 * Displays a filtered file browser for audio samples with preview and drag support.
 */
class SampleBrowserComponent : public juce::Component,
                               public juce::FileBrowserListener,
                               private juce::Button::Listener,
                               private juce::Timer,
                               public juce::FileDragAndDropTarget,
                               public juce::ChangeBroadcaster
{
public:
    SampleBrowserComponent();
    ~SampleBrowserComponent() override;

    void resized() override;
    void paint(juce::Graphics& g) override;

    // FileBrowserListener callbacks
    void selectionChanged() override;
    void fileClicked(const juce::File&, const juce::MouseEvent&) override;
    void fileDoubleClicked(const juce::File&) override;
    void browserRootChanged(const juce::File&) override;

    // Button::Listener callback
    void buttonClicked(juce::Button* button) override;

    // FileDragAndDropTarget callbacks
    bool isInterestedInFileDrag (const juce::StringArray& files) override;
    void filesDropped (const juce::StringArray& files, int x, int y) override;
    void fileDragEnter (const juce::StringArray& files, int x, int y) override;
    void fileDragExit (const juce::StringArray& files) override;

    // Set callback to notify when a file is dragged and dropped externally
    std::function<void(const juce::File&)> onSampleDropped;

    // ** NEW ** Callback for when a file is chosen via double-click
    std::function<void(const juce::File&)> onFileChosenForProcessing;
    
    // ** NEW ** Get the currently selected file
    juce::File getSelectedFile() const { return selectedFile; }
    
    // ** NEW ** Check if a file is suitable (delegates to fileFilter)
    bool isFileSuitable(const juce::File& file) const { return fileFilter.isFileSuitable(file); }

    // Start previewing a sample
    void startPreview(const juce::File& file);
    
    // Stop previewing
    void stopPreview();

private:
    // Use the custom audio file filter
    AudioFileFilter fileFilter;
    std::unique_ptr<juce::FileBrowserComponent> fileBrowser;

    // Preview playback
    juce::AudioFormatManager formatManager;
    std::unique_ptr<juce::AudioFormatReaderSource> readerSource;
    juce::AudioTransportSource transportSource;
    juce::AudioSourcePlayer audioSourcePlayer;
    juce::AudioDeviceManager deviceManager;

    // Play/Stop button
    juce::TextButton playButton { "Play" };

    // Currently selected file
    juce::File selectedFile;

    // Drag highlight flag
    bool isShowingDragHighlight = false;

    // Timer callback for updating preview
    void timerCallback() override;
    
    // Create a new wildcard filter for audio files
    static juce::WildcardFileFilter* createAudioFileFilter();
    
    // Add a file to the recent files list
    void addToRecentFiles(const juce::File& file);
    
    // Setup preview components
    void setupPreviewComponents();

    // Content component for file browser

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SampleBrowserComponent)
};

} // namespace undergroundBeats