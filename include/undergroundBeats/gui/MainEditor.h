#pragma once

// Use the central JUCE header
#include "JuceHeader.h"
#include "TransportControls.h" // Include the new TransportControls header
#include "StemControlPanel.h"  // Include StemControlPanel header
#include "SidebarComponent.h"  // Include SidebarComponent
#include "TopBarComponent.h"   // Include TopBarComponent
#include "panels/EQPanelComponent.h"
#include "panels/CompressorPanelComponent.h"
#include "panels/StyleTransferPanelComponent.h"
#include "panels/VariationExplorerComponent.h"
#include <vector>              // Include for std::vector
#include <memory>              // Include for std::unique_ptr

// Forward declarations
namespace undergroundBeats {
    class UndergroundBeatsProcessor;

    /**
     * The main editor component for the UndergroundBeats application.
     * Manages the overall UI layout and interactions.
     */
    class MainEditor : public juce::AudioProcessorEditor
    {
    public:
        MainEditor(UndergroundBeatsProcessor& processor);
        ~MainEditor() override;

        /** Called when the editor is resized */
        void resized() override;
        
        /** Called to paint the editor */
        void paint(juce::Graphics& g) override;

    private:
        /** Reference to the processor */
        UndergroundBeatsProcessor& audioProcessor;

        /** Transport controls for playback */
        TransportControls transportControls;

        /** The sidebar containing samples and presets */
        SidebarComponent sidebar;

        /** The top bar with file controls and settings */
        TopBarComponent topBar;

        /** Vector of stem control panels for each stem */
        std::vector<std::unique_ptr<StemControlPanel>> stemPanels;
        
        /** EQ panel component for frequency adjustment */
        EQPanelComponent eqPanel;
        
        /** Compressor panel for dynamics processing */
        CompressorPanelComponent compressorPanel;
        
        /** Style transfer panel for audio style transfer */
        StyleTransferPanelComponent styleTransferPanel;
        
        /** Variation explorer for managing variations */
        VariationExplorerComponent variationExplorer;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainEditor)
    };
}
