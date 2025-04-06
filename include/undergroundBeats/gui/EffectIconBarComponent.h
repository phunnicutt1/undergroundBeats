#pragma once

#include "JuceHeader.h"

namespace undergroundBeats {

/**
 * A horizontal bar of effect icons that toggle collapsible panels.
 */
class EffectIconBarComponent : public juce::Component
{
public:
    EffectIconBarComponent()
    {
        eqButton.setButtonText("EQ");
        eqButton.setTooltip("Equalizer");
        addAndMakeVisible(eqButton);

        compButton.setButtonText("Comp");
        compButton.setTooltip("Compressor");
        addAndMakeVisible(compButton);

        reverbButton.setButtonText("Reverb");
        reverbButton.setTooltip("Reverb Effect");
        addAndMakeVisible(reverbButton);

        delayButton.setButtonText("Delay");
        delayButton.setTooltip("Delay Effect");
        addAndMakeVisible(delayButton);

        chorusButton.setButtonText("Chorus");
        chorusButton.setTooltip("Chorus Effect");
        addAndMakeVisible(chorusButton);

        saturationButton.setButtonText("Saturate");
        saturationButton.setTooltip("Saturation Effect");
        addAndMakeVisible(saturationButton);

        styleButton.setButtonText("Style");
        styleButton.setTooltip("Style Transfer");
        addAndMakeVisible(styleButton);
    }

    ~EffectIconBarComponent() override = default;

    void resized() override
    {
        auto area = getLocalBounds().reduced(4);
        int buttonWidth = 60;
        int spacing = 10;

        eqButton.setBounds(area.removeFromLeft(buttonWidth));
        area.removeFromLeft(spacing);
        compButton.setBounds(area.removeFromLeft(buttonWidth));
        area.removeFromLeft(spacing);
        reverbButton.setBounds(area.removeFromLeft(buttonWidth));
        area.removeFromLeft(spacing);
        delayButton.setBounds(area.removeFromLeft(buttonWidth));
        area.removeFromLeft(spacing);
        chorusButton.setBounds(area.removeFromLeft(buttonWidth));
        area.removeFromLeft(spacing);
        saturationButton.setBounds(area.removeFromLeft(buttonWidth));
        area.removeFromLeft(spacing);
        styleButton.setBounds(area.removeFromLeft(buttonWidth));
    }

    juce::TextButton eqButton;
    juce::TextButton compButton;
    juce::TextButton reverbButton;
    juce::TextButton delayButton;
    juce::TextButton chorusButton;
    juce::TextButton saturationButton;
    juce::TextButton styleButton;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EffectIconBarComponent)
};

} // namespace undergroundBeats