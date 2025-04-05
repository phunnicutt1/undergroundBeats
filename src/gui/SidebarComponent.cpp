#include "../../include/undergroundBeats/gui/SidebarComponent.h"

//==============================================================================
SidebarComponent::SidebarComponent()
{
    addAndMakeVisible(sampleBrowser);
}

SidebarComponent::~SidebarComponent()
{
}

void SidebarComponent::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::darkgrey.darker(0.6f));
}

void SidebarComponent::resized()
{
    sampleBrowser.setBounds(getLocalBounds().reduced(5));
}