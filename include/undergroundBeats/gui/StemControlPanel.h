#pragma once

#include "JuceHeader.h"
#include "WaveformDisplay.h"
#include "../UndergroundBeatsProcessor.h"

namespace undergroundBeats {

/**
 * A component that groups a WaveformDisplay with controls
 * for a single audio stem.
 */
class StemControlPanel : public juce::Component,
                         private juce::Button::Listener,
                         private juce::Slider::Listener
{
public:
    /**
     * Constructor.
     * @param name The name of the stem.
     * @param color The color to use for waveform and accents.
     */
    StemControlPanel(const juce::String& name = "Stem", juce::Colour color = juce::Colours::lightgreen)
        : stemName(name), stemColor(color)
    {
        // Configure the name label with more emphasis
        nameLabel.setText(stemName, juce::dontSendNotification);
        nameLabel.setColour(juce::Label::textColourId, juce::Colours::white);
        nameLabel.setColour(juce::Label::backgroundColourId, stemColor.withAlpha(0.3f));
        nameLabel.setJustificationType(juce::Justification::centred);
        nameLabel.setFont(juce::Font(16.0f, juce::Font::bold));
        addAndMakeVisible(nameLabel);

        // Configure waveform display with stem color
        waveformDisplay.setWaveformColour(stemColor);
        addAndMakeVisible(waveformDisplay);

        // Volume slider - make more prominent
        volumeLabel.setText("Vol", juce::dontSendNotification);
        volumeLabel.setColour(juce::Label::textColourId, juce::Colours::white);
        volumeLabel.setJustificationType(juce::Justification::centred);
        volumeLabel.setFont(juce::Font(12.0f, juce::Font::bold));
        addAndMakeVisible(volumeLabel);
        
        volumeSlider.setRange(0.0, 1.0, 0.01);
        volumeSlider.setValue(1.0);
        volumeSlider.setSliderStyle(juce::Slider::LinearVertical);
        volumeSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
        volumeSlider.setColour(juce::Slider::thumbColourId, stemColor);
        volumeSlider.setColour(juce::Slider::trackColourId, stemColor.withAlpha(0.5f));
        volumeSlider.addListener(this);
        addAndMakeVisible(volumeSlider);

        // Gain slider - make more prominent
        gainLabel.setText("Gain", juce::dontSendNotification);
        gainLabel.setColour(juce::Label::textColourId, juce::Colours::white);
        gainLabel.setJustificationType(juce::Justification::centred);
        gainLabel.setFont(juce::Font(12.0f, juce::Font::bold));
        addAndMakeVisible(gainLabel);
        
        gainSlider.setRange(-24.0, 24.0, 0.1);
        gainSlider.setValue(0.0);
        gainSlider.setSliderStyle(juce::Slider::LinearVertical);
        gainSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
        gainSlider.setColour(juce::Slider::thumbColourId, stemColor);
        gainSlider.setColour(juce::Slider::trackColourId, stemColor.withAlpha(0.5f));
        gainSlider.addListener(this);
        addAndMakeVisible(gainSlider);

        // Solo button - make more prominent
        soloButton.setButtonText("S");
        soloButton.setTooltip("Solo");
        soloButton.setColour(juce::TextButton::buttonColourId, juce::Colours::darkgreen);
        soloButton.setColour(juce::TextButton::buttonOnColourId, juce::Colours::green);
        soloButton.setColour(juce::TextButton::textColourOffId, juce::Colours::white);
        soloButton.addListener(this);
        addAndMakeVisible(soloButton);

        // Mute button - make more prominent
        muteButton.setButtonText("M");
        muteButton.setTooltip("Mute");
        muteButton.setColour(juce::TextButton::buttonColourId, juce::Colours::darkred);
        muteButton.setColour(juce::TextButton::buttonOnColourId, juce::Colours::red);
        muteButton.setColour(juce::TextButton::textColourOffId, juce::Colours::white);
        muteButton.addListener(this);
        addAndMakeVisible(muteButton);

        // Zoom slider - make more intuitive
        zoomSlider.setRange(1.0, 20.0, 0.1);
        zoomSlider.setValue(1.0);
        zoomSlider.setSliderStyle(juce::Slider::LinearHorizontal);
        zoomSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 50, 20);
        zoomSlider.setColour(juce::Slider::thumbColourId, stemColor);
        zoomSlider.setTooltip("Zoom");
        zoomSlider.addListener(this);
        addAndMakeVisible(zoomSlider);

        // Accessibility (to be expanded)
        nameLabel.setAccessible(true);
        volumeSlider.setAccessible(true);
        gainSlider.setAccessible(true);
        soloButton.setAccessible(true);
        muteButton.setAccessible(true);
        zoomSlider.setAccessible(true);
    }

    /** Destructor. */
    ~StemControlPanel() override = default;

    void paint(juce::Graphics& g) override
    {
        // Custom background with color theme
        g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId).darker(0.2f));
        
        // Highlighted border around the panel
        g.setColour(stemColor.withAlpha(0.7f));
        g.drawRect(getLocalBounds(), 2);
        
        // Draw control area background
        auto controlAreaBounds = getLocalBounds().reduced(4);
        controlAreaBounds.removeFromTop(30); // Skip the name label
        controlAreaBounds.removeFromBottom(30); // Skip the zoom area
        auto rightControls = controlAreaBounds.removeFromRight(160);
        g.setColour(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId).darker(0.3f));
        g.fillRect(rightControls);
        g.setColour(stemColor.withAlpha(0.5f));
        g.drawRect(rightControls, 1);
    }

    void resized() override
    {
        auto bounds = getLocalBounds().reduced(5);

        // Top label with more space
        nameLabel.setBounds(bounds.removeFromTop(30).reduced(1));

        // Bottom zoom area
        auto zoomArea = bounds.removeFromBottom(30);
        zoomSlider.setBounds(zoomArea.reduced(5, 0));

        // Right side controls - now wider with better layout
        auto controlArea = bounds.removeFromRight(160);
        controlArea = controlArea.reduced(5);
        
        // Top section for buttons
        auto buttonArea = controlArea.removeFromTop(60);
        
        // Position solo and mute buttons side by side
        int buttonHeight = 30;
        muteButton.setBounds(buttonArea.removeFromLeft(75).withHeight(buttonHeight));
        soloButton.setBounds(buttonArea.withHeight(buttonHeight));
        
        // Position sliders side by side with labels
        auto sliderArea = controlArea;
        auto leftSliderArea = sliderArea.removeFromLeft(75);
        auto rightSliderArea = sliderArea;
        
        // Volume slider on left
        volumeLabel.setBounds(leftSliderArea.removeFromTop(20));
        volumeSlider.setBounds(leftSliderArea);
        
        // Gain slider on right
        gainLabel.setBounds(rightSliderArea.removeFromTop(20));
        gainSlider.setBounds(rightSliderArea);

        // Waveform in remaining area with padding
        waveformDisplay.setBounds(bounds.reduced(5, 0));
    }

    // Update the audio buffer for the waveform display
    void setAudioBuffer(const juce::AudioBuffer<float>* buffer)
    {
        waveformDisplay.setAudioBuffer(buffer);
    }

    // Set zoom factor
    void setZoomFactor(float zoom)
    {
        waveformDisplay.setZoomFactor(zoom);
        zoomSlider.setValue(zoom, juce::dontSendNotification);
    }
    
    // Set the processor and stem index
    void setProcessorAndStem(UndergroundBeatsProcessor* processor, int stemIdx)
    {
        processorRef = processor;
        stemIndex = stemIdx;
        
        if (processorRef != nullptr)
        {
            // Connect to parameters
            updateControlsFromProcessor();
        }
    }
    
    // Update controls from processor parameters
    void updateControlsFromProcessor()
    {
        if (processorRef == nullptr)
            return;
            
        auto& vts = processorRef->getValueTreeState();
        
        // Volume
        auto volumeParamID = UndergroundBeatsProcessor::getStemParameterID(stemIndex, "Volume");
        auto* volumeParam = vts.getParameter(volumeParamID);
        if (volumeParam != nullptr)
            volumeSlider.setValue(volumeParam->getValue(), juce::dontSendNotification);
            
        // Gain
        auto gainParamID = UndergroundBeatsProcessor::getStemParameterID(stemIndex, "Gain");
        auto* gainParam = vts.getParameter(gainParamID);
        if (gainParam != nullptr)
        {
            const auto range = vts.getParameterRange(gainParamID);
            float gainValue = range.convertFrom0to1(gainParam->getValue());
            gainSlider.setValue(gainValue, juce::dontSendNotification);
        }
        
        // Solo
        auto soloParamID = UndergroundBeatsProcessor::getStemParameterID(stemIndex, "Solo");
        auto* soloParam = vts.getParameter(soloParamID);
        if (soloParam != nullptr)
            soloButton.setToggleState(soloParam->getValue() > 0.5f, juce::dontSendNotification);
            
        // Mute
        auto muteParamID = UndergroundBeatsProcessor::getStemParameterID(stemIndex, "Mute");
        auto* muteParam = vts.getParameter(muteParamID);
        if (muteParam != nullptr)
            muteButton.setToggleState(muteParam->getValue() > 0.5f, juce::dontSendNotification);
    }

private:
    // Button listener implementation
    void buttonClicked(juce::Button* button) override
    {
        if (processorRef == nullptr)
            return;
            
        if (button == &soloButton)
        {
            bool isSolo = soloButton.getToggleState();
            auto paramID = UndergroundBeatsProcessor::getStemParameterID(stemIndex, "Solo");
            auto* param = processorRef->getValueTreeState().getParameter(paramID);
            if (param != nullptr)
                param->setValueNotifyingHost(isSolo ? 1.0f : 0.0f);
        }
        else if (button == &muteButton)
        {
            bool isMuted = muteButton.getToggleState();
            auto paramID = UndergroundBeatsProcessor::getStemParameterID(stemIndex, "Mute");
            auto* param = processorRef->getValueTreeState().getParameter(paramID);
            if (param != nullptr)
                param->setValueNotifyingHost(isMuted ? 1.0f : 0.0f);
        }
    }
    
    // Slider listener implementation
    void sliderValueChanged(juce::Slider* slider) override
    {
        if (processorRef == nullptr)
            return;
            
        if (slider == &volumeSlider)
        {
            auto paramID = UndergroundBeatsProcessor::getStemParameterID(stemIndex, "Volume");
            auto* param = processorRef->getValueTreeState().getParameter(paramID);
            if (param != nullptr)
            {
                // We need to convert the slider's value to the normalized range
                const auto range = processorRef->getValueTreeState().getParameterRange(paramID);
                float normalizedValue = range.convertTo0to1((float)slider->getValue());
                param->setValueNotifyingHost(normalizedValue);
                DBG("Setting Volume param " + paramID + " to: " + juce::String(normalizedValue));
            }
        }
        else if (slider == &gainSlider)
        {
            auto paramID = UndergroundBeatsProcessor::getStemParameterID(stemIndex, "Gain");
            auto* param = processorRef->getValueTreeState().getParameter(paramID);
            if (param != nullptr)
            {
                // Convert from dB to 0-1 normalized value
                const auto range = processorRef->getValueTreeState().getParameterRange(paramID);
                // Clamp the value to the valid range first
                float clampedValue = juce::jlimit(range.start, range.end, (float)slider->getValue());
                float normalizedValue = range.convertTo0to1(clampedValue);
                param->setValueNotifyingHost(normalizedValue);
                DBG("Setting Gain param " + paramID + " to normalized: " + juce::String(normalizedValue) 
                    + " (from " + juce::String(clampedValue) + " dB)");
            }
        }
        else if (slider == &zoomSlider)
        {
            waveformDisplay.setZoomFactor((float)slider->getValue());
        }
    }

    juce::String stemName;
    juce::Colour stemColor;
    int stemIndex = 0;
    UndergroundBeatsProcessor* processorRef = nullptr;

    juce::Label nameLabel;
    WaveformDisplay waveformDisplay;

    juce::Slider volumeSlider;
    juce::Label volumeLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> volumeAttachment;

    juce::Slider gainSlider;
    juce::Label gainLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> gainAttachment;

    juce::ToggleButton soloButton;
    juce::ToggleButton muteButton;

    juce::Slider zoomSlider;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(StemControlPanel)
};

} // namespace undergroundBeats