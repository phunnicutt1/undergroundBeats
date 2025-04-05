#pragma once

#include "EffectPanelComponent.h"

// Style Transfer control panel with sliders/toggles for style parameters
class StyleTransferPanelComponent : public EffectPanelComponent
{
public:
    StyleTransferPanelComponent();
    ~StyleTransferPanelComponent() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    juce::Slider styleAmountSlider;
    juce::ComboBox stylePresetBox;
    juce::ToggleButton enableButton;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(StyleTransferPanelComponent)
};