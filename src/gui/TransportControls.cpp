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
    pauseButton.setButtonText("Pause");
    stopButton.setButtonText("Stop");
    
    // Add buttons to the component
    addAndMakeVisible(playButton);
    addAndMakeVisible(pauseButton);
    addAndMakeVisible(stopButton);
    
    // Set up listeners
    playButton.addListener(this);
    pauseButton.addListener(this);
    stopButton.addListener(this);
    
    // Disable buttons if there's no processor
    playButton.setEnabled(false);
    pauseButton.setEnabled(false);
    stopButton.setEnabled(false);
}

TransportControls::TransportControls(UndergroundBeatsProcessor& processor)
    : audioProcessor(&processor)
{
    // Initialize and configure buttons
    playButton.setButtonText("Play");
    pauseButton.setButtonText("Pause");
    stopButton.setButtonText("Stop");
    
    // Add buttons to the component
    addAndMakeVisible(playButton);
    addAndMakeVisible(pauseButton);
    addAndMakeVisible(stopButton);
    
    // Set up listeners
    playButton.addListener(this);
    pauseButton.addListener(this);
    stopButton.addListener(this);
}

TransportControls::~TransportControls()
{
    // Clean up listeners
    playButton.removeListener(this);
    pauseButton.removeListener(this);
    stopButton.removeListener(this);
}

//==============================================================================
void TransportControls::paint(juce::Graphics& g)
{
    // Fill background
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId).darker(0.2f));
    
    // Add border
    g.setColour(juce::Colours::grey);
    g.drawRect(getLocalBounds(), 1);
}

void TransportControls::resized()
{
    auto bounds = getLocalBounds().reduced(5);
    int buttonWidth = bounds.getWidth() / 3;
    
    playButton.setBounds(bounds.removeFromLeft(buttonWidth).reduced(2));
    pauseButton.setBounds(bounds.removeFromLeft(buttonWidth).reduced(2));
    stopButton.setBounds(bounds.reduced(2));
}

//==============================================================================
void TransportControls::buttonClicked(juce::Button* button)
{
    // Can't control playback without processor
    if (!audioProcessor)
        return;
    
    if (button == &playButton)
    {
        audioProcessor->startPlayback();
    }
    else if (button == &pauseButton)
    {
        audioProcessor->pausePlayback();
    }
    else if (button == &stopButton)
    {
        audioProcessor->stopPlayback();
    }
}

} // namespace undergroundBeats