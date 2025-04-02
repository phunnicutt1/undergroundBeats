#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_gui_extra/juce_gui_extra.h>
#include "undergroundBeats/audio/AudioEngine.h"
#include "undergroundBeats/gui/SampleLoaderComponent.h"

namespace undergroundBeats {
namespace gui {

/**
 * @class MainComponent
 * @brief Main application component for the Underground Beats UI.
 * 
 * This class serves as the root component for the application's user interface.
 * It contains the main UI layout and controls for the audio engine.
 */
class MainComponent : public juce::Component
{
public:
    /**
     * @brief Constructor for the MainComponent class.
     * @param audioEngine Reference to the application's audio engine.
     */
    MainComponent(audio::AudioEngine& audioEngine);
    
    /**
     * @brief Destructor for the MainComponent class.
     */
    ~MainComponent() override;

    /**
     * @brief Paint the component.
     * @param g Graphics context to use for drawing.
     */
    void paint(juce::Graphics& g) override;
    
    /**
     * @brief Called when the component is resized.
     */
    void resized() override;

private:
    audio::AudioEngine& audioEngine;
    
    // UI Components
    juce::TabbedComponent tabbedComponent;
    
    // Test Tone components
    juce::TextButton startButton;
    juce::TextButton stopButton;
    juce::Slider frequencySlider;
    juce::Slider amplitudeSlider;
    juce::Label frequencyLabel;
    juce::Label amplitudeLabel;
    
    // Sample loader component
    SampleLoaderComponent sampleLoaderComponent;
    
    // Button click handlers
    void startButtonClicked();
    void stopButtonClicked();
    
    // Slider value change handlers
    void frequencySliderChanged();
    void amplitudeSliderChanged();
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};

} // namespace gui
} // namespace undergroundBeats