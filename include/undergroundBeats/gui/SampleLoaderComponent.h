#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_utils/juce_audio_utils.h>
#include "undergroundBeats/audio/AudioEngine.h"

namespace undergroundBeats {
namespace gui {

/**
 * @class SampleLoaderComponent
 * @brief Component for loading and playing audio samples.
 * 
 * This component provides a UI for loading, visualizing, and playing audio samples.
 */
class SampleLoaderComponent : public juce::Component,
                             public juce::FileDragAndDropTarget,
                             public juce::Button::Listener,
                             public juce::Slider::Listener
{
public:
    /**
     * @brief Constructor for the SampleLoaderComponent class.
     * @param audioEngine Reference to the application's audio engine.
     */
    SampleLoaderComponent(audio::AudioEngine& audioEngine);
    
    /**
     * @brief Destructor for the SampleLoaderComponent class.
     */
    ~SampleLoaderComponent() override;
    
    /**
     * @brief Paint the component.
     * @param g Graphics context to use for drawing.
     */
    void paint(juce::Graphics& g) override;
    
    /**
     * @brief Called when the component is resized.
     */
    void resized() override;
    
    /**
     * @brief Load a sample from a file.
     * @param file The file to load.
     * @return True if the sample was loaded successfully, false otherwise.
     */
    bool loadSample(const juce::File& file);
    
    // FileDragAndDropTarget interface implementation
    bool isInterestedInFileDrag(const juce::StringArray& files) override;
    void filesDropped(const juce::StringArray& files, int x, int y) override;
    
    // Button::Listener implementation
    void buttonClicked(juce::Button* button) override;
    
    // Slider::Listener implementation
    void sliderValueChanged(juce::Slider* slider) override;

private:
    audio::AudioEngine& audioEngine;
    
    // Sample player pointer (owned by the audio engine)
    audio::SamplePlayer* currentPlayer;
    
    // UI Components
    juce::TextButton loadButton;
    juce::TextButton playButton;
    juce::TextButton stopButton;
    juce::Label fileNameLabel;
    juce::Label statusLabel;
    juce::Slider speedSlider;
    juce::Label speedLabel;
    juce::Slider volumeSlider;
    juce::Label volumeLabel;
    juce::ToggleButton loopToggle;
    
    // Waveform display
    juce::AudioThumbnail thumbnail;
    juce::AudioThumbnailCache thumbnailCache;
    
    // Methods to update UI state
    void updateButtons();
    void updateLabels();
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SampleLoaderComponent)
};

} // namespace gui
} // namespace undergroundBeats