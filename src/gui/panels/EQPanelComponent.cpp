#include "../../../include/undergroundBeats/gui/panels/EQPanelComponent.h"

namespace undergroundBeats {

EQPanelComponent::EQPanelComponent()
{
    setSize(500, 300);
}

EQPanelComponent::~EQPanelComponent()
{
}

void EQPanelComponent::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::darkgrey.darker(0.8f));
    g.setColour(juce::Colours::white);
    g.setFont(15.0f);
    g.drawText("EQ Panel", getLocalBounds(), juce::Justification::centred, true);
}

void EQPanelComponent::resized()
{
    // Layout controls when implemented
} 

} // namespace undergroundBeats 