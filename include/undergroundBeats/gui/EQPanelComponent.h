#pragma once

#include "EffectPanelComponent.h"

// EQ control panel with sliders/toggles for EQ parameters
class EQPanelComponent : public EffectPanelComponent
{
public:
    EQPanelComponent();
    ~EQPanelComponent() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    // Example EQ controls
    juce::Slider lowGainSlider, midGainSlider, highGainSlider;
    juce::Slider lowFreqSlider, midFreqSlider, highFreqSlider;
    juce::ToggleButton enableButton;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EQPanelComponent)
};