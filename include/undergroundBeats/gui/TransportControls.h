#pragma once

#include "JuceHeader.h"
#include "../UndergroundBeatsProcessor.h"

namespace undergroundBeats {

/**
 * Transport controls component providing play, pause, and stop buttons
 * for controlling audio playback.
 */
class TransportControls : public juce::Component,
                        public juce::Button::Listener
{
public:
    /**
     * Default constructor.
     */
    TransportControls();
    
    /**
     * Constructor.
     * @param processor Reference to the audio processor.
     */
    TransportControls(UndergroundBeatsProcessor& processor);
    
    /** Destructor. */
    ~TransportControls() override;

    /** Paint method. */
    void paint(juce::Graphics& g) override;
    
    /** Component resized method. */
    void resized() override;
    
    /** Button clicked event handler. */
    void buttonClicked(juce::Button* button) override;

    // Callbacks for transport actions
    std::function<void()> onPlay;
    std::function<void()> onPause;
    std::function<void()> onStop;

private:
    juce::TextButton playButton {"Play"};
    juce::TextButton pauseButton {"Pause"};
    juce::TextButton stopButton {"Stop"};
    
    // Reference to the audio processor (can be null)
    UndergroundBeatsProcessor* audioProcessor = nullptr;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TransportControls)
};

} // namespace undergroundBeats