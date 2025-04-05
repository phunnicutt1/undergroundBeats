#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

//==============================================================================
/**
 * A component that displays a grid of variation options for exploring
 * different processing settings across stems.
 */
class VariationExplorerComponent : public juce::Component
{
public:
    VariationExplorerComponent();
    ~VariationExplorerComponent() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(VariationExplorerComponent)
}; 