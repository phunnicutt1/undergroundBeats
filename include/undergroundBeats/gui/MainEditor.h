#pragma once

#include "undergroundBeats/core/UndergroundBeatsController.h"
#include <juce_audio_utils/juce_audio_utils.h>
#include <juce_gui_extra/juce_gui_extra.h>
#include <memory>

namespace undergroundBeats {
namespace gui {

// Forward declare the AudioThumbnailComponent
class AudioThumbnailComponent;

/**
 * @class MainEditor
 * @brief Main editor for the Underground Beats application
 * 
 * This class provides the main user interface for the Underground Beats application.
 */
class MainEditor : public juce::AudioProcessorEditor {
public:
    /**
     * @brief Constructor
     * @param processor The audio processor to use
     * @param controller The controller to use
     */
    MainEditor(juce::AudioProcessor& processor, core::UndergroundBeatsController& controller);
    
    /**
     * @brief Destructor
     */
    ~MainEditor() override;
    
    /**
     * @brief Paint the editor
     * @param g The graphics context to paint with
     */
    void paint(juce::Graphics& g) override;
    
    /**
     * @brief Handle resizing
     */
    void resized() override;
    
private:
    /**
     * @brief Initialize the UI components
     */
    void initializeUI();
    
    /**
     * @brief Update the UI components based on the current state
     */
    void updateUI();
    
    /**
     * @brief Handle file selection for loading samples
     */
    void handleLoadSample();
    
    /**
     * @brief Handle file selection for saving output
     */
    void handleSaveOutput();
    
    /**
     * @brief Handle generating variations
     * @param componentName The name of the component to generate variations for
     */
    void handleGenerateVariations(const std::string& componentName);
    
    /**
     * @brief Create a component UI for a given component name
     * @param componentName The name of the component
     * @param colour The colour to use for this component
     * @return The created ComponentUI
     */
    void createComponentUI(const std::string& componentName, juce::Colour colour);
    
    /**
     * @brief Create the effect buttons
     */
    void createEffectButtons();
    
    /**
     * @brief Create the variation explorer UI
     */
    void createVariationExplorerUI();
    
    /**
     * @brief Create the transport controls
     */
    void createTransportControls();
    
    /**
     * @brief Apply the dark theme to all components
     */
    void applyDarkTheme();
    
    core::UndergroundBeatsController& controller;
    
    // UI Components
    juce::TextButton loadButton;
    juce::TextButton saveButton;
    juce::ComboBox generationMethodComboBox;
    
    // Main workspace
    std::unique_ptr<juce::Viewport> waveformViewport;
    std::unique_ptr<juce::Component> waveformContainer;
    
    // Component controls
    struct ComponentUI {
        std::unique_ptr<juce::AudioThumbnail> thumbnail;
        std::unique_ptr<AudioThumbnailComponent> thumbnailComponent;
        std::unique_ptr<juce::Slider> gainSlider;
        std::unique_ptr<juce::Slider> pitchSlider;
        std::unique_ptr<juce::Slider> tempoSlider;
        std::unique_ptr<juce::ComboBox> effectsComboBox;
        std::unique_ptr<juce::TextButton> soloButton;
        std::unique_ptr<juce::TextButton> muteButton;
        std::unique_ptr<juce::TextButton> generateVariationsButton;
        juce::Colour colour;
    };
    
    std::map<std::string, ComponentUI> componentUIs;
    
    // Left sidebar
    std::unique_ptr<juce::FileTreeComponent> fileTree;
    std::unique_ptr<juce::DirectoryContentsList> directoryContentsList;
    std::unique_ptr<juce::FileFilter> audioFileFilter;
    
    // Transport controls
    std::unique_ptr<juce::AudioTransportSource> transportSource;
    std::unique_ptr<juce::TextButton> playButton;
    std::unique_ptr<juce::TextButton> stopButton;
    std::unique_ptr<juce::TextButton> recordButton;
    std::unique_ptr<juce::Slider> timelineSlider;
    
    // Effect icons
    std::vector<std::unique_ptr<juce::DrawableButton>> effectButtons;
    
    // Variation explorer
    std::unique_ptr<juce::Component> variationExplorer;
    std::vector<std::unique_ptr<juce::AudioThumbnail>> variationThumbnails;
    
    // Theme colors
    juce::Colour backgroundColour = juce::Colour(20, 20, 24);  // Deep black with subtle blue
    juce::Colour accentColour = juce::Colour(0, 170, 255);     // Electric blue
    juce::Colour textColour = juce::Colours::white;
    
    // Component colors
    juce::Colour bassColour = juce::Colour(255, 50, 50);       // Red for bass
    juce::Colour drumsColour = juce::Colour(50, 100, 255);     // Blue for drums
    juce::Colour midColour = juce::Colour(50, 200, 50);        // Green for mid-range
    juce::Colour instrumentsColour = juce::Colour(255, 200, 50); // Yellow for instruments
    
    // Audio format handling
    std::unique_ptr<juce::AudioFormatManager> formatManager;
    std::unique_ptr<juce::AudioThumbnailCache> thumbnailCache;
    std::unique_ptr<juce::TimeSliceThread> timeSliceThread;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainEditor)
};

/**
 * @class AudioThumbnailComponent
 * @brief Component that displays an audio thumbnail
 */
class AudioThumbnailComponent : public juce::Component {
public:
    AudioThumbnailComponent() = default;
    
    void setThumbnail(juce::AudioThumbnail* newThumbnail) {
        thumbnail = newThumbnail;
        repaint();
    }
    
    void paint(juce::Graphics& g) override {
        g.fillAll(juce::Colours::black);
        
        if (thumbnail) {
            g.setColour(juce::Colours::white);
            thumbnail->drawChannels(g, getLocalBounds(), 0.0, thumbnail->getTotalLength(), 1.0f);
        }
    }
    
private:
    juce::AudioThumbnail* thumbnail = nullptr;
};

} // namespace gui
} // namespace undergroundBeats
