#include "undergroundBeats/gui/StyleTransferPanelComponent.h"

StyleTransferPanelComponent::StyleTransferPanelComponent()
{
    addAndMakeVisible(enableButton);
    enableButton.setButtonText("Enable Style Transfer");

    addAndMakeVisible(styleAmountSlider);
    styleAmountSlider.setSliderStyle(juce::Slider::Rotary);
    styleAmountSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);

    addAndMakeVisible(stylePresetBox);
    stylePresetBox.addItem("Hip-Hop", 1);
    stylePresetBox.addItem("Trap", 2);
    stylePresetBox.addItem("Jazz", 3);
    stylePresetBox.addItem("Electronic", 4);
    stylePresetBox.addItem("Custom", 5);
}

StyleTransferPanelComponent::~StyleTransferPanelComponent() = default;

void StyleTransferPanelComponent::paint(juce::Graphics& g)
{
    EffectPanelComponent::paint(g);
}

void StyleTransferPanelComponent::resized()
{
    auto area = getLocalBounds().reduced(10);
    enableButton.setBounds(area.removeFromTop(25));

    stylePresetBox.setBounds(area.removeFromTop(30).reduced(5));
    styleAmountSlider.setBounds(area.reduced(5));
}