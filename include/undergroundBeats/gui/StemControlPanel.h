#pragma once

#include "JuceHeader.h"
#include "WaveformDisplay.h"

namespace undergroundBeats {

/**
 * A component that groups a WaveformDisplay with Volume and Gain sliders
 * for a single audio stem.
 */
class StemControlPanel : public juce::Component
{
public:
    /**
     * Constructor.
     * @param name The name of the stem.
     */
    StemControlPanel(const juce::String& name = "Stem")
        : stemName(name)
    {
        nameLabel.setText(stemName, juce::dontSendNotification);
        addAndMakeVisible(nameLabel);
        addAndMakeVisible(waveformDisplay);
    }

    /** Destructor. */
    ~StemControlPanel() override = default;

    void paint(juce::Graphics& g) override
    {
        g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId).darker(0.1f));
        g.setColour(juce::Colours::white);
        g.drawRect(getLocalBounds(), 1);
    }

    void resized() override
    {
        auto bounds = getLocalBounds().reduced(2);
        nameLabel.setBounds(bounds.removeFromTop(20));
        waveformDisplay.setBounds(bounds);
    }

    // Update the audio buffer for the waveform display
    void setAudioBuffer(const juce::AudioBuffer<float>* buffer)
    {
        waveformDisplay.setAudioBuffer(buffer);
    }

private:
    juce::String stemName;
    juce::Label nameLabel;
    WaveformDisplay waveformDisplay;

    juce::Slider volumeSlider;
    juce::Label volumeLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> volumeAttachment;

    juce::Slider gainSlider;
    juce::Label gainLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> gainAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(StemControlPanel)
};

} // namespace undergroundBeats