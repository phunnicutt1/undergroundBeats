#pragma once

#include "JuceHeader.h"
#include <vector>
#include <memory>

namespace undergroundBeats
{

// Forward declarations
class UndergroundBeatsProcessor;
class TransportControls;
class SidebarComponent;
class TopBarComponent;
class EQPanelComponent;
class CompressorPanelComponent;
class ReverbPanelComponent;
class DelayPanelComponent;
class ChorusPanelComponent;
class SaturationPanelComponent;
class StyleTransferPanelComponent;
class EffectIconBarComponent;
class StemControlPanel;

//==============================================================================
class MainEditor : public juce::AudioProcessorEditor,
                   private juce::Timer
{
  public:
    MainEditor(UndergroundBeatsProcessor&);
    ~MainEditor() override; // Definition must be in .cpp where SidebarComponent etc. are fully defined

    //==============================================================================
    void resized() override;
    //==============================================================================
    void paint(juce::Graphics& g) override;

  private:
    // Timer callback to check for processor updates
    void timerCallback() override;
    
    // Update the stem displays with the latest audio buffers
    void updateStemDisplays();

    UndergroundBeatsProcessor& processorRef;

    // Unique pointers require full definition visibility in .cpp for destructor
    std::unique_ptr<TransportControls> transportControls;
    std::unique_ptr<SidebarComponent> sidebar;
    std::unique_ptr<TopBarComponent> topBar;
    std::unique_ptr<EQPanelComponent> eqPanel;
    std::unique_ptr<CompressorPanelComponent> compressorPanel;
    std::unique_ptr<ReverbPanelComponent> reverbPanel;
    std::unique_ptr<DelayPanelComponent> delayPanel;
    std::unique_ptr<ChorusPanelComponent> chorusPanel;
    std::unique_ptr<SaturationPanelComponent> saturationPanel;
    std::unique_ptr<StyleTransferPanelComponent> styleTransferPanel;
    std::unique_ptr<EffectIconBarComponent> effectIconBar;

    // Vector of stem control panels for displaying the separated stems
    std::vector<std::unique_ptr<StemControlPanel>> stemPanels;
    
    // Component to hold the stem panels
    std::unique_ptr<juce::Component> stemContainer;

    // Callback functions for effect toggles
    void toggleEQPanel();
    void toggleCompressorPanel();
    void toggleReverbPanel();
    void toggleDelayPanel();
    void toggleChorusPanel();
    void toggleSaturationPanel();
    void toggleStyleTransferPanel();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainEditor)
};

} // namespace undergroundBeats
