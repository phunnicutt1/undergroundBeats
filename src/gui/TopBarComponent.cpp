#include "../../include/undergroundBeats/gui/TopBarComponent.h"
#include "../../include/undergroundBeats/UndergroundBeatsProcessor.h"
#include "../../include/undergroundBeats/gui/SidebarComponent.h"
#include "../../include/undergroundBeats/gui/SampleBrowserComponent.h"

namespace undergroundBeats {

//==============================================================================
TopBarComponent::TopBarComponent(UndergroundBeatsProcessor& processor, SidebarComponent& sidebar)
    : processorRef(processor), sidebarRef(sidebar)
{
    addAndMakeVisible(projectNameLabel);
    projectNameLabel.setText("Untitled Project", juce::dontSendNotification);
    projectNameLabel.setColour(juce::Label::textColourId, juce::Colours::white);

    addAndMakeVisible(saveStatusIndicator);
    saveStatusIndicator.setColour(juce::Label::backgroundColourId, juce::Colours::green);
    saveStatusIndicator.setColour(juce::Label::textColourId, juce::Colours::transparentBlack);

    addAndMakeVisible(loadButton);
    loadButton.addListener(this);
    loadButton.setEnabled(false); // Initially disabled

    addAndMakeVisible(saveButton);
    saveButton.addListener(this);
    
    addAndMakeVisible(settingsButton);
    settingsButton.addListener(this);
    
    addAndMakeVisible(helpButton);
    helpButton.addListener(this);
    
    // Start listening to changes from the sample browser
    // Check if pointer is valid before adding listener
    if (auto* browser = sidebarRef.getSampleBrowser())
        browser->addChangeListener(this);
    else
        DBG("TopBarComponent Warning: Sidebar has no valid SampleBrowser to listen to!");

    setColour(juce::ResizableWindow::backgroundColourId, juce::Colours::darkgrey.darker(0.8f));

    setSize(800, 40);
}

TopBarComponent::~TopBarComponent()
{
    // Stop listening
    // Check if pointer is valid before removing listener
    if (auto* browser = sidebarRef.getSampleBrowser())
        browser->removeChangeListener(this);
}

void TopBarComponent::paint(juce::Graphics& g)
{
    g.fillAll(findColour(juce::ResizableWindow::backgroundColourId));

    // Bottom border
    g.setColour(juce::Colours::black);
    g.fillRect(0, getHeight() - 1, getWidth(), 1);

    // Draw save status indicator as a circle
    auto indicatorBounds = saveStatusIndicator.getBounds().toFloat();
    g.setColour(saveStatusIndicator.findColour(juce::Label::backgroundColourId));
    g.fillEllipse(indicatorBounds);
}

void TopBarComponent::resized()
{
    auto area = getLocalBounds().reduced(4);

    const int buttonWidth = 80;
    const int spacing = 8;

    // Arrange buttons on right
    helpButton.setBounds(area.removeFromRight(buttonWidth));
    area.removeFromRight(spacing);
    settingsButton.setBounds(area.removeFromRight(buttonWidth));
    area.removeFromRight(spacing);
    saveButton.setBounds(area.removeFromRight(buttonWidth));
    area.removeFromRight(spacing);
    loadButton.setBounds(area.removeFromRight(buttonWidth));

    // Save status indicator (small circle) to left of project name
    auto leftArea = area.removeFromLeft(200);
    auto indicatorSize = 12;
    saveStatusIndicator.setBounds(leftArea.removeFromLeft(indicatorSize).reduced(2));

    projectNameLabel.setBounds(leftArea);
}

void TopBarComponent::buttonClicked(juce::Button* button)
{
    if (button == &loadButton)
    {
        DBG("TopBar: Load button clicked.");
        if (currentlySelectedFile.existsAsFile())
        {
             DBG("TopBar: Loading selected file: " + currentlySelectedFile.getFullPathName());
             processorRef.loadAudioFile(currentlySelectedFile);
        }
        else
        {
            DBG("TopBar: No valid file selected to load.");
        }
    }
    else if (button == &saveButton)
    {
        DBG("TopBar: Save button clicked - Not implemented.");
    }
    else if (button == &settingsButton)
    {
        DBG("TopBar: Settings button clicked - Not implemented.");
    }
    else if (button == &helpButton)
    {
        DBG("TopBar: Help button clicked - Not implemented.");
    }
}

void TopBarComponent::changeListenerCallback(juce::ChangeBroadcaster* source)
{
    // Check if the change came from the sample browser we are listening to
    auto* expectedBrowser = sidebarRef.getSampleBrowser();
    if (expectedBrowser != nullptr && source == expectedBrowser)
    {
        // Update the currently selected file
        currentlySelectedFile = expectedBrowser->getSelectedFile();
        DBG("TopBar: Sample browser selection changed to: " + currentlySelectedFile.getFileName());

        // Enable the load button only if a valid file is selected
        bool fileIsValid = currentlySelectedFile.existsAsFile() && expectedBrowser->isFileSuitable(currentlySelectedFile);
        loadButton.setEnabled(fileIsValid);
         DBG("TopBar: Load button enabled: " + juce::String(fileIsValid ? "Yes" : "No"));
    }
}

} // namespace undergroundBeats