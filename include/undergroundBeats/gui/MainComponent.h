#pragma once

#include <JuceHeader.h>
#include "MainEditor.h" // Include MainEditor header
#include "SidebarComponent.h" // Include SidebarComponent header
#include "TopBarComponent.h" // Include TopBarComponent header

//==============================================================================
/**
 * MainComponent serves as the top-level container for the application's UI.
 * It arranges the main sections: Top Bar, Left Sidebar, Main Workspace (Editor), and Bottom Panel.
*/
class MainComponent  : public juce::Component
{
public:
    //==============================================================================
    MainComponent(UndergroundBeatsProcessor& processor); // Pass processor reference
    ~MainComponent() override;

    //==============================================================================
    void paint (juce::Graphics&amp;) override;
    void resized() override;

private:
    //==============================================================================
    // Reference to the audio processor
    UndergroundBeatsProcessor&amp; audioProcessor;

    // Child Components
    TopBarComponent topBarComponent; // Specific component for the top bar
    SidebarComponent leftSidebar; // Use the specific SidebarComponent type
    MainEditor mainEditor; // Main workspace area
    juce::Component bottomPanel; // Panel to hold transport controls

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};