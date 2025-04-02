#include "undergroundBeats/UndergroundBeatsProcessor.h"
#include "undergroundBeats/gui/MainEditor.h"

namespace undergroundBeats {

UndergroundBeatsProcessor::UndergroundBeatsProcessor()
    : AudioProcessor(BusesProperties()
                    .withInput("Input", juce::AudioChannelSet::stereo())
                    .withOutput("Output", juce::AudioChannelSet::stereo())),
      thumbnailCache(std::make_unique<juce::AudioThumbnailCache>(100)) {
    
    // Initialize the format manager
    formatManager.registerBasicFormats();
    
    // Initialize the controller
    controller.initialize();
}

UndergroundBeatsProcessor::~UndergroundBeatsProcessor() = default;

void UndergroundBeatsProcessor::prepareToPlay(double sampleRate, int samplesPerBlock) {
    // Nothing to do here yet
}

void UndergroundBeatsProcessor::releaseResources() {
    // Nothing to do here yet
}

void UndergroundBeatsProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) {
    // Clear the output buffer
    buffer.clear();
    
    // TODO: Implement audio processing
    // For now, just pass through any input
    
    // Apply any processing from the controller (when implemented)
}

juce::AudioProcessorEditor* UndergroundBeatsProcessor::createEditor() {
    return new gui::MainEditor(*this, controller);
}

bool UndergroundBeatsProcessor::hasEditor() const {
    return true;
}

const juce::String UndergroundBeatsProcessor::getName() const {
    return "Underground Beats";
}

bool UndergroundBeatsProcessor::acceptsMidi() const {
    return true;
}

bool UndergroundBeatsProcessor::producesMidi() const {
    return false;
}

bool UndergroundBeatsProcessor::isMidiEffect() const {
    return false;
}

double UndergroundBeatsProcessor::getTailLengthSeconds() const {
    return 0.0;
}

int UndergroundBeatsProcessor::getNumPrograms() {
    return 1;
}

int UndergroundBeatsProcessor::getCurrentProgram() {
    return 0;
}

void UndergroundBeatsProcessor::setCurrentProgram(int index) {
    // No programs yet, so nothing to do
}

const juce::String UndergroundBeatsProcessor::getProgramName(int index) {
    return "Default";
}

void UndergroundBeatsProcessor::changeProgramName(int index, const juce::String& newName) {
    // No programs yet, so nothing to do
}

void UndergroundBeatsProcessor::getStateInformation(juce::MemoryBlock& destData) {
    // TODO: Save state information
}

void UndergroundBeatsProcessor::setStateInformation(const void* data, int sizeInBytes) {
    // TODO: Load state information
}

// This creates new instances of the plugin
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() {
    return new UndergroundBeatsProcessor();
}

} // namespace undergroundBeats
