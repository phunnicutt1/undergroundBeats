#include "undergroundBeats/gui/EffectPanelComponent.h"

EffectPanelComponent::EffectPanelComponent()
{
    setOpaque(false);
    setVisible(false);
}

EffectPanelComponent::~EffectPanelComponent() = default;

void EffectPanelComponent::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();

    // Dark background with neon border
    g.setColour(juce::Colours::black.withAlpha(0.85f));
    g.fillRoundedRectangle(bounds, 8.0f);

    g.setColour(juce::Colours::cyan);
    g.drawRoundedRectangle(bounds, 8.0f, 2.0f);
}

void EffectPanelComponent::resized()
{
    // Override in subclasses to layout controls
}

void EffectPanelComponent::toggleVisibility(bool animate)
{
    if (visible)
        hidePanel(animate);
    else
        showPanel(animate);
}

void EffectPanelComponent::showPanel(bool animate)
{
    visible = true;
    setVisible(true);

    if (animate)
    {
        animator.cancelAllAnimations(false);
        setAlpha(0.0f);
        animator.fadeIn(this, 200);
    }
    else
    {
        setAlpha(1.0f);
    }
}

void EffectPanelComponent::hidePanel(bool animate)
{
    visible = false;

    if (animate)
    {
        animator.cancelAllAnimations(false);
        animator.fadeOut(this, 200);
    }
    else
    {
        setVisible(false);
    }
}