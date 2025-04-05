#include "gui/MainComponent.h"
#include "UndergroundBeatsProcessor.h" // Include processor header

//==============================================================================
MainComponent::MainComponent(UndergroundBeatsProcessor&amp; processor)
    : audioProcessor(processor), mainEditor(processor) // Initialize mainEditor with processor reference
{
    // Add and make visible all child components
    addAndMakeVisible(topBarComponent); // Use the specific component
    addAndMakeVisible(leftSidebar);
    addAndMakeVisible(mainEditor);
    addAndMakeVisible(bottomPanel);

    // Apply basic dark theme background colors as per design doc
    // topBar.setColour(juce::Component::backgroundColourId, juce::Colours::darkgrey.darker(0.8f)); // Handled by TopBarComponent
    // leftSidebar background is handled by SidebarComponent::paint
    bottomPanel.setColour(juce::Component::backgroundColourId, juce::Colours::darkgrey.darker(0.8f));
    mainEditor.setColour(juce::Component::backgroundColourId, juce::Colours::black); // Main workspace is black

    // Set initial size for the main window
    setSize (1000, 700);
}

MainComponent::~MainComponent()
{
    // Destructor
}

//==============================================================================
void MainComponent::paint (juce::Graphics&amp; g)
{
    // Fill the background of the main component itself (optional, as children cover it)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

void MainComponent::resized()
{
    // This method lays out the child components within MainComponent's bounds

    auto bounds = getLocalBounds();

    // Define heights/widths for the panels
    const int topBarHeight = 30;
    const int leftSidebarWidth = 180;
    const int bottomPanelHeight = 60;

    // Layout Top Bar
    topBarComponent.setBounds(bounds.removeFromTop(topBarHeight)); // Use the specific component

    // Layout Left Sidebar
    leftSidebar.setBounds(bounds.removeFromLeft(leftSidebarWidth));

    // Layout Bottom Panel (containing transport controls)
    bottomPanel.setBounds(bounds.removeFromBottom(bottomPanelHeight));

    // The remaining area is for the Main Editor (main workspace)
    mainEditor.setBounds(bounds);
}