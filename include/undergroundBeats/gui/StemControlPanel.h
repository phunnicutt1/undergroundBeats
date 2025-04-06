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
        nameLabel.setText(stemName, juce::dontSendNotification);
        nameLabel.setColour(juce::Label::textColourId, juce::Colours::white);
        addAndMakeVisible(nameLabel);

        // Configure waveform display with stem color
        waveformDisplay.setWaveformColour(stemColor);
        addAndMakeVisible(waveformDisplay);

        // Volume slider
        volumeLabel.setText("Vol", juce::dontSendNotification);
        volumeLabel.setColour(juce::Label::textColourId, juce::Colours::white);
        volumeLabel.attachToComponent(&volumeSlider, true);
        volumeSlider.setRange(0.0, 1.0, 0.01);
        volumeSlider.setValue(1.0);
        volumeSlider.setSliderStyle(juce::Slider::LinearVertical);
        volumeSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
        addAndMakeVisible(volumeSlider);
        addAndMakeVisible(volumeLabel);

        // Gain slider
        gainLabel.setText("Gain", juce::dontSendNotification);
        gainLabel.setColour(juce::Label::textColourId, juce::Colours::white);
        gainLabel.attachToComponent(&gainSlider, true);
        gainSlider.setRange(-24.0, 24.0, 0.1);
        gainSlider.setValue(0.0);
        gainSlider.setSliderStyle(juce::Slider::LinearVertical);
        gainSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
        addAndMakeVisible(gainSlider);
        addAndMakeVisible(gainLabel);

        // Solo button
        soloButton.setButtonText("S");
        soloButton.setTooltip("Solo");
        addAndMakeVisible(soloButton);

        // Mute button
        muteButton.setButtonText("M");
        muteButton.setTooltip("Mute");
        addAndMakeVisible(muteButton);

        // Zoom slider
        zoomSlider.setRange(1.0, 20.0, 0.1);
        zoomSlider.setValue(1.0);
        zoomSlider.setSliderStyle(juce::Slider::LinearHorizontal);
        zoomSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
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
        g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId).darker(0.1f));
        g.setColour(stemColor.withAlpha(0.5f));
        g.drawRect(getLocalBounds(), 1);
    }

    void resized() override
    {
        auto bounds = getLocalBounds().reduced(4);

        // Top label
        nameLabel.setBounds(bounds.removeFromTop(20));

        // Bottom zoom slider
        zoomSlider.setBounds(bounds.removeFromBottom(20));

        // Right side controls
        auto controlArea = bounds.removeFromRight(80);
        int buttonHeight = 20;
        soloButton.setBounds(controlArea.removeFromTop(buttonHeight).reduced(2));
        muteButton.setBounds(controlArea.removeFromTop(buttonHeight).reduced(2));
        volumeSlider.setBounds(controlArea.removeFromTop(bounds.getHeight() / 2).reduced(2));
        gainSlider.setBounds(controlArea.reduced(2));

        // Waveform in remaining area
        waveformDisplay.setBounds(bounds);
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