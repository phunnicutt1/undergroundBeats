#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

namespace undergroundBeats {

//==============================================================================
/**
 * A component that displays an EQ interface with frequency response visualization
 * and parameter controls for a specific stem.
 */
class EQPanelComponent : public juce::Component
{
public:
    EQPanelComponent();
    ~EQPanelComponent() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EQPanelComponent)
}; 

} // namespace undergroundBeats 