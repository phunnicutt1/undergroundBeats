#pragma once

#include "JuceHeader.h"
#include "../UndergroundBeatsProcessor.h"

namespace undergroundBeats {

/**
 * A component that provides playback controls.
 */
class TransportControls : public juce::Component,
                          public juce::Button::Listener,
                          public juce::Timer
{
public:
    /** Default constructor. */
    TransportControls();

    /** Constructor with processor. */
    TransportControls(UndergroundBeatsProcessor& processor);

    /** Destructor. */
    ~TransportControls() override;

    //==============================================================================
    /** Paint the component. */
    void paint(juce::Graphics&) override;

    /** Handle resizing. */
    void resized() override;

    /** Update the transport controls based on the processor state. */
    void updateState(bool isPlaying, bool isPaused);
    
    /** Set the processor to use for direct control. */
    void setProcessor(UndergroundBeatsProcessor* processor);

    // Callbacks
    std::function<void()> onPlay;
    std::function<void()> onPause;
    std::function<void()> onStop;

private:
    //==============================================================================
    /** Handle button clicks. */
    void buttonClicked(juce::Button* button) override;
    
    /** Timer callback for UI animations. */
    void timerCallback() override;

    juce::TextButton playButton;
    juce::TextButton pauseButton;
    juce::TextButton stopButton;

    // The processor that we'll control
    UndergroundBeatsProcessor* audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TransportControls)
};

} // namespace undergroundBeats