#pragma once

#include "EffectPanelComponent.h"

// Compressor control panel with sliders/toggles for compressor parameters
class CompressorPanelComponent : public EffectPanelComponent
{
public:
    CompressorPanelComponent();
    ~CompressorPanelComponent() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    juce::Slider thresholdSlider, ratioSlider, attackSlider, releaseSlider, makeupGainSlider;
    juce::ToggleButton enableButton;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CompressorPanelComponent)
};