#include "undergroundBeats/gui/VariationExplorerComponent.h"

// Need MemoryAudioFormat for creating thumbnails from buffers
#include <juce_audio_formats/juce_audio_formats.h>

namespace undergroundBeats {
namespace gui {

// --- VariationThumbnailComponent Implementation ---

VariationThumbnailComponent::VariationThumbnailComponent(
    const std::string& id,
    const std::string& compName,
    juce::AudioFormatManager& formatMgr,
    juce::AudioThumbnailCache& cache,
    VariationExplorerComponent* owner)
    : variationId(id),
      componentName(compName),
      thumbnail(512, formatMgr, cache), // Standard thumbnail setup
      ownerComponent(owner)
{
    thumbnail.addChangeListener(this);
    setTooltip("Variation ID: " + juce::String(id)); // Set tooltip for identification
}

VariationThumbnailComponent::~VariationThumbnailComponent()
{
    thumbnail.removeChangeListener(this);
}

void VariationThumbnailComponent::setSource(std::unique_ptr<juce::InputSource> source)
{
    thumbnail.setSource(source.release()); // Thumbnail takes ownership
}

void VariationThumbnailComponent::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds();
    g.fillAll(juce::Colours::darkgrey.darker()); // Background for thumbnail area
    g.setColour(juce::Colours::black);
    g.drawRect(bounds, 1.0f); // Border

    if (thumbnail.isFullyLoaded()) {
        g.setColour(waveformColour);
        thumbnail.drawChannels(g, bounds.reduced(2), 0.0, thumbnail.getTotalLength(), 1.0f);
    } else {
        g.setColour(juce::Colours::lightgrey);
        g.drawText("...", bounds, juce::Justification::centred, false); // Placeholder while loading
    }
}

void VariationThumbnailComponent::resized()
{
    // No children to resize typically
}

void VariationThumbnailComponent::mouseDown(const juce::MouseEvent& event)
{
    if (event.mods.isLeftButtonDown() && ownerComponent) {
        // Notify the parent VariationExplorerComponent about the click
        ownerComponent->notifyVariationSelected(componentName, variationId);
    }
}

void VariationThumbnailComponent::changeListenerCallback(juce::ChangeBroadcaster* /*source*/)
{
    // Thumbnail data has changed (e.g., loaded), repaint
    repaint();
}


// --- VariationExplorerComponent Implementation ---

VariationExplorerComponent::VariationExplorerComponent(
    juce::AudioFormatManager& fm, juce::AudioThumbnailCache& tc)
    : formatManager(fm), thumbnailCache(tc)
{
    // Setup viewport for horizontal scrolling
    addAndMakeVisible(viewport);
    viewport.setViewedComponent(&thumbnailContainer, false); // Container not owned
    thumbnailContainer.setSize(0, 0); // Initial size, will grow
}

VariationExplorerComponent::~VariationExplorerComponent()
{
    listeners.clear();
    stopTimer(); // Ensure timer is stopped
    // Child components are managed by unique_ptr in `variations` vector
}

void VariationExplorerComponent::addListener(Listener* listenerToAdd)
{
    listeners.add(listenerToAdd);
}

void VariationExplorerComponent::removeListener(Listener* listenerToRemove)
{
    listeners.remove(listenerToRemove);
}

void VariationExplorerComponent::clearVariations()
{
    thumbnailContainer.removeAllChildren(); // Remove UI elements
    variations.clear(); // Clear data
    thumbnailContainer.setSize(0, getHeight()); // Reset container size
    currentHoveredVariationId = ""; // Reset hover state
    stopTimer(); // Stop preview timer if running
}

void VariationExplorerComponent::addVariation(
    const std::string& variationId,
    const juce::AudioBuffer<float>& audioBuffer,
    const std::string& componentName) // Added componentName
{
    if (audioBuffer.getNumSamples() == 0) {
        DBG("VariationExplorerComponent::addVariation: Skipping empty buffer for ID " + variationId);
        return;
    }

    VariationInfo info;
    info.id = variationId;
    info.componentName = componentName; // Store component name

    info.thumbnailComponent = std::make_unique<VariationThumbnailComponent>(
        variationId, componentName, formatManager, thumbnailCache, this); // Pass componentName

    // --- Create Thumbnail Source (similar to MainEditor::createComponentUI) ---
    auto numChannels = juce::jmax(1, audioBuffer.getNumChannels());
    auto bufferToUse = std::make_unique<juce::AudioBuffer<float>>(numChannels, audioBuffer.getNumSamples());
    for (int ch = 0; ch < numChannels; ++ch) {
        if (ch < audioBuffer.getNumChannels()) {
            bufferToUse->copyFrom(ch, 0, audioBuffer, ch, 0, audioBuffer.getNumSamples());
        } else {
            bufferToUse->clear(ch, 0, audioBuffer.getNumSamples());
        }
    }

    juce::MemoryOutputStream memoryStream;
    std::unique_ptr<juce::AudioFormatWriter> writer(
        formatManager.findFormatForFileExtension("wav")->createWriterFor(&memoryStream,
                                                                         44100.0, // Assume SR or get from controller if possible
                                                                         numChannels, 16, {}, 0));
    if (writer) {
        writer->writeFromAudioSampleBuffer(*bufferToUse, 0, bufferToUse->getNumSamples());
        writer = nullptr; // Flush
        auto inputSource = std::make_unique<juce::MemoryInputSource>(memoryStream.getData(), memoryStream.getDataSize(), false);
        info.thumbnailComponent->setSource(std::move(inputSource));
    } else {
        DBG("VariationExplorerComponent: Failed to create WAV writer for thumbnail ID " + variationId);
    }
    // --------------------------------------------------------------------

    // Add the thumbnail component to the container
    thumbnailContainer.addAndMakeVisible(*info.thumbnailComponent);
    variations.push_back(std::move(info));

    // Update container size and trigger layout
    resized();
}


void VariationExplorerComponent::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::black.withAlpha(0.8f)); // Semi-transparent background
    g.setColour(juce::Colours::grey);
    g.drawRect(getLocalBounds(), 1.0f); // Border
}

void VariationExplorerComponent::resized()
{
    auto bounds = getLocalBounds();
    viewport.setBounds(bounds); // Viewport takes the whole area

    // Calculate total width needed for all thumbnails
    int totalWidth = 0;
    int thumbnailWidth = 100; // Example fixed width
    int spacing = 5;

    for (size_t i = 0; i < variations.size(); ++i) {
        if (variations[i].thumbnailComponent) {
            variations[i].thumbnailComponent->setBounds(totalWidth, 0, thumbnailWidth, bounds.getHeight());
            totalWidth += thumbnailWidth + spacing;
        }
    }

    // Set the size of the component being viewed by the viewport
    thumbnailContainer.setSize(totalWidth, bounds.getHeight());
}


// --- Mouse Handling & Preview Logic ---

void VariationExplorerComponent::mouseMove(const juce::MouseEvent& event)
{
    juce::String newHoveredId = "";
    // Find which thumbnail the mouse is over
    auto* child = thumbnailContainer.getComponentAt(event.getPosition() - thumbnailContainer.getPosition());
    auto* thumb = dynamic_cast<VariationThumbnailComponent*>(child);

    if (thumb) {
        newHoveredId = thumb->getVariationId();
    }

    if (newHoveredId != currentHoveredVariationId) {
        currentHoveredVariationId = newHoveredId;
        if (!currentHoveredVariationId.isEmpty()) {
            // Start timer for preview delay
            startTimer(previewDelayMs);
        } else {
            // Moved off a thumbnail, stop preview immediately and stop timer
            stopTimer();
            notifyPreviewStop();
        }
    }
    isMouseOver = true; // Track mouse presence
}


void VariationExplorerComponent::mouseEnter(const juce::MouseEvent& /*event*/)
{
    isMouseOver = true;
}

void VariationExplorerComponent::mouseExit(const juce::MouseEvent& /*event*/)
{
    isMouseOver = false;
    currentHoveredVariationId = "";
    stopTimer(); // Stop timer if mouse leaves the whole component
    notifyPreviewStop(); // Ensure preview stops
}

void VariationExplorerComponent::timerCallback()
{
    // Timer fires after delay
    stopTimer(); // Only fire once per hover entry

    if (isMouseOver && !currentHoveredVariationId.isEmpty()) {
        // Find the component name associated with the hovered ID
        std::string componentName = "";
        for(const auto& info : variations) {
            if (info.id == currentHoveredVariationId.toStdString()) {
                componentName = info.componentName;
                break;
            }
        }
        if (!componentName.empty()) {
            notifyPreviewStart(componentName, currentHoveredVariationId.toStdString());
        }
    }
}


// --- Notification Helpers ---

void VariationExplorerComponent::notifyVariationSelected(const std::string& componentName, const std::string& variationId)
{
    // Forward the click event to listeners (MainEditor)
    listeners.call([&](Listener& l) { l.variationSelected(componentName, variationId); });
}

void VariationExplorerComponent::notifyPreviewStart(const std::string& componentName, const std::string& variationId)
{
    listeners.call([&](Listener& l) { l.variationPreviewStart(componentName, variationId); });
}

void VariationExplorerComponent::notifyPreviewStop()
{
    listeners.call([&](Listener& l) { l.variationPreviewStop(); });
}


} // namespace gui
} // namespace undergroundBeats 