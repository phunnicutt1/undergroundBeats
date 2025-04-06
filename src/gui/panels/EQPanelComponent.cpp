#include "../../../include/undergroundBeats/gui/panels/EQPanelComponent.h"
#include "JuceHeader.h"

namespace undergroundBeats {

EQPanelComponent::EQPanelComponent()
{
    // Set up title
    titleLabel.setText("EQ Panel", juce::dontSendNotification);
    titleLabel.setFont(juce::Font(20.0f, juce::Font::bold));
    titleLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(titleLabel);

    // Set up stem selector
    stemSelector.addItem("Drums", 1);
    stemSelector.addItem("Bass", 2);
    stemSelector.addItem("Vocals", 3);
    stemSelector.addItem("Other", 4);
    stemSelector.setSelectedId(1, juce::dontSendNotification);
    stemSelector.onChange = [this] { stemSelectionChanged(stemSelector.getSelectedId() - 1); };
    addAndMakeVisible(stemSelector);

    // Set up band 1 (Low)
    band1EnableButton.setButtonText("Low Band");
    band1EnableButton.setToggleState(true, juce::dontSendNotification);
    band1EnableButton.addListener(this);
    addAndMakeVisible(band1EnableButton);

    setupSlider(band1FreqSlider, 20.0, 500.0, 100.0, " Hz");
    band1FreqSlider.addListener(this);
    addAndMakeVisible(band1FreqSlider);
    band1FreqLabel.setText("Frequency", juce::dontSendNotification);
    band1FreqLabel.attachToComponent(&band1FreqSlider, true);
    addAndMakeVisible(band1FreqLabel);

    setupSlider(band1GainSlider, -24.0, 24.0, 0.0, " dB");
    band1GainSlider.addListener(this);
    addAndMakeVisible(band1GainSlider);
    band1GainLabel.setText("Gain", juce::dontSendNotification);
    band1GainLabel.attachToComponent(&band1GainSlider, true);
    addAndMakeVisible(band1GainLabel);

    setupSlider(band1QSlider, 0.1, 10.0, 1.0, "");
    band1QSlider.addListener(this);
    addAndMakeVisible(band1QSlider);
    band1QLabel.setText("Q", juce::dontSendNotification);
    band1QLabel.attachToComponent(&band1QSlider, true);
    addAndMakeVisible(band1QLabel);

    // Set up band 2 (Mid)
    band2EnableButton.setButtonText("Mid Band");
    band2EnableButton.setToggleState(true, juce::dontSendNotification);
    band2EnableButton.addListener(this);
    addAndMakeVisible(band2EnableButton);

    setupSlider(band2FreqSlider, 200.0, 5000.0, 1000.0, " Hz");
    band2FreqSlider.addListener(this);
    addAndMakeVisible(band2FreqSlider);
    band2FreqLabel.setText("Frequency", juce::dontSendNotification);
    band2FreqLabel.attachToComponent(&band2FreqSlider, true);
    addAndMakeVisible(band2FreqLabel);

    setupSlider(band2GainSlider, -24.0, 24.0, 0.0, " dB");
    band2GainSlider.addListener(this);
    addAndMakeVisible(band2GainSlider);
    band2GainLabel.setText("Gain", juce::dontSendNotification);
    band2GainLabel.attachToComponent(&band2GainSlider, true);
    addAndMakeVisible(band2GainLabel);

    setupSlider(band2QSlider, 0.1, 10.0, 1.0, "");
    band2QSlider.addListener(this);
    addAndMakeVisible(band2QSlider);
    band2QLabel.setText("Q", juce::dontSendNotification);
    band2QLabel.attachToComponent(&band2QSlider, true);
    addAndMakeVisible(band2QLabel);

    // Set up band 3 (High)
    band3EnableButton.setButtonText("High Band");
    band3EnableButton.setToggleState(true, juce::dontSendNotification);
    band3EnableButton.addListener(this);
    addAndMakeVisible(band3EnableButton);

    setupSlider(band3FreqSlider, 2000.0, 20000.0, 5000.0, " Hz");
    band3FreqSlider.addListener(this);
    addAndMakeVisible(band3FreqSlider);
    band3FreqLabel.setText("Frequency", juce::dontSendNotification);
    band3FreqLabel.attachToComponent(&band3FreqSlider, true);
    addAndMakeVisible(band3FreqLabel);

    setupSlider(band3GainSlider, -24.0, 24.0, 0.0, " dB");
    band3GainSlider.addListener(this);
    addAndMakeVisible(band3GainSlider);
    band3GainLabel.setText("Gain", juce::dontSendNotification);
    band3GainLabel.attachToComponent(&band3GainSlider, true);
    addAndMakeVisible(band3GainLabel);

    setupSlider(band3QSlider, 0.1, 10.0, 1.0, "");
    band3QSlider.addListener(this);
    addAndMakeVisible(band3QSlider);
    band3QLabel.setText("Q", juce::dontSendNotification);
    band3QLabel.attachToComponent(&band3QSlider, true);
    addAndMakeVisible(band3QLabel);

    setSize(600, 400);
}

EQPanelComponent::~EQPanelComponent()
{
    // Remove listeners
    band1EnableButton.removeListener(this);
    band1FreqSlider.removeListener(this);
    band1GainSlider.removeListener(this);
    band1QSlider.removeListener(this);
    
    band2EnableButton.removeListener(this);
    band2FreqSlider.removeListener(this);
    band2GainSlider.removeListener(this);
    band2QSlider.removeListener(this);
    
    band3EnableButton.removeListener(this);
    band3FreqSlider.removeListener(this);
    band3GainSlider.removeListener(this);
    band3QSlider.removeListener(this);
}

void EQPanelComponent::paint(juce::Graphics& g)
{
    // Background
    g.fillAll(juce::Colours::darkgrey.darker(0.8f));
    
    // Draw EQ response graph area
    g.setColour(juce::Colours::black);
    g.fillRect(graphArea);
    
    g.setColour(juce::Colours::white);
    g.drawRect(graphArea, 1);
    
    // Draw frequency grid lines (simplified)
    g.setColour(juce::Colours::grey.withAlpha(0.5f));
    
    // Draw horizontal lines
    float graphHeight = graphArea.getHeight();
    int numLines = 5;
    for (int i = 1; i < numLines; ++i)
    {
        float y = graphArea.getY() + (i * (graphHeight / numLines));
        g.drawLine(graphArea.getX(), y, graphArea.getRight(), y, 1.0f);
    }
    
    // Draw vertical lines (frequency markers)
    float graphWidth = graphArea.getWidth();
    int numFreqLines = 9;
    for (int i = 0; i < numFreqLines; ++i)
    {
        float x = graphArea.getX() + (i * (graphWidth / (numFreqLines - 1)));
        g.drawLine(x, graphArea.getY(), x, graphArea.getBottom(), 1.0f);
    }
    
    // Draw frequency labels
    g.setFont(12.0f);
    const char* freqLabels[] = {"20", "50", "100", "200", "500", "1k", "2k", "5k", "20k"};
    for (int i = 0; i < numFreqLines; ++i)
    {
        float x = graphArea.getX() + (i * (graphWidth / (numFreqLines - 1)));
        g.drawText(freqLabels[i], (int)x - 10, graphArea.getBottom() + 2, 20, 20, 
                   juce::Justification::centred, false);
    }
    
    // Placeholder for EQ response curve
    g.setColour(juce::Colours::orange);
    juce::Path curvePath;
    curvePath.startNewSubPath(graphArea.getX(), graphArea.getCentreY());
    
    for (int x = 0; x < graphWidth; ++x)
    {
        float freq = 20.0f * std::pow(1000.0f, x / (float)graphWidth);
        float response = 0.0f;
        
        // Simplified peak filter response for visualization
        if (band1EnableButton.getToggleState())
        {
            float band1Freq = (float)band1FreqSlider.getValue();
            float band1Gain = (float)band1GainSlider.getValue();
            float band1Q = (float)band1QSlider.getValue();
            response += calculatePeakResponse(freq, band1Freq, band1Gain, band1Q);
        }
        
        if (band2EnableButton.getToggleState())
        {
            float band2Freq = (float)band2FreqSlider.getValue();
            float band2Gain = (float)band2GainSlider.getValue();
            float band2Q = (float)band2QSlider.getValue();
            response += calculatePeakResponse(freq, band2Freq, band2Gain, band2Q);
        }
        
        if (band3EnableButton.getToggleState())
        {
            float band3Freq = (float)band3FreqSlider.getValue();
            float band3Gain = (float)band3GainSlider.getValue();
            float band3Q = (float)band3QSlider.getValue();
            response += calculatePeakResponse(freq, band3Freq, band3Gain, band3Q);
        }
        
        // Map response from dB to graph height
        float y = graphArea.getCentreY() - (response * graphHeight / 48.0f);
        y = juce::jlimit(graphArea.getY() + 2.0f, graphArea.getBottom() - 2.0f, y);
        
        curvePath.lineTo(graphArea.getX() + x, y);
    }
    
    g.strokePath(curvePath, juce::PathStrokeType(2.0f));
}

// Simplified peak filter response calculation for visualization
float EQPanelComponent::calculatePeakResponse(float freq, float centerFreq, float gainDb, float q)
{
    float ratio = freq / centerFreq;
    float response = gainDb / (1.0f + q * std::abs(std::log10(ratio)));
    return response;
}

void EQPanelComponent::resized()
{
    auto bounds = getLocalBounds().reduced(10);
    
    // Top section - title and stem selector
    auto topSection = bounds.removeFromTop(40);
    titleLabel.setBounds(topSection.removeFromLeft(topSection.getWidth() / 2).reduced(5));
    stemSelector.setBounds(topSection.reduced(5));
    
    bounds.removeFromTop(10); // Spacing
    
    // Graph area takes approximately 1/3 of the remaining height
    graphArea = bounds.removeFromTop(bounds.getHeight() / 3);
    
    bounds.removeFromTop(20); // Spacing
    
    // Remaining area for controls
    auto controlsArea = bounds;
    
    // Three columns for each EQ band
    int columnWidth = controlsArea.getWidth() / 3;
    
    // Band 1 controls
    auto band1Area = controlsArea.removeFromLeft(columnWidth).reduced(5);
    band1EnableButton.setBounds(band1Area.removeFromTop(30));
    band1Area.removeFromTop(10); // Spacing
    
    auto sliderHeight = (band1Area.getHeight() - 20) / 3; // Equal height for 3 sliders
    
    band1FreqSlider.setBounds(band1Area.removeFromTop(sliderHeight).withTrimmedLeft(70));
    band1Area.removeFromTop(10); // Spacing
    
    band1GainSlider.setBounds(band1Area.removeFromTop(sliderHeight).withTrimmedLeft(70));
    band1Area.removeFromTop(10); // Spacing
    
    band1QSlider.setBounds(band1Area.removeFromTop(sliderHeight).withTrimmedLeft(70));
    
    // Band 2 controls
    auto band2Area = controlsArea.removeFromLeft(columnWidth).reduced(5);
    band2EnableButton.setBounds(band2Area.removeFromTop(30));
    band2Area.removeFromTop(10); // Spacing
    
    band2FreqSlider.setBounds(band2Area.removeFromTop(sliderHeight).withTrimmedLeft(70));
    band2Area.removeFromTop(10); // Spacing
    
    band2GainSlider.setBounds(band2Area.removeFromTop(sliderHeight).withTrimmedLeft(70));
    band2Area.removeFromTop(10); // Spacing
    
    band2QSlider.setBounds(band2Area.removeFromTop(sliderHeight).withTrimmedLeft(70));
    
    // Band 3 controls
    auto band3Area = controlsArea.reduced(5);
    band3EnableButton.setBounds(band3Area.removeFromTop(30));
    band3Area.removeFromTop(10); // Spacing
    
    band3FreqSlider.setBounds(band3Area.removeFromTop(sliderHeight).withTrimmedLeft(70));
    band3Area.removeFromTop(10); // Spacing
    
    band3GainSlider.setBounds(band3Area.removeFromTop(sliderHeight).withTrimmedLeft(70));
    band3Area.removeFromTop(10); // Spacing
    
    band3QSlider.setBounds(band3Area.removeFromTop(sliderHeight).withTrimmedLeft(70));
} 

void EQPanelComponent::setProcessorAndStem(UndergroundBeatsProcessor* processor, int stemIndex)
{
    processorRef = processor;
    currentStemIndex = stemIndex;
    stemSelector.setSelectedId(stemIndex + 1, juce::dontSendNotification);
    
    if (processorRef != nullptr)
    {
        updateUIFromProcessor();
    }
}

void EQPanelComponent::stemSelectionChanged(int newStemIndex)
{
    if (newStemIndex >= 0 && newStemIndex < 4)
    {
        currentStemIndex = newStemIndex;
        updateUIFromProcessor();
    }
}

void EQPanelComponent::buttonClicked(juce::Button* button)
{
    if (processorRef == nullptr)
        return;
    
    if (button == &band1EnableButton)
    {
        processorRef->getValueTreeState().getParameter(
            UndergroundBeatsProcessor::getStemParameterID(currentStemIndex, "EQ1_Enable"))
            ->setValueNotifyingHost(button->getToggleState() ? 1.0f : 0.0f);
    }
    else if (button == &band2EnableButton)
    {
        processorRef->getValueTreeState().getParameter(
            UndergroundBeatsProcessor::getStemParameterID(currentStemIndex, "EQ2_Enable"))
            ->setValueNotifyingHost(button->getToggleState() ? 1.0f : 0.0f);
    }
    else if (button == &band3EnableButton)
    {
        processorRef->getValueTreeState().getParameter(
            UndergroundBeatsProcessor::getStemParameterID(currentStemIndex, "EQ3_Enable"))
            ->setValueNotifyingHost(button->getToggleState() ? 1.0f : 0.0f);
    }
    
    repaint(); // Update the EQ curve visualization
}

void EQPanelComponent::sliderValueChanged(juce::Slider* slider)
{
    if (processorRef == nullptr)
        return;
    
    // Band 1 sliders
    if (slider == &band1FreqSlider)
    {
        processorRef->getValueTreeState().getParameter(
            UndergroundBeatsProcessor::getStemParameterID(currentStemIndex, "EQ1_Freq"))
            ->setValueNotifyingHost(processorRef->getValueTreeState().getParameterRange(
                UndergroundBeatsProcessor::getStemParameterID(currentStemIndex, "EQ1_Freq"))
                .convertTo0to1(slider->getValue()));
    }
    else if (slider == &band1GainSlider)
    {
        processorRef->getValueTreeState().getParameter(
            UndergroundBeatsProcessor::getStemParameterID(currentStemIndex, "EQ1_Gain"))
            ->setValueNotifyingHost(processorRef->getValueTreeState().getParameterRange(
                UndergroundBeatsProcessor::getStemParameterID(currentStemIndex, "EQ1_Gain"))
                .convertTo0to1(slider->getValue()));
    }
    else if (slider == &band1QSlider)
    {
        processorRef->getValueTreeState().getParameter(
            UndergroundBeatsProcessor::getStemParameterID(currentStemIndex, "EQ1_Q"))
            ->setValueNotifyingHost(processorRef->getValueTreeState().getParameterRange(
                UndergroundBeatsProcessor::getStemParameterID(currentStemIndex, "EQ1_Q"))
                .convertTo0to1(slider->getValue()));
    }
    
    // Band 2 sliders
    else if (slider == &band2FreqSlider)
    {
        processorRef->getValueTreeState().getParameter(
            UndergroundBeatsProcessor::getStemParameterID(currentStemIndex, "EQ2_Freq"))
            ->setValueNotifyingHost(processorRef->getValueTreeState().getParameterRange(
                UndergroundBeatsProcessor::getStemParameterID(currentStemIndex, "EQ2_Freq"))
                .convertTo0to1(slider->getValue()));
    }
    else if (slider == &band2GainSlider)
    {
        processorRef->getValueTreeState().getParameter(
            UndergroundBeatsProcessor::getStemParameterID(currentStemIndex, "EQ2_Gain"))
            ->setValueNotifyingHost(processorRef->getValueTreeState().getParameterRange(
                UndergroundBeatsProcessor::getStemParameterID(currentStemIndex, "EQ2_Gain"))
                .convertTo0to1(slider->getValue()));
    }
    else if (slider == &band2QSlider)
    {
        processorRef->getValueTreeState().getParameter(
            UndergroundBeatsProcessor::getStemParameterID(currentStemIndex, "EQ2_Q"))
            ->setValueNotifyingHost(processorRef->getValueTreeState().getParameterRange(
                UndergroundBeatsProcessor::getStemParameterID(currentStemIndex, "EQ2_Q"))
                .convertTo0to1(slider->getValue()));
    }
    
    // Band 3 sliders
    else if (slider == &band3FreqSlider)
    {
        processorRef->getValueTreeState().getParameter(
            UndergroundBeatsProcessor::getStemParameterID(currentStemIndex, "EQ3_Freq"))
            ->setValueNotifyingHost(processorRef->getValueTreeState().getParameterRange(
                UndergroundBeatsProcessor::getStemParameterID(currentStemIndex, "EQ3_Freq"))
                .convertTo0to1(slider->getValue()));
    }
    else if (slider == &band3GainSlider)
    {
        processorRef->getValueTreeState().getParameter(
            UndergroundBeatsProcessor::getStemParameterID(currentStemIndex, "EQ3_Gain"))
            ->setValueNotifyingHost(processorRef->getValueTreeState().getParameterRange(
                UndergroundBeatsProcessor::getStemParameterID(currentStemIndex, "EQ3_Gain"))
                .convertTo0to1(slider->getValue()));
    }
    else if (slider == &band3QSlider)
    {
        processorRef->getValueTreeState().getParameter(
            UndergroundBeatsProcessor::getStemParameterID(currentStemIndex, "EQ3_Q"))
            ->setValueNotifyingHost(processorRef->getValueTreeState().getParameterRange(
                UndergroundBeatsProcessor::getStemParameterID(currentStemIndex, "EQ3_Q"))
                .convertTo0to1(slider->getValue()));
    }
    
    repaint(); // Update the EQ curve visualization
}

void EQPanelComponent::updateUIFromProcessor()
{
    if (processorRef == nullptr)
        return;
    
    auto& vts = processorRef->getValueTreeState();
    
    // Band 1
    auto* band1EnableParam = vts.getParameter(UndergroundBeatsProcessor::getStemParameterID(currentStemIndex, "EQ1_Enable"));
    auto* band1FreqParam = vts.getParameter(UndergroundBeatsProcessor::getStemParameterID(currentStemIndex, "EQ1_Freq"));
    auto* band1GainParam = vts.getParameter(UndergroundBeatsProcessor::getStemParameterID(currentStemIndex, "EQ1_Gain"));
    auto* band1QParam = vts.getParameter(UndergroundBeatsProcessor::getStemParameterID(currentStemIndex, "EQ1_Q"));
    
    if (band1EnableParam != nullptr)
        band1EnableButton.setToggleState(band1EnableParam->getValue() > 0.5f, juce::dontSendNotification);
    
    if (band1FreqParam != nullptr)
        band1FreqSlider.setValue(vts.getParameterRange(
            UndergroundBeatsProcessor::getStemParameterID(currentStemIndex, "EQ1_Freq"))
            .convertFrom0to1(band1FreqParam->getValue()), juce::dontSendNotification);
    
    if (band1GainParam != nullptr)
        band1GainSlider.setValue(vts.getParameterRange(
            UndergroundBeatsProcessor::getStemParameterID(currentStemIndex, "EQ1_Gain"))
            .convertFrom0to1(band1GainParam->getValue()), juce::dontSendNotification);
    
    if (band1QParam != nullptr)
        band1QSlider.setValue(vts.getParameterRange(
            UndergroundBeatsProcessor::getStemParameterID(currentStemIndex, "EQ1_Q"))
            .convertFrom0to1(band1QParam->getValue()), juce::dontSendNotification);
    
    // Band 2
    auto* band2EnableParam = vts.getParameter(UndergroundBeatsProcessor::getStemParameterID(currentStemIndex, "EQ2_Enable"));
    auto* band2FreqParam = vts.getParameter(UndergroundBeatsProcessor::getStemParameterID(currentStemIndex, "EQ2_Freq"));
    auto* band2GainParam = vts.getParameter(UndergroundBeatsProcessor::getStemParameterID(currentStemIndex, "EQ2_Gain"));
    auto* band2QParam = vts.getParameter(UndergroundBeatsProcessor::getStemParameterID(currentStemIndex, "EQ2_Q"));
    
    if (band2EnableParam != nullptr)
        band2EnableButton.setToggleState(band2EnableParam->getValue() > 0.5f, juce::dontSendNotification);
    
    if (band2FreqParam != nullptr)
        band2FreqSlider.setValue(vts.getParameterRange(
            UndergroundBeatsProcessor::getStemParameterID(currentStemIndex, "EQ2_Freq"))
            .convertFrom0to1(band2FreqParam->getValue()), juce::dontSendNotification);
    
    if (band2GainParam != nullptr)
        band2GainSlider.setValue(vts.getParameterRange(
            UndergroundBeatsProcessor::getStemParameterID(currentStemIndex, "EQ2_Gain"))
            .convertFrom0to1(band2GainParam->getValue()), juce::dontSendNotification);
    
    if (band2QParam != nullptr)
        band2QSlider.setValue(vts.getParameterRange(
            UndergroundBeatsProcessor::getStemParameterID(currentStemIndex, "EQ2_Q"))
            .convertFrom0to1(band2QParam->getValue()), juce::dontSendNotification);
    
    // Band 3
    auto* band3EnableParam = vts.getParameter(UndergroundBeatsProcessor::getStemParameterID(currentStemIndex, "EQ3_Enable"));
    auto* band3FreqParam = vts.getParameter(UndergroundBeatsProcessor::getStemParameterID(currentStemIndex, "EQ3_Freq"));
    auto* band3GainParam = vts.getParameter(UndergroundBeatsProcessor::getStemParameterID(currentStemIndex, "EQ3_Gain"));
    auto* band3QParam = vts.getParameter(UndergroundBeatsProcessor::getStemParameterID(currentStemIndex, "EQ3_Q"));
    
    if (band3EnableParam != nullptr)
        band3EnableButton.setToggleState(band3EnableParam->getValue() > 0.5f, juce::dontSendNotification);
    
    if (band3FreqParam != nullptr)
        band3FreqSlider.setValue(vts.getParameterRange(
            UndergroundBeatsProcessor::getStemParameterID(currentStemIndex, "EQ3_Freq"))
            .convertFrom0to1(band3FreqParam->getValue()), juce::dontSendNotification);
    
    if (band3GainParam != nullptr)
        band3GainSlider.setValue(vts.getParameterRange(
            UndergroundBeatsProcessor::getStemParameterID(currentStemIndex, "EQ3_Gain"))
            .convertFrom0to1(band3GainParam->getValue()), juce::dontSendNotification);
    
    if (band3QParam != nullptr)
        band3QSlider.setValue(vts.getParameterRange(
            UndergroundBeatsProcessor::getStemParameterID(currentStemIndex, "EQ3_Q"))
            .convertFrom0to1(band3QParam->getValue()), juce::dontSendNotification);
    
    repaint(); // Update the EQ curve visualization
}

void EQPanelComponent::setupSlider(juce::Slider& slider, double minValue, double maxValue, 
                                  double defaultValue, const juce::String& suffix)
{
    slider.setRange(minValue, maxValue);
    slider.setValue(defaultValue, juce::dontSendNotification);
    slider.setTextValueSuffix(suffix);
    slider.setSliderStyle(juce::Slider::LinearHorizontal);
    slider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 60, 20);
}

} // namespace undergroundBeats 