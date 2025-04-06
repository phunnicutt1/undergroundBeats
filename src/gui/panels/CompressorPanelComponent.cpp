#include "../../../include/undergroundBeats/gui/panels/CompressorPanelComponent.h"

namespace undergroundBeats {

CompressorPanelComponent::CompressorPanelComponent()
{
    setSize(500, 300);
}

CompressorPanelComponent::~CompressorPanelComponent()
{
}

void CompressorPanelComponent::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::darkgrey.darker(0.8f));
    g.setColour(juce::Colours::white);
    g.setFont(15.0f);
    g.drawText("Compressor Panel", getLocalBounds(), juce::Justification::centred, true);
}

void CompressorPanelComponent::resized()
{
    // Layout controls when implemented
} 

} // namespace undergroundBeats 