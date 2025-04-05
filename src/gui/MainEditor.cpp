#include "../../include/undergroundBeats/gui/MainEditor.h"
#include "../../include/undergroundBeats/UndergroundBeatsProcessor.h"
#include "../../include/undergroundBeats/gui/StemControlPanel.h"
#include "JuceHeader.h"

namespace undergroundBeats {

//==============================================================================
MainEditor::MainEditor(UndergroundBeatsProcessor& processor)
    : AudioProcessorEditor(&processor),
      audioProcessor(processor),
      transportControls(),
      sidebar(),
      topBar(),
      eqPanel(),
      compressorPanel(),
      styleTransferPanel(),
      variationExplorer()
{
    // Set initial size
    setSize(800, 600);
    
    // Add components
    addAndMakeVisible(transportControls);
    addAndMakeVisible(sidebar);
    addAndMakeVisible(topBar);
    
    // Create stem panels
    const int initialNumStems = 4;
    for (int i = 0; i < initialNumStems; ++i)
    {
        auto panel = std::make_unique<StemControlPanel>("Stem " + juce::String(i+1));
        addAndMakeVisible(panel.get());
        stemPanels.push_back(std::move(panel));
    }
    
    // Add effect panels
    addAndMakeVisible(eqPanel);
    addAndMakeVisible(compressorPanel);
    addAndMakeVisible(styleTransferPanel);
    
    // Add variation explorer (initially hidden)
    addChildComponent(variationExplorer);
}

MainEditor::~MainEditor()
{
    // Clean up resources
}

void MainEditor::paint(juce::Graphics& g)
{
    // Fill the background
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
    
    // Add border
    g.setColour(juce::Colours::grey);
    g.drawRect(getLocalBounds(), 1);
}

void MainEditor::resized()
{
    auto area = getLocalBounds();
    
    // Top bar
    topBar.setBounds(area.removeFromTop(40));
    
    // Sidebar on the left
    sidebar.setBounds(area.removeFromLeft(200));
    
    // Transport controls at the bottom
    transportControls.setBounds(area.removeFromBottom(50));
    
    // Stem panels in the main area
    auto stemArea = area;
    const int stemHeight = stemArea.getHeight() / juce::jmax(1, (int)stemPanels.size());
    
    for (auto& panel : stemPanels)
    {
        panel->setBounds(stemArea.removeFromTop(stemHeight));
    }
    
    // Effect panels - position them in the center when visible
    auto effectArea = area.reduced(50);
    eqPanel.setBounds(effectArea);
    compressorPanel.setBounds(effectArea);
    styleTransferPanel.setBounds(effectArea);
    
    // Variation explorer covers most of the window when visible
    variationExplorer.setBounds(getLocalBounds().reduced(30));
}

} // namespace undergroundBeats
