/*
 * Underground Beats
 * AppComponent.h
 * 
 * Main application component
 */

#pragma once

#include <JuceHeader.h>
#include "../MainComponent.h"

namespace UndergroundBeats {

/**
 * @class AppComponent
 * @brief Main application component
 * 
 * The AppComponent class serves as the main component of the application,
 * containing all the UI components and managing the application state.
 */
class AppComponent : public juce::Component
{
public:
    AppComponent();
    ~AppComponent() override;
    
    /**
     * @brief Component resized callback
     */
    void resized() override;
    
    /**
     * @brief Component paint callback
     * 
     * @param g Graphics context to paint to
     */
    void paint(juce::Graphics& g) override;
    
private:
    // Main content component
    std::unique_ptr<MainComponent> mainComponent;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AppComponent)
};

} // namespace UndergroundBeats
