#include "../../../include/undergroundBeats/gui/panels/StyleTransferPanelComponent.h"

namespace undergroundBeats {

StyleTransferPanelComponent::StyleTransferPanelComponent()
{
    setSize(500, 300);
}

StyleTransferPanelComponent::~StyleTransferPanelComponent()
{
}

void StyleTransferPanelComponent::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::darkgrey.darker(0.8f));
    g.setColour(juce::Colours::white);
    g.setFont(15.0f);
    g.drawText("Style Transfer Panel", getLocalBounds(), juce::Justification::centred, true);
}

void StyleTransferPanelComponent::resized()
{
    // Layout controls when implemented
} 

} // namespace undergroundBeats 