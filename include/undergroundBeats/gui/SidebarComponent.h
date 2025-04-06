#pragma once

#include "JuceHeader.h"
// #include "undergroundBeats/gui/SampleBrowserComponent.h" // Include in cpp instead

namespace undergroundBeats // Add namespace
{

// Forward declaration
class SampleBrowserComponent;

//==============================================================================
// SidebarComponent embeds the SampleBrowserComponent with filter/search and collapse toggle
class SidebarComponent : public juce::Component
{
public:
    SidebarComponent();
    ~SidebarComponent() override;

    void paint(juce::Graphics&) override;
    void resized() override;

    bool isCollapsed() const { return collapsed; }

    // Getter returns pointer now
    SampleBrowserComponent* getSampleBrowser(); // Return pointer

private:
    bool collapsed = false;

    juce::TextButton toggleButton { "<<" };
    juce::Label searchLabel { {}, "Search:" };
    juce::TextEditor searchBox;

    // Use unique_ptr
    std::unique_ptr<SampleBrowserComponent> sampleBrowser;

    void toggleCollapse();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SidebarComponent)
};

} // namespace undergroundBeats // Close namespace