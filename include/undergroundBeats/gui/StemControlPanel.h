#pragma once

#include "JuceHeader.h"
#include "WaveformDisplay.h"

namespace undergroundBeats {

/**
 * A component that groups a WaveformDisplay with controls
 * for a single audio stem.
 */
class StemControlPanel : public juce::Component
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
        addAndMakeVisible(gainSlider);

        // Solo button - make more prominent
        soloButton.setButtonText("S");
        soloButton.setTooltip("Solo");
        soloButton.setColour(juce::TextButton::buttonColourId, juce::Colours::darkgreen);
        soloButton.setColour(juce::TextButton::buttonOnColourId, juce::Colours::green);
        soloButton.setColour(juce::TextButton::textColourOffId, juce::Colours::white);
        addAndMakeVisible(soloButton);

        // Mute button - make more prominent
        muteButton.setButtonText("M");
        muteButton.setTooltip("Mute");
        muteButton.setColour(juce::TextButton::buttonColourId, juce::Colours::darkred);
        muteButton.setColour(juce::TextButton::buttonOnColourId, juce::Colours::red);
        muteButton.setColour(juce::TextButton::textColourOffId, juce::Colours::white);
        addAndMakeVisible(muteButton);

        // Zoom slider - make more intuitive
        zoomSlider.setRange(1.0, 20.0, 0.1);
        zoomSlider.setValue(1.0);
        zoomSlider.setSliderStyle(juce::Slider::LinearHorizontal);
        zoomSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 50, 20);
        zoomSlider.setColour(juce::Slider::thumbColourId, stemColor);
        zoomSlider.setTooltip("Zoom");
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

private:
    juce::String stemName;
    juce::Colour stemColor;

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