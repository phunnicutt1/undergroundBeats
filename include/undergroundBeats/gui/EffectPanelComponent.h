#pragma once

#include <JuceHeader.h>

// Base class for collapsible/popup effect panels with dark/neon styling
class EffectPanelComponent : public juce::Component
{
public:
    EffectPanelComponent();
    ~EffectPanelComponent() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

    void toggleVisibility(bool animate = true);
    bool isVisible() const noexcept { return visible; }

protected:
    bool visible = false;
    juce::ComponentAnimator animator;

    void showPanel(bool animate);
    void hidePanel(bool animate);
};