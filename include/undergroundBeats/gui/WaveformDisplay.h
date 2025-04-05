#pragma once

#include "JuceHeader.h"

namespace undergroundBeats {

/**
 * WaveformDisplay is a component that displays an audio waveform.
 */
class WaveformDisplay : public juce::Component
{
public:
    WaveformDisplay()
    {
        // Default constructor
    }

    ~WaveformDisplay() override = default;

    void paint(juce::Graphics& g) override
    {
        // Fill background
        g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId).darker(0.2f));

        // Draw border
        g.setColour(juce::Colours::grey);
        g.drawRect(getLocalBounds(), 1);

        // Draw placeholder waveform if no buffer
        if (!audioBuffer)
        {
            g.setColour(juce::Colours::white.withAlpha(0.3f));
            auto bounds = getLocalBounds().toFloat().reduced(2.0f);
            float midY = bounds.getCentreY();
            
            // Draw a sine wave placeholder
            juce::Path path;
            path.startNewSubPath(bounds.getX(), midY);
            
            for (float x = bounds.getX(); x < bounds.getRight(); x += 0.5f)
            {
                float sine = std::sin((x - bounds.getX()) / 20.0f) * 10.0f;
                path.lineTo(x, midY + sine);
            }
            
            g.strokePath(path, juce::PathStrokeType(1.0f));
        }
        else
        {
            // Draw the actual waveform
            g.setColour(juce::Colours::lightgreen);
            drawWaveform(g);
        }
    }

    void resized() override
    {
        // Nothing to do here
    }

    void setAudioBuffer(const juce::AudioBuffer<float>* buffer)
    {
        audioBuffer = buffer;
        repaint();
    }

private:
    void drawWaveform(juce::Graphics& g)
    {
        if (!audioBuffer || audioBuffer->getNumChannels() == 0 || audioBuffer->getNumSamples() == 0)
            return;

        auto bounds = getLocalBounds().toFloat().reduced(2.0f);
        const float midY = bounds.getCentreY();
        const float waveformHeight = bounds.getHeight() * 0.8f;
        
        // Draw waveform for first channel only for simplicity
        const float* samples = audioBuffer->getReadPointer(0);
        const int numSamples = audioBuffer->getNumSamples();
        
        // Create path for waveform
        juce::Path path;
        path.startNewSubPath(bounds.getX(), midY);
        
        const float xScale = bounds.getWidth() / numSamples;
        
        for (int i = 0; i < numSamples; ++i)
        {
            const float x = bounds.getX() + i * xScale;
            const float y = midY - samples[i] * (waveformHeight / 2.0f);
            path.lineTo(x, y);
        }
        
        g.strokePath(path, juce::PathStrokeType(1.0f));
    }

    const juce::AudioBuffer<float>* audioBuffer = nullptr;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WaveformDisplay)
};

} // namespace undergroundBeats