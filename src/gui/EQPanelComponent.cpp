#include "undergroundBeats/gui/EQPanelComponent.h"

EQPanelComponent::EQPanelComponent()
{
    addAndMakeVisible(enableButton);
    enableButton.setButtonText("Enable EQ");

    auto sliders = { &lowGainSlider, &midGainSlider, &highGainSlider,
                     &lowFreqSlider, &midFreqSlider, &highFreqSlider };

    for (auto* slider : sliders)
    {
        addAndMakeVisible(*slider);
        slider->setSliderStyle(juce::Slider::Rotary);
        slider->setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
    }
}

EQPanelComponent::~EQPanelComponent() = default;

void EQPanelComponent::paint(juce::Graphics& g)
{
    EffectPanelComponent::paint(g);
}

void EQPanelComponent::resized()
{
    auto area = getLocalBounds().reduced(10);
    enableButton.setBounds(area.removeFromTop(25));

    auto sliderArea = area.reduced(0, 10);
    auto rowHeight = sliderArea.getHeight() / 2;

    auto topRow = sliderArea.removeFromTop(rowHeight);
    auto bottomRow = sliderArea;

    auto sliderWidth = topRow.getWidth() / 3;

    lowGainSlider.setBounds(topRow.removeFromLeft(sliderWidth).reduced(5));
    midGainSlider.setBounds(topRow.removeFromLeft(sliderWidth).reduced(5));
    highGainSlider.setBounds(topRow.reduced(5));

    lowFreqSlider.setBounds(bottomRow.removeFromLeft(sliderWidth).reduced(5));
    midFreqSlider.setBounds(bottomRow.removeFromLeft(sliderWidth).reduced(5));
    highFreqSlider.setBounds(bottomRow.reduced(5));
}