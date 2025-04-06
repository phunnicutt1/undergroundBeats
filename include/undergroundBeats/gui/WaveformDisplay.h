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

        // Draw grid lines
        drawGrid(g);

        // Draw waveform or placeholder
        if (!audioBuffer)
        {
            drawPlaceholder(g);
        }
        else
        {
            // Draw the actual waveform
            g.setColour(waveformColour);
            drawWaveform(g);
        }

        // Draw border
        g.setColour(juce::Colours::grey);
        g.drawRect(getLocalBounds(), 1);
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
        zoomFactor = juce::jlimit(1.0f, 20.0f, zoom);
        repaint();
    }

private:
    void drawGrid(juce::Graphics& g)
    {
        auto bounds = getLocalBounds().toFloat().reduced(2.0f);
        const float midY = bounds.getCentreY();
        
        // Draw center line
        g.setColour(juce::Colours::grey.withAlpha(0.5f));
        g.drawHorizontalLine((int)midY, bounds.getX(), bounds.getRight());
        
        // Draw time markers
        int numMarkers = 10;
        g.setColour(juce::Colours::grey.withAlpha(0.3f));
        for (int i = 1; i < numMarkers; ++i)
        {
            float x = bounds.getX() + (bounds.getWidth() * i) / numMarkers;
            g.drawVerticalLine((int)x, bounds.getY(), bounds.getBottom());
        }
        
        // Draw amplitude markers (at +/- 0.5)
        float halfHeight = bounds.getHeight() / 4.0f;
        g.setColour(juce::Colours::grey.withAlpha(0.2f));
        g.drawHorizontalLine((int)(midY - halfHeight), bounds.getX(), bounds.getRight());
        g.drawHorizontalLine((int)(midY + halfHeight), bounds.getX(), bounds.getRight());
    }
    
    void drawPlaceholder(juce::Graphics& g)
    {
        g.setColour(waveformColour.withAlpha(0.3f));
        auto bounds = getLocalBounds().toFloat().reduced(2.0f);
        float midY = bounds.getCentreY();
        
        // Draw a sine wave placeholder
        juce::Path path;
        path.startNewSubPath(bounds.getX(), midY);
        
        for (float x = bounds.getX(); x < bounds.getRight(); x += 0.5f)
        {
            float sine = std::sin((x - bounds.getX()) / 20.0f) * 15.0f;
            path.lineTo(x, midY + sine);
        }
        
        g.strokePath(path, juce::PathStrokeType(1.0f));
    }

    void drawWaveform(juce::Graphics& g)
    {
        if (!audioBuffer || audioBuffer->getNumSamples() == 0)
            return;

        auto bounds = getLocalBounds().toFloat().reduced(2.0f);
        const float midY = bounds.getCentreY();
        const float waveformHeight = bounds.getHeight() * 0.8f;
        
        // Get number of channels (handle mono or stereo)
        const int numChannels = audioBuffer->getNumChannels();
        const int totalSamples = audioBuffer->getNumSamples();

        // Calculate number of samples to display based on zoom
        int displaySamples = juce::jmax(1, (int)(totalSamples / zoomFactor));
        int startSample = scrollPosition;
        
        // Ensure we don't go out of bounds
        startSample = juce::jlimit(0, juce::jmax(0, totalSamples - displaySamples), startSample);
        
        // If very zoomed in, we need to show fewer samples with more detail
        const float xScale = bounds.getWidth() / (float)juce::jmin(displaySamples, totalSamples - startSample);
        
        // Draw a path for each channel
        for (int channel = 0; channel < numChannels; ++channel)
        {
            const float* samples = audioBuffer->getReadPointer(channel);
            
            // Create RMS path for the waveform
            juce::Path waveformPath;
            waveformPath.startNewSubPath(bounds.getX(), midY);
            
            // Create a separate path for the outline
            juce::Path outlinePath;
            
            // Set the color slightly different for each channel if stereo
            if (numChannels > 1)
            {
                // Left channel more reddish, right channel more bluish
                g.setColour(channel == 0 ? 
                    waveformColour.withHue(waveformColour.getHue() + 0.05f) : 
                    waveformColour.withHue(waveformColour.getHue() - 0.05f));
            }
            else
            {
                g.setColour(waveformColour);
            }
            
            // For very zoomed-in views, use line drawing
            if (xScale > 0.5f)
            {
                // Direct sample view - good for very zoomed in
                for (int i = 0; i < displaySamples; ++i)
                {
                    int sampleIndex = startSample + i;
                    if (sampleIndex >= totalSamples)
                        break;

                    float x = bounds.getX() + i * xScale;
                    float sample = samples[sampleIndex];
                    
                    // Clip extreme values that might be in the buffer
                    sample = juce::jlimit(-1.0f, 1.0f, sample);
                    
                    float y = midY - sample * (waveformHeight / 2.0f);
                    
                    if (i == 0)
                        waveformPath.startNewSubPath(x, y);
                    else
                        waveformPath.lineTo(x, y);
                }
                
                // Draw the waveform
                g.strokePath(waveformPath, juce::PathStrokeType(1.5f));
            }
            else
            {
                // For zoomed-out views, use RMS values for better visual impact
                outlinePath.startNewSubPath(bounds.getX(), midY);
                
                const int samplesPerPixel = (int)(totalSamples / bounds.getWidth());
                
                for (float x = 0; x < bounds.getWidth(); x += 1.0f)
                {
                    int pixelStartSample = startSample + (int)(x * samplesPerPixel);
                    int pixelEndSample = juce::jmin(pixelStartSample + samplesPerPixel, totalSamples);
                    
                    if (pixelStartSample >= totalSamples)
                        break;
                    
                    float maxSample = 0.0f;
                    float minSample = 0.0f;
                    
                    // Find min/max in this pixel's range
                    for (int s = pixelStartSample; s < pixelEndSample; ++s)
                    {
                        const float sample = samples[s];
                        maxSample = juce::jmax(maxSample, sample);
                        minSample = juce::jmin(minSample, sample);
                    }
                    
                    // Clip extreme values
                    maxSample = juce::jlimit(-1.0f, 1.0f, maxSample);
                    minSample = juce::jlimit(-1.0f, 1.0f, minSample);
                    
                    float topY = midY - maxSample * (waveformHeight / 2.0f);
                    float bottomY = midY - minSample * (waveformHeight / 2.0f);
                    
                    // Ensure min height for visibility
                    if (std::abs(bottomY - topY) < 1.0f)
                    {
                        bottomY = topY + 1.0f;
                    }
                    
                    // Draw vertical line for this pixel
                    g.drawVerticalLine((int)(bounds.getX() + x), topY, bottomY);
                }
            }
        }
    }

    const juce::AudioBuffer<float>* audioBuffer = nullptr;
    juce::Colour waveformColour = juce::Colours::lightgreen;
    float zoomFactor = 1.0f;
    int scrollPosition = 0; // For future scrolling support

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WaveformDisplay)
};

} // namespace undergroundBeats