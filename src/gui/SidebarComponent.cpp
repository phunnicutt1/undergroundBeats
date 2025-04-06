#include "../../include/undergroundBeats/gui/SidebarComponent.h"
#include "../../include/undergroundBeats/gui/SampleBrowserComponent.h"

namespace undergroundBeats {

//==============================================================================
SidebarComponent::SidebarComponent()
{
    // Create the sample browser instance
    sampleBrowser = std::make_unique<SampleBrowserComponent>();

    addAndMakeVisible(toggleButton);
    toggleButton.onClick = [this] { toggleCollapse(); };
    
    addAndMakeVisible(searchLabel);
    addAndMakeVisible(searchBox);
    
    addAndMakeVisible(sampleBrowser.get());

    setSize(200, 600);
}

SidebarComponent::~SidebarComponent()
{
    // unique_ptr manages sampleBrowser lifetime
}

void SidebarComponent::paint(juce::Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId).darker(0.1f));
    // Draw a dividing line on the right
    g.setColour(juce::Colours::black);
    g.fillRect(getWidth() - 1, 0, 1, getHeight());
}

void SidebarComponent::resized()
{
    auto area = getLocalBounds().reduced(2);
    
    toggleButton.setBounds(area.removeFromTop(20));
    
    auto searchArea = area.removeFromTop(30);
    searchLabel.setBounds(searchArea.removeFromLeft(60));
    searchBox.setBounds(searchArea);
    
    // Sample browser takes remaining space
    if (sampleBrowser)
        sampleBrowser->setBounds(area);
}

void SidebarComponent::toggleCollapse()
{
    collapsed = !collapsed;
    toggleButton.setButtonText(collapsed ? ">>" : "<<");
    
    // TODO: Implement actual collapse/expand animation and resizing
    DBG("Sidebar collapse toggled (Not fully implemented)");
    if (getParentComponent())
        getParentComponent()->resized(); // Trigger parent resize
}

// Implement the getter
SampleBrowserComponent* SidebarComponent::getSampleBrowser()
{
    return sampleBrowser.get();
}

} // namespace undergroundBeats