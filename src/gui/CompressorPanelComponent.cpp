#include "undergroundBeats/gui/CompressorPanelComponent.h"

CompressorPanelComponent::CompressorPanelComponent()
{
    addAndMakeVisible(enableButton);
    enableButton.setButtonText("Enable Compressor");

    auto sliders = { &thresholdSlider, &ratioSlider, &attackSlider, &releaseSlider, &makeupGainSlider };

    for (auto* slider : sliders)
    {
        addAndMakeVisible(*slider);
        slider->setSliderStyle(juce::Slider::Rotary);
        slider->setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
    }
}

CompressorPanelComponent::~CompressorPanelComponent() = default;

void CompressorPanelComponent::paint(juce::Graphics& g)
{
    EffectPanelComponent::paint(g);
}

void CompressorPanelComponent::resized()
{
    auto area = getLocalBounds().reduced(10);
    enableButton.setBounds(area.removeFromTop(25));

    auto sliderArea = area.reduced(0, 10);
    auto rowHeight = sliderArea.getHeight();

    auto sliderWidth = sliderArea.getWidth() / 5;

    thresholdSlider.setBounds(sliderArea.removeFromLeft(sliderWidth).reduced(5));
    ratioSlider.setBounds(sliderArea.removeFromLeft(sliderWidth).reduced(5));
    attackSlider.setBounds(sliderArea.removeFromLeft(sliderWidth).reduced(5));
    releaseSlider.setBounds(sliderArea.removeFromLeft(sliderWidth).reduced(5));
    makeupGainSlider.setBounds(sliderArea.reduced(5));
}