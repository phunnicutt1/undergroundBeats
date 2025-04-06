// Include own header FIRST
#include "../../include/undergroundBeats/gui/MainEditor.h"

// Then include dependencies
#include "JuceHeader.h"
// Include FULL definitions for unique_ptr members and other interactions
#include "../../include/undergroundBeats/gui/TransportControls.h"
#include "../../include/undergroundBeats/gui/SidebarComponent.h"
#include "../../include/undergroundBeats/gui/TopBarComponent.h"
#include "../../include/undergroundBeats/gui/panels/EQPanelComponent.h"
#include "../../include/undergroundBeats/gui/panels/CompressorPanelComponent.h"
#include "../../include/undergroundBeats/gui/panels/ReverbPanelComponent.h"
#include "../../include/undergroundBeats/gui/panels/DelayPanelComponent.h"
#include "../../include/undergroundBeats/gui/panels/ChorusPanelComponent.h"
#include "../../include/undergroundBeats/gui/panels/SaturationPanelComponent.h"
#include "../../include/undergroundBeats/gui/panels/StyleTransferPanelComponent.h"
#include "../../include/undergroundBeats/gui/EffectIconBarComponent.h"
#include "../../include/undergroundBeats/gui/SampleBrowserComponent.h"
#include "../../include/undergroundBeats/gui/StemControlPanel.h"
#include "../../include/undergroundBeats/UndergroundBeatsProcessor.h"

namespace undergroundBeats
{

//==============================================================================
MainEditor::MainEditor(UndergroundBeatsProcessor& processor)
    : juce::AudioProcessorEditor(&processor),
      processorRef(processor)
{
    // These should now work as full definitions are included
    sidebar = std::make_unique<SidebarComponent>();
    topBar = std::make_unique<TopBarComponent>(processorRef, *sidebar);
    
    // Create transport controls with processor reference
    DBG("MainEditor: Creating TransportControls with processor reference");
    transportControls = std::make_unique<TransportControls>();
    transportControls->setProcessor(&processorRef); // Set direct processor reference
    
    eqPanel = std::make_unique<EQPanelComponent>();
    compressorPanel = std::make_unique<CompressorPanelComponent>();
    reverbPanel = std::make_unique<ReverbPanelComponent>();
    delayPanel = std::make_unique<DelayPanelComponent>();
    chorusPanel = std::make_unique<ChorusPanelComponent>();
    saturationPanel = std::make_unique<SaturationPanelComponent>();
    styleTransferPanel = std::make_unique<StyleTransferPanelComponent>();
    effectIconBar = std::make_unique<EffectIconBarComponent>();
    
    // Create stem container
    stemContainer = std::make_unique<juce::Component>();
    addAndMakeVisible(stemContainer.get());

    setSize(800, 600);

    // Add components - these should now work too
    addAndMakeVisible(transportControls.get());
    addAndMakeVisible(sidebar.get());
    addAndMakeVisible(topBar.get());
    addAndMakeVisible(effectIconBar.get());

    // Connect transport controls to processor using callbacks AS BACKUP
    DBG("MainEditor: Setting up transport control callbacks");
    transportControls->onPlay = [this] { 
        DBG("MainEditor: Play button clicked, calling processor startPlayback()");
        // Stop any preview playback in the sample browser
        if (sidebar && sidebar->getSampleBrowser())
        {
            sidebar->getSampleBrowser()->stopPreview();
            DBG("MainEditor: Stopped sample browser preview");
        }
        processorRef.startPlayback(); 
    };
    transportControls->onPause = [this] { 
        DBG("MainEditor: Pause button clicked, calling processor pausePlayback()");
        processorRef.pausePlayback(); 
    };
    transportControls->onStop = [this] { 
        DBG("MainEditor: Stop button clicked, calling processor stopPlayback()");
        processorRef.stopPlayback(); 
    };
    
    // Verify transport controls are set up
    DBG("MainEditor: Transport controls connected to processor");

    // Add effect panels
    addAndMakeVisible(eqPanel.get());
    eqPanel->setVisible(false);
    addAndMakeVisible(compressorPanel.get());
    compressorPanel->setVisible(false);
    addAndMakeVisible(reverbPanel.get());
    reverbPanel->setVisible(false);
    addAndMakeVisible(delayPanel.get());
    delayPanel->setVisible(false);
    addAndMakeVisible(chorusPanel.get());
    chorusPanel->setVisible(false);
    addAndMakeVisible(saturationPanel.get());
    saturationPanel->setVisible(false);
    addAndMakeVisible(styleTransferPanel.get());
    styleTransferPanel->setVisible(false);

    // Setup callbacks - requires full SidebarComponent & SampleBrowserComponent definitions
    if (sidebar && sidebar->getSampleBrowser())
    {
        sidebar->getSampleBrowser()->onSampleDropped = [this](const juce::File& file) 
        {
            DBG("MainEditor: Sample dropped, triggering processor load for: " + file.getFileName());
            processorRef.loadAudioFile(file);
        };
        sidebar->getSampleBrowser()->onFileChosenForProcessing = [this](const juce::File& file) 
        {
             DBG("MainEditor: File chosen in browser, triggering processor load for: " + file.getFileName());
             processorRef.loadAudioFile(file);
        };
    }

    // Connect effect buttons to toggle functions
    effectIconBar->eqButton.onClick = [this] { toggleEQPanel(); };
    effectIconBar->compButton.onClick = [this] { toggleCompressorPanel(); };
    effectIconBar->reverbButton.onClick = [this] { toggleReverbPanel(); };
    effectIconBar->delayButton.onClick = [this] { toggleDelayPanel(); };
    effectIconBar->chorusButton.onClick = [this] { toggleChorusPanel(); };
    effectIconBar->saturationButton.onClick = [this] { toggleSaturationPanel(); };
    effectIconBar->styleButton.onClick = [this] { toggleStyleTransferPanel(); };
    
    // Start timer to check for processor updates
    startTimerHz(10); // Check 10 times per second
}

MainEditor::~MainEditor()
{
    // Stop timer
    stopTimer();
    
    // Clear stem panels before destroying the editor
    stemPanels.clear();
}

void MainEditor::timerCallback()
{
    // Check if processor parameters have changed (which happens after stems are loaded)
    if (processorRef.getParametersChangedFlag().exchange(false))
    {
        DBG("MainEditor: Processor parameters changed, updating stem displays");
        updateStemDisplays();
    }
    
    // Update transport controls UI based on processor state
    bool isPlaying = processorRef.isPlaying();
    bool isPaused = processorRef.isPaused();
    transportControls->updateState(isPlaying || isPaused, isPaused);
    
    // Update each stem control panel from processor parameters
    for (auto& panel : stemPanels)
    {
        panel->updateControlsFromProcessor();
    }
}

void MainEditor::updateStemDisplays()
{
    // Get the separated stem buffers from the processor
    const auto& stemBuffers = processorRef.getSeparatedStemBuffers();
    
    // If no stems, clear any existing displays
    if (stemBuffers.empty())
    {
        stemPanels.clear();
        resized(); // Trigger layout update
        return;
    }
    
    // Names and colors for the different stems
    const juce::String stemNames[] = { "Drums", "Bass", "Vocals", "Other" };
    const juce::Colour stemColors[] = { juce::Colours::red, juce::Colours::blue, 
                                       juce::Colours::green, juce::Colours::yellow };
    
    // Resize the panel vector to match the number of stems
    int numStems = static_cast<int>(stemBuffers.size());
    
    // Remove excess panels if needed
    while (stemPanels.size() > numStems)
    {
        stemPanels.pop_back();
    }
    
    // Add new panels if needed
    while (stemPanels.size() < numStems)
    {
        int idx = static_cast<int>(stemPanels.size());
        juce::String name = (idx < 4) ? stemNames[idx] : "Stem " + juce::String(idx + 1);
        juce::Colour color = (idx < 4) ? stemColors[idx] : juce::Colours::white;
        
        auto panel = std::make_unique<StemControlPanel>(name, color);
        stemContainer->addAndMakeVisible(panel.get());
        stemPanels.push_back(std::move(panel));
    }
    
    // Update each panel with its corresponding buffer and connect to processor
    for (int i = 0; i < numStems; ++i)
    {
        stemPanels[i]->setAudioBuffer(&stemBuffers[i]);
        stemPanels[i]->setProcessorAndStem(&processorRef, i); // Connect to processor
    }
    
    // Update layout
    resized();
    DBG("MainEditor: Updated " + juce::String(stemPanels.size()) + " stem panels");
}

void MainEditor::paint(juce::Graphics& g)
{
    // (Our component is opaque, so we must fill the background with a solid colour)
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

    g.setColour(juce::Colours::white);
    g.setFont(15.0f);
    // g.drawFittedText ("Hello World!", getLocalBounds(), juce::Justification::centred, 1);
}

void MainEditor::resized()
{
    auto bounds = getLocalBounds();
    int sidebarWidth = 200;
    int topBarHeight = 50;
    int transportHeight = 80;
    int iconBarHeight = 40;

    // Layout main areas - requires full definitions
    topBar->setBounds(bounds.removeFromTop(topBarHeight));
    sidebar->setBounds(bounds.removeFromLeft(sidebarWidth));
    transportControls->setBounds(bounds.removeFromBottom(transportHeight));
    effectIconBar->setBounds(bounds.removeFromTop(iconBarHeight));

    // Main content area now contains stems and effect panels
    stemContainer->setBounds(bounds);
    
    // Layout the stem panels vertically with proper spacing
    int numStems = static_cast<int>(stemPanels.size());
    if (numStems > 0)
    {
        // Calculate height with padding between stems
        int totalPadding = (numStems - 1) * 10; // 10px padding between stems
        int availableHeight = bounds.getHeight() - totalPadding;
        int stemHeight = availableHeight / numStems;
        
        // Position each stem panel using full width
        auto stemBounds = stemContainer->getLocalBounds();
        for (int i = 0; i < numStems; ++i)
        {
            auto panelBounds = stemBounds.removeFromTop(stemHeight);
            if (i < numStems - 1) // Add padding except after the last stem
                stemBounds.removeFromTop(10);
                
            stemPanels[i]->setBounds(panelBounds);
        }
    }

    // Effect panels occupy the same area as stem container
    eqPanel->setBounds(stemContainer->getBounds());
    compressorPanel->setBounds(stemContainer->getBounds());
    reverbPanel->setBounds(stemContainer->getBounds());
    delayPanel->setBounds(stemContainer->getBounds());
    chorusPanel->setBounds(stemContainer->getBounds());
    saturationPanel->setBounds(stemContainer->getBounds());
    styleTransferPanel->setBounds(stemContainer->getBounds());
}

void MainEditor::toggleEQPanel()
{
    bool visible = !eqPanel->isVisible();
    eqPanel->setVisible(visible);
    
    // Connect to processor when shown
    if (visible)
    {
        // Get the selected stem index or default to 0
        int stemIndex = 0; // Default to first stem
        eqPanel->setProcessorAndStem(&processorRef, stemIndex);
    }
    
    compressorPanel->setVisible(false);
    reverbPanel->setVisible(false);
    delayPanel->setVisible(false);
    chorusPanel->setVisible(false);
    saturationPanel->setVisible(false);
    styleTransferPanel->setVisible(false);
}

void MainEditor::toggleCompressorPanel()
{
    bool visible = !compressorPanel->isVisible();
    compressorPanel->setVisible(visible);
    eqPanel->setVisible(false);
    reverbPanel->setVisible(false);
    delayPanel->setVisible(false);
    chorusPanel->setVisible(false);
    saturationPanel->setVisible(false);
    styleTransferPanel->setVisible(false);
}

void MainEditor::toggleReverbPanel()
{
    bool visible = !reverbPanel->isVisible();
    reverbPanel->setVisible(visible);
    eqPanel->setVisible(false);
    compressorPanel->setVisible(false);
    delayPanel->setVisible(false);
    chorusPanel->setVisible(false);
    saturationPanel->setVisible(false);
    styleTransferPanel->setVisible(false);
}

void MainEditor::toggleDelayPanel()
{
    bool visible = !delayPanel->isVisible();
    delayPanel->setVisible(visible);
    eqPanel->setVisible(false);
    compressorPanel->setVisible(false);
    reverbPanel->setVisible(false);
    chorusPanel->setVisible(false);
    saturationPanel->setVisible(false);
    styleTransferPanel->setVisible(false);
}

void MainEditor::toggleChorusPanel()
{
    bool visible = !chorusPanel->isVisible();
    chorusPanel->setVisible(visible);
    eqPanel->setVisible(false);
    compressorPanel->setVisible(false);
    reverbPanel->setVisible(false);
    delayPanel->setVisible(false);
    saturationPanel->setVisible(false);
    styleTransferPanel->setVisible(false);
}

void MainEditor::toggleSaturationPanel()
{
    bool visible = !saturationPanel->isVisible();
    saturationPanel->setVisible(visible);
    eqPanel->setVisible(false);
    compressorPanel->setVisible(false);
    reverbPanel->setVisible(false);
    delayPanel->setVisible(false);
    chorusPanel->setVisible(false);
    styleTransferPanel->setVisible(false);
}

void MainEditor::toggleStyleTransferPanel()
{
    bool visible = !styleTransferPanel->isVisible();
    styleTransferPanel->setVisible(visible);
    eqPanel->setVisible(false);
    compressorPanel->setVisible(false);
    reverbPanel->setVisible(false);
    delayPanel->setVisible(false);
    chorusPanel->setVisible(false);
    saturationPanel->setVisible(false);
}

} // namespace undergroundBeats
