/*
 * Underground Beats
 * AppComponent.cpp
 * 
 * Implementation of the AppComponent class
 */

#include "AppComponent.h"

namespace UndergroundBeats {

AppComponent::AppComponent()
{
    // Create main component
    mainComponent = std::make_unique<MainComponent>();
    addAndMakeVisible(mainComponent.get());
    
    // Set the size of the application window
    setSize(1024, 768);
}

AppComponent::~AppComponent()
{
    // Clean up resources
}

void AppComponent::resized()
{
    // Main component takes up the entire area
    mainComponent->setBounds(getLocalBounds());
}

void AppComponent::paint(juce::Graphics& g)
{
    // Nothing to paint directly in the app component,
    // let the main component handle painting
}

} // namespace UndergroundBeats
