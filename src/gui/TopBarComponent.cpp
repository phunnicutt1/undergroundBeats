#include "../../include/undergroundBeats/gui/TopBarComponent.h"

//==============================================================================
TopBarComponent::TopBarComponent()
    : loadButton("Load"),
      saveButton("Save"),
      settingsButton("Settings")
{
    // Add buttons to component
    addAndMakeVisible(loadButton);
    addAndMakeVisible(saveButton);
    addAndMakeVisible(settingsButton);
    
    // Use ResizableWindow's background color instead of Component's
    setColour(juce::ResizableWindow::backgroundColourId, juce::Colours::darkgrey.darker(0.8f));
    
    // Set default size
    setSize(600, 40);
}

TopBarComponent::~TopBarComponent()
{
    // Destructor
}

//==============================================================================
void TopBarComponent::paint(juce::Graphics& g)
{
    g.fillAll(findColour(juce::ResizableWindow::backgroundColourId));
    
    // Add a subtle border at the bottom
    g.setColour(juce::Colours::black);
    g.fillRect(0, getHeight() - 1, getWidth(), 1);
}

void TopBarComponent::resized()
{
    // Layout: buttons evenly sized and aligned to the right
    auto area = getLocalBounds().reduced(4);
    
    // Place buttons from right to left
    const int buttonWidth = 80;
    const int spacing = 10;
    
    settingsButton.setBounds(area.removeFromRight(buttonWidth));
    area.removeFromRight(spacing);
    saveButton.setBounds(area.removeFromRight(buttonWidth));
    area.removeFromRight(spacing);
    loadButton.setBounds(area.removeFromRight(buttonWidth));
}