#include "../../include/undergroundBeats/gui/TransportControls.h"
#include "../../include/undergroundBeats/UndergroundBeatsProcessor.h"
#include "JuceHeader.h"

namespace undergroundBeats {

//==============================================================================
TransportControls::TransportControls()
    : audioProcessor(nullptr)
{
    // Initialize and configure buttons
    playButton.setButtonText("Play");
    playButton.setColour(juce::TextButton::buttonColourId, juce::Colours::darkgreen);
    playButton.setColour(juce::TextButton::buttonOnColourId, juce::Colours::green);
    
    pauseButton.setButtonText("Pause");
    pauseButton.setColour(juce::TextButton::buttonColourId, juce::Colours::darkblue);
    pauseButton.setColour(juce::TextButton::buttonOnColourId, juce::Colours::blue);
    
    stopButton.setButtonText("Stop");
    stopButton.setColour(juce::TextButton::buttonColourId, juce::Colours::darkred);
    stopButton.setColour(juce::TextButton::buttonOnColourId, juce::Colours::red);
    
    // Add buttons to the component
    addAndMakeVisible(playButton);
    addAndMakeVisible(pauseButton);
    addAndMakeVisible(stopButton);
    
    // Set up listeners
    playButton.addListener(this);
    pauseButton.addListener(this);
    stopButton.addListener(this);
    
    // Enable buttons - we'll handle processor state in the callbacks
    playButton.setEnabled(true);
    pauseButton.setEnabled(true);
    stopButton.setEnabled(true);
    
    DBG("TransportControls: Default constructor initialized");
}

TransportControls::TransportControls(UndergroundBeatsProcessor& processor)
    : audioProcessor(&processor)
{
    // Initialize and configure buttons
    playButton.setButtonText("Play");
    playButton.setColour(juce::TextButton::buttonColourId, juce::Colours::darkgreen);
    playButton.setColour(juce::TextButton::buttonOnColourId, juce::Colours::green);
    
    pauseButton.setButtonText("Pause");
    pauseButton.setColour(juce::TextButton::buttonColourId, juce::Colours::darkblue);
    pauseButton.setColour(juce::TextButton::buttonOnColourId, juce::Colours::blue);
    
    stopButton.setButtonText("Stop");
    stopButton.setColour(juce::TextButton::buttonColourId, juce::Colours::darkred);
    stopButton.setColour(juce::TextButton::buttonOnColourId, juce::Colours::red);
    
    // Add buttons to the component
    addAndMakeVisible(playButton);
    addAndMakeVisible(pauseButton);
    addAndMakeVisible(stopButton);
    
    // Set up listeners
    playButton.addListener(this);
    pauseButton.addListener(this);
    stopButton.addListener(this);
    
    // Enable buttons
    playButton.setEnabled(true);
    pauseButton.setEnabled(true);
    stopButton.setEnabled(true);
    
    DBG("TransportControls: Constructor with processor initialized. audioProcessor = " << 
        (audioProcessor != nullptr ? "valid" : "nullptr"));
}

TransportControls::~TransportControls()
{
    // Remove listeners to avoid callbacks after destruction
    playButton.removeListener(this);
    pauseButton.removeListener(this);
    stopButton.removeListener(this);
}

//==============================================================================
void TransportControls::paint(juce::Graphics& g)
{
    // Fill the background
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId).darker(0.2f));
    
    // Add a border
    g.setColour(juce::Colours::grey);
    g.drawRect(getLocalBounds(), 1);
}

void TransportControls::resized()
{
    auto bounds = getLocalBounds().reduced(4);
    int buttonWidth = bounds.getWidth() / 3;
    
    playButton.setBounds(bounds.removeFromLeft(buttonWidth).reduced(4));
    pauseButton.setBounds(bounds.removeFromLeft(buttonWidth).reduced(4));
    stopButton.setBounds(bounds.reduced(4));
}

//==============================================================================
void TransportControls::setProcessor(UndergroundBeatsProcessor* processor)
{
    audioProcessor = processor;
    DBG("TransportControls: setProcessor called, audioProcessor = " << 
        (audioProcessor != nullptr ? "valid" : "nullptr"));
}

void TransportControls::buttonClicked(juce::Button* button)
{
    DBG("TransportControls::buttonClicked called");
    
    // First, provide visual feedback
    button->setColour(juce::TextButton::buttonColourId, juce::Colours::white);
    startTimer(200); // Start a timer to reset the color after a short delay
    
    if (button == &playButton)
    {
        DBG("Transport: Play clicked");
        
        // Update button visual state
        updateState(true, false);
        
        // ALWAYS try direct processor call first
        if (audioProcessor != nullptr)
        {
            DBG("Transport: Direct call to processor startPlayback()");
            audioProcessor->startPlayback();
        }
        // Then try callback as fallback
        else if (onPlay) 
        {
            DBG("Transport: Calling onPlay callback");
            onPlay();
        }
        else
        {
            DBG("Transport: ERROR - audioProcessor is nullptr AND onPlay callback is NOT set!");
        }
    }
    else if (button == &pauseButton)
    {
        DBG("Transport: Pause clicked");
        
        // Update button visual state
        updateState(true, true);
        
        // ALWAYS try direct processor call first
        if (audioProcessor != nullptr)
        {
            DBG("Transport: Direct call to processor pausePlayback()");
            audioProcessor->pausePlayback();
        }
        // Then try callback as fallback
        else if (onPause) 
        {
            DBG("Transport: Calling onPause callback");
            onPause();
        }
        else
        {
            DBG("Transport: ERROR - audioProcessor is nullptr AND onPause callback is NOT set!");
        }
    }
    else if (button == &stopButton)
    {
        DBG("Transport: Stop clicked");
        
        // Update button visual state
        updateState(false, false);
        
        // ALWAYS try direct processor call first
        if (audioProcessor != nullptr)
        {
            DBG("Transport: Direct call to processor stopPlayback()");
            audioProcessor->stopPlayback();
        }
        // Then try callback as fallback
        else if (onStop) 
        {
            DBG("Transport: Calling onStop callback");
            onStop();
        }
        else
        {
            DBG("Transport: ERROR - audioProcessor is nullptr AND onStop callback is NOT set!");
        }
    }
}

void TransportControls::updateState(bool isPlaying, bool isPaused)
{
    // Update button colors and appearance based on state
    if (isPlaying && !isPaused)
    {
        // Playing state
        playButton.setToggleState(true, juce::dontSendNotification);
        pauseButton.setToggleState(false, juce::dontSendNotification);
        stopButton.setToggleState(false, juce::dontSendNotification);
        
        playButton.setColour(juce::TextButton::buttonColourId, juce::Colours::green);
        pauseButton.setColour(juce::TextButton::buttonColourId, juce::Colours::darkblue);
        stopButton.setColour(juce::TextButton::buttonColourId, juce::Colours::darkred);
        
        DBG("Transport: UI updated to Playing state");
    }
    else if (isPlaying && isPaused)
    {
        // Paused state
        playButton.setToggleState(false, juce::dontSendNotification);
        pauseButton.setToggleState(true, juce::dontSendNotification);
        stopButton.setToggleState(false, juce::dontSendNotification);
        
        playButton.setColour(juce::TextButton::buttonColourId, juce::Colours::darkgreen);
        pauseButton.setColour(juce::TextButton::buttonColourId, juce::Colours::blue);
        stopButton.setColour(juce::TextButton::buttonColourId, juce::Colours::darkred);
        
        DBG("Transport: UI updated to Paused state");
    }
    else // Stopped
    {
        // Stopped state
        playButton.setToggleState(false, juce::dontSendNotification);
        pauseButton.setToggleState(false, juce::dontSendNotification);
        stopButton.setToggleState(true, juce::dontSendNotification);
        
        playButton.setColour(juce::TextButton::buttonColourId, juce::Colours::darkgreen);
        pauseButton.setColour(juce::TextButton::buttonColourId, juce::Colours::darkblue);
        stopButton.setColour(juce::TextButton::buttonColourId, juce::Colours::red);
        
        DBG("Transport: UI updated to Stopped state");
    }
}

// Timer callback to reset button colors
void TransportControls::timerCallback()
{
    // Reset button colors
    playButton.setColour(juce::TextButton::buttonColourId, juce::Colours::darkgreen);
    pauseButton.setColour(juce::TextButton::buttonColourId, juce::Colours::darkblue);
    stopButton.setColour(juce::TextButton::buttonColourId, juce::Colours::darkred);
    
    // Stop the timer
    stopTimer();
    
    DBG("Transport: Reset button colors");
    
    // Update visual state based on processor state
    if (audioProcessor != nullptr)
    {
        updateState(audioProcessor->isPlaying(), audioProcessor->isPaused());
    }
}

} // namespace undergroundBeats