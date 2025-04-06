#pragma once

#include "JuceHeader.h"

namespace undergroundBeats
{

// Forward declarations to resolve dependencies
class UndergroundBeatsProcessor;
class SidebarComponent; // Forward declaration within namespace

// #include "undergroundBeats/gui/SidebarComponent.h" // Removed earlier

//==============================================================================
/**
 * TopBarComponent provides the UI for file operations, project info, and settings.
 */
class TopBarComponent : public juce::Component,
                          public juce::Button::Listener,
                          public juce::ChangeListener
{
public:
    // Constructor signature uses types within this namespace
    TopBarComponent(UndergroundBeatsProcessor& processor, SidebarComponent& sidebar);
    ~TopBarComponent() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

    void buttonClicked(juce::Button* button) override;
    void changeListenerCallback(juce::ChangeBroadcaster* source) override;

private:
    // Reference types are within this namespace
    UndergroundBeatsProcessor& processorRef;
    SidebarComponent& sidebarRef;

    juce::Label projectNameLabel { {}, "Untitled Project" };
    juce::Label saveStatusIndicator;
    juce::TextButton loadButton { "Load Selected" };
    juce::TextButton saveButton { "Save" };
    juce::TextButton settingsButton { "Settings" };
    juce::TextButton helpButton { "?" };

    juce::File currentlySelectedFile;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TopBarComponent)
};

} // namespace undergroundBeats // Close namespace