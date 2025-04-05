#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_formats/juce_audio_formats.h>
#include <juce_audio_utils/juce_audio_utils.h>
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

// Forward declare helper thumbnail class if needed or include its header
// For simplicity, let's assume we reuse or adapt the AudioThumbnailComponent concept

namespace undergroundBeats {
namespace gui {

// Forward declare the thumbnail component used internally
class VariationThumbnailComponent;

/**
 * @class VariationExplorerComponent
 * @brief Displays generated audio variations as thumbnails and allows selection/preview.
 */
class VariationExplorerComponent : public juce::Component,
                                   private juce::Timer
{
public:
    /**
     * @class Listener
     * @brief Interface for listeners interested in variation explorer events.
     */
    class Listener {
    public:
        virtual ~Listener() = default;
        // Called when a variation thumbnail is clicked/selected
        virtual void variationSelected(const std::string& componentName, const std::string& variationId) = 0;
        // Called when preview should start (e.g., mouse hover)
        virtual void variationPreviewStart(const std::string& componentName, const std::string& variationId) = 0;
        // Called when preview should stop
        virtual void variationPreviewStop() = 0;
    };

    /**
     * @brief Constructor
     * @param formatManager Reference to the global AudioFormatManager
     * @param thumbnailCache Reference to the global AudioThumbnailCache
     */
    VariationExplorerComponent(juce::AudioFormatManager& formatManager,
                               juce::AudioThumbnailCache& thumbnailCache);

    /**
     * @brief Destructor
     */
    ~VariationExplorerComponent() override;

    // --- Listener Management ---
    void addListener(Listener* listenerToAdd);
    void removeListener(Listener* listenerToRemove);

    // --- Variation Management ---

    /**
     * @brief Clears all currently displayed variations.
     */
    void clearVariations();

    /**
     * @brief Adds a new variation to be displayed.
     * @param variationId Unique ID for this variation.
     * @param audioBuffer The audio data for the variation.
     * @param componentName The name of the original component this variation belongs to.
     */
    void addVariation(const std::string& variationId,
                      const juce::AudioBuffer<float>& audioBuffer,
                      const std::string& componentName); // Added componentName

    // --- Component Overrides ---
    void paint(juce::Graphics& g) override;
    void resized() override;

    // --- Mouse Handling for Previews (Optional) ---
    void mouseEnter(const juce::MouseEvent& event) override;
    void mouseExit(const juce::MouseEvent& event) override;
    void mouseMove(const juce::MouseEvent& event) override;


private:
    // --- Timer Callback for Preview Delay ---
    void timerCallback() override;

    // --- Internal Data Structures ---
    struct VariationInfo {
        std::string id;
        std::string componentName; // Store associated component name
        std::unique_ptr<VariationThumbnailComponent> thumbnailComponent;
    };

    // Use a vector to maintain order, map for quick lookup if needed later
    std::vector<VariationInfo> variations;

    // --- UI Elements ---
    juce::Viewport viewport; // To make thumbnails scrollable horizontally
    juce::Component thumbnailContainer; // Holds the actual thumbnail components

    // --- Dependencies ---
    juce::AudioFormatManager& formatManager;
    juce::AudioThumbnailCache& thumbnailCache;

    // --- Listener ---
    juce::ListenerList<Listener> listeners;

    // --- Preview State ---
    juce::String currentHoveredVariationId; // ID of the variation under the mouse
    bool isMouseOver = false;
    static constexpr int previewDelayMs = 500; // Delay before starting preview

    // --- Helper ---
    void notifyVariationSelected(const std::string& componentName, const std::string& variationId);
    void notifyPreviewStart(const std::string& componentName, const std::string& variationId);
    void notifyPreviewStop();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(VariationExplorerComponent)
};


/**
 * @class VariationThumbnailComponent
 * @brief Internal component to display a single variation thumbnail and handle clicks.
 */
class VariationThumbnailComponent : public juce::Component,
                                    public juce::ChangeListener,
                                    public juce::SettableTooltipClient

{
public:
    VariationThumbnailComponent(const std::string& id,
                                const std::string& compName,
                                juce::AudioFormatManager& formatMgr,
                                juce::AudioThumbnailCache& cache,
                                VariationExplorerComponent* owner);
    ~VariationThumbnailComponent() override;

    void setSource(std::unique_ptr<juce::InputSource> source);
    const std::string& getVariationId() const { return variationId; }
    const std::string& getComponentName() const { return componentName; }

    void paint(juce::Graphics& g) override;
    void resized() override;
    void mouseDown(const juce::MouseEvent& event) override;

    // ChangeListener callback for thumbnail loading
    void changeListenerCallback(juce::ChangeBroadcaster* source) override;

private:
    std::string variationId;
    std::string componentName;
    juce::AudioThumbnail thumbnail;
    juce::Colour waveformColour = juce::Colours::grey; // Default color
    VariationExplorerComponent* ownerComponent = nullptr; // To notify clicks
};


} // namespace gui
} // namespace undergroundBeats 