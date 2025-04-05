#pragma once

#include <JuceHeader.h>

// Timeline ruler with zoom and scroll support
class TimelineRulerComponent : public juce::Component
{
public:
    TimelineRulerComponent();
    ~TimelineRulerComponent() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

    void setZoomLevel(float zoom);
    void setScrollPosition(float position);

private:
    float zoomLevel = 1.0f;
    float scrollPosition = 0.0f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TimelineRulerComponent)
};