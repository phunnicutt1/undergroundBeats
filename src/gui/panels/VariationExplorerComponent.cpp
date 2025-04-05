#include "../../../include/undergroundBeats/gui/panels/VariationExplorerComponent.h"

VariationExplorerComponent::VariationExplorerComponent()
{
    setSize(600, 400);
}

VariationExplorerComponent::~VariationExplorerComponent()
{
}

void VariationExplorerComponent::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::black.withAlpha(0.8f));
    g.setColour(juce::Colours::white);
    g.setFont(15.0f);
    g.drawText("Variation Explorer", getLocalBounds(), juce::Justification::centred, true);
}

void VariationExplorerComponent::resized()
{
    // Layout controls when implemented
} 