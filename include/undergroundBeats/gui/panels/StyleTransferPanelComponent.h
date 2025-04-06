#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

namespace undergroundBeats {

//==============================================================================
/**
 * A component that displays style transfer controls and visualizations
 * for a specific stem.
 */
class StyleTransferPanelComponent : public juce::Component
{
public:
    StyleTransferPanelComponent();
    ~StyleTransferPanelComponent() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(StyleTransferPanelComponent)
}; 

} // namespace undergroundBeats 