#pragma once

#include "../JuceHeader.h" // Include from parent gui directory

namespace undergroundBeats
{

//==============================================================================
/*
    Placeholder for the Reverb Panel Component.
*/
class ReverbPanelComponent : public juce::Component
{
public:
    ReverbPanelComponent()
    {
        // Constructor logic for the reverb panel will go here
        // For now, just set a background color for visibility if needed
        // setOpaque(true);
    }

    ~ReverbPanelComponent() override = default;

    void paint (juce::Graphics& g) override
    {
        // Placeholder paint routine
        g.fillAll (juce::Colours::darkred); // Placeholder background
        g.setColour (juce::Colours::white);
        g.setFont (14.0f);
        g.drawText ("Reverb Panel Placeholder", getLocalBounds(), juce::Justification::centred, true);
    }

    void resized() override
    {
        // Layout code for reverb controls will go here
    }

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ReverbPanelComponent)
};

} // namespace undergroundBeats 