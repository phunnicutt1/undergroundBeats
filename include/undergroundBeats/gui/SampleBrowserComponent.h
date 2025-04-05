#pragma once

#include "JuceHeader.h"
#include "AudioFileFilter.h" // Include the custom filter
#include <functional> // For std::function

// Forward declaration for the constructor argument
namespace juce { class FilePreviewComponent; }

/**
 * Displays a filtered file browser for audio samples with preview and drag support.
 */
class SampleBrowserComponent : public juce::Component,
                               public juce::FileBrowserListener,
                               public juce::Button::Listener,
                               public juce::FileDragAndDropTarget
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

    // Helpers
    void startPreview(const juce::File& file);
    void stopPreview();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SampleBrowserComponent)
};