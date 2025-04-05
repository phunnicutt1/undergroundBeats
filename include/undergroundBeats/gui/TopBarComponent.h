#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

//==============================================================================
/**
 * TopBarComponent provides the UI for file operations and settings.
 * Currently contains placeholder buttons.
*/
class TopBarComponent  : public juce::Component
{
public:
    //==============================================================================
    TopBarComponent();
    ~TopBarComponent() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    //==============================================================================
    juce::TextButton loadButton;
    juce::TextButton saveButton;
    juce::TextButton settingsButton;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TopBarComponent)
};