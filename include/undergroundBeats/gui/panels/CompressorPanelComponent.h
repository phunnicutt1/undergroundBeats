#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

namespace undergroundBeats {

//==============================================================================
/**
 * A component that displays a compressor interface with gain reduction visualization
 * and parameter controls for a specific stem.
 */
class CompressorPanelComponent : public juce::Component
{
public:
    CompressorPanelComponent();
    ~CompressorPanelComponent() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CompressorPanelComponent)
}; 

} // namespace undergroundBeats 