#pragma once

#include "JuceHeader.h"
#include "../../UndergroundBeatsProcessor.h"

namespace undergroundBeats {

//==============================================================================
/**
 * A component that displays an EQ interface with frequency response visualization
 * and parameter controls for a specific stem.
 */
class EQPanelComponent : public juce::Component,
                         private juce::Button::Listener,
                         private juce::Slider::Listener
{
public:
    EQPanelComponent();
    ~EQPanelComponent() override;

    void paint(juce::Graphics&) override;
    void resized() override;
    
    /** Sets the processor and stem index to control. */
    void setProcessorAndStem(UndergroundBeatsProcessor* processor, int stemIndex);
    
    /** Called when stem selection changes. */
    void stemSelectionChanged(int newStemIndex);

private:
    // Listener implementations
    void buttonClicked(juce::Button* button) override;
    void sliderValueChanged(juce::Slider* slider) override;
    
    // Helper method for EQ visualization
    float calculatePeakResponse(float freq, float centerFreq, float gainDb, float q);
    
    // References to processor and stem index
    UndergroundBeatsProcessor* processorRef = nullptr;
    int currentStemIndex = 0;
    
    // UI Components
    juce::Label titleLabel;
    juce::ComboBox stemSelector;
    
    // EQ Band 1 (Low)
    juce::ToggleButton band1EnableButton;
    juce::Slider band1FreqSlider;
    juce::Slider band1GainSlider;
    juce::Slider band1QSlider;
    juce::Label band1FreqLabel;
    juce::Label band1GainLabel;
    juce::Label band1QLabel;
    
    // EQ Band 2 (Mid)
    juce::ToggleButton band2EnableButton;
    juce::Slider band2FreqSlider;
    juce::Slider band2GainSlider;
    juce::Slider band2QSlider;
    juce::Label band2FreqLabel;
    juce::Label band2GainLabel;
    juce::Label band2QLabel;
    
    // EQ Band 3 (High)
    juce::ToggleButton band3EnableButton;
    juce::Slider band3FreqSlider;
    juce::Slider band3GainSlider;
    juce::Slider band3QSlider;
    juce::Label band3FreqLabel;
    juce::Label band3GainLabel;
    juce::Label band3QLabel;
    
    // EQ Response Graph (placeholder for now)
    juce::Rectangle<int> graphArea;
    
    // Updates UI from processor parameters
    void updateUIFromProcessor();
    
    // Set up slider properties
    void setupSlider(juce::Slider& slider, double minValue, double maxValue, 
                     double defaultValue, const juce::String& suffix = "");
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EQPanelComponent)
}; 

} // namespace undergroundBeats 