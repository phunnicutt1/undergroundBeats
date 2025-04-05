#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_basics/juce_audio_basics.h>
#include "undergroundBeats/gui/SampleBrowserComponent.h"

//==============================================================================
// SidebarComponent now embeds the SampleBrowserComponent
class SidebarComponent : public juce::Component
{
public:
    SidebarComponent();
    ~SidebarComponent() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    SampleBrowserComponent sampleBrowser;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SidebarComponent)
};