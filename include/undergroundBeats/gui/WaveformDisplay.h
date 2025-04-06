#pragma once

#include "JuceHeader.h"

namespace undergroundBeats {

/**
 * WaveformDisplay is a component that displays an audio waveform.
 */
class WaveformDisplay : public juce::Component
{
public:
    WaveformDisplay() = default;

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
            g.setColour(waveformColour);
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

    void setWaveformColour(juce::Colour colour)
    {
        waveformColour = colour;
        repaint();
    }

    void setZoomFactor(float zoom)
    {
        zoomFactor = zoom;
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
        
        const float* samples = audioBuffer->getReadPointer(0);
        const int totalSamples = audioBuffer->getNumSamples();

        // Calculate number of samples to display based on zoom
        int displaySamples = juce::jmax(1, (int)(totalSamples / zoomFactor));
        int startSample = juce::jlimit(0, totalSamples - displaySamples, 0);

        juce::Path path;
        path.startNewSubPath(bounds.getX(), midY);

        const float xScale = bounds.getWidth() / (float)displaySamples;

        for (int i = 0; i < displaySamples; ++i)
        {
            int sampleIndex = startSample + i;
            if (sampleIndex >= totalSamples)
                break;

            float x = bounds.getX() + i * xScale;
            float y = midY - samples[sampleIndex] * (waveformHeight / 2.0f);
            path.lineTo(x, y);
        }

        g.strokePath(path, juce::PathStrokeType(1.0f));
    }

    const juce::AudioBuffer<float>* audioBuffer = nullptr;
    juce::Colour waveformColour = juce::Colours::lightgreen;
    float zoomFactor = 1.0f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WaveformDisplay)
};

} // namespace undergroundBeats