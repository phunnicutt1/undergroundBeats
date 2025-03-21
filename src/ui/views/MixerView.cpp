/*
 * Underground Beats
 * MixerView.cpp
 * 
 * Implementation of the MixerView class
 */

#include "MixerView.h"

namespace UndergroundBeats {

MixerView::MixerView()
    : audioEngine(nullptr),
      numEffectSends(2) // Default to 2 effect sends
{
    // Set up the viewport for channel scrolling
    addAndMakeVisible(channelsViewport);
    channelsViewport.setViewedComponent(&channelsContainer, false);
    channelsViewport.setScrollBarsShown(true, false);
    
    // Add control buttons
    addAndMakeVisible(addChannelButton);
    addAndMakeVisible(removeChannelButton);
    addAndMakeVisible(addSendButton);
    addAndMakeVisible(removeSendButton);
    
    // Configure buttons
    addChannelButton.setButtonText("Add Channel");
    removeChannelButton.setButtonText("Remove Channel");
    addSendButton.setButtonText("Add Send");
    removeSendButton.setButtonText("Remove Send");
    
    // Add button listeners
    addChannelButton.addListener(this);
    removeChannelButton.addListener(this);
    addSendButton.addListener(this);
    removeSendButton.addListener(this);
    
    // Create default channels
    createDefaultChannels();
}

MixerView::~MixerView()
{
    // Remove button listeners
    addChannelButton.removeListener(this);
    removeChannelButton.removeListener(this);
    addSendButton.removeListener(this);
    removeSendButton.removeListener(this);
}

void MixerView::setAudioEngine(Engine* engine)
{
    audioEngine = engine;
}

void MixerView::setNumInputChannels(int numChannels)
{
    // Ensure we have at least 1 channel
    numChannels = std::max(1, numChannels);
    
    // Resize the input channels vector
    while (inputChannels.size() < static_cast<size_t>(numChannels))
    {
        auto index = inputChannels.size();
        auto channel = std::make_unique<MixerChannel>("Channel " + juce::String(index + 1));
        channel->setNumSends(numEffectSends);
        
        // Set up callbacks for this channel
        channel->setLevelChangeCallback([this, index](float level) {
            handleLevelChange(static_cast<int>(index), level);
        });
        
        channel->setPanChangeCallback([this, index](float pan) {
            handlePanChange(static_cast<int>(index), pan);
        });
        
        channel->setMuteChangeCallback([this, index](bool muted) {
            handleMuteChange(static_cast<int>(index), muted);
        });
        
        channel->setSoloChangeCallback([this, index](bool soloed) {
            handleSoloChange(static_cast<int>(index), soloed);
        });
        
        // Set up callbacks for sends
        for (int sendIndex = 0; sendIndex < numEffectSends; ++sendIndex)
        {
            const int sendIndexCopy = sendIndex;
            channel->setSendLevelChangeCallback(sendIndex, [this, index, sendIndexCopy](float level) {
                handleSendLevelChange(static_cast<int>(index), sendIndexCopy, level);
            });
        }
        
        channelsContainer.addAndMakeVisible(channel.get());
        inputChannels.push_back(std::move(channel));
    }
    
    // Remove excess channels
    while (inputChannels.size() > static_cast<size_t>(numChannels))
    {
        inputChannels.pop_back();
    }
    
    // Update the layout
    updateChannelLayout();
}

void MixerView::setNumEffectSends(int numSends)
{
    // Ensure we have at least 0 sends
    numEffectSends = std::max(0, numSends);
    
    // Update all channels with the new number of sends
    for (auto& channel : inputChannels)
    {
        channel->setNumSends(numEffectSends);
    }
    
    // Resize the effect return channels vector
    while (effectReturnChannels.size() < static_cast<size_t>(numEffectSends))
    {
        auto index = effectReturnChannels.size();
        auto channel = std::make_unique<MixerChannel>("FX " + juce::String(index + 1));
        channel->setNumSends(0); // Effect returns don't have sends
        
        // Set up callbacks for this channel
        channel->setLevelChangeCallback([this, index](float level) {
            // Handle effect return level change
            // This would be implemented to control the effect return level
        });
        
        channel->setPanChangeCallback([this, index](float pan) {
            // Handle effect return pan change
            // This would be implemented to control the effect return pan
        });
        
        channel->setMuteChangeCallback([this, index](bool muted) {
            // Handle effect return mute change
            // This would be implemented to control the effect return mute
        });
        
        channelsContainer.addAndMakeVisible(channel.get());
        effectReturnChannels.push_back(std::move(channel));
    }
    
    // Remove excess effect return channels
    while (effectReturnChannels.size() > static_cast<size_t>(numEffectSends))
    {
        effectReturnChannels.pop_back();
    }
    
    // Update the layout
    updateChannelLayout();
}

void MixerView::resized()
{
    auto area = getLocalBounds();
    
    // Button row at the bottom
    auto buttonRow = area.removeFromBottom(40);
    
    // Split button row into 4 equal parts
    auto buttonWidth = buttonRow.getWidth() / 4;
    addChannelButton.setBounds(buttonRow.removeFromLeft(buttonWidth).reduced(2));
    removeChannelButton.setBounds(buttonRow.removeFromLeft(buttonWidth).reduced(2));
    addSendButton.setBounds(buttonRow.removeFromLeft(buttonWidth).reduced(2));
    removeSendButton.setBounds(buttonRow.reduced(2));
    
    // Viewport takes the rest of the space
    channelsViewport.setBounds(area);
    
    // Update channel layout
    updateChannelLayout();
}

void MixerView::paint(juce::Graphics& g)
{
    // Background fill
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
    
    // Border
    g.setColour(juce::Colours::grey);
    g.drawRect(getLocalBounds().toFloat(), 1.0f);
}

void MixerView::buttonClicked(juce::Button* button)
{
    if (button == &addChannelButton)
    {
        // Add a new input channel
        setNumInputChannels(static_cast<int>(inputChannels.size()) + 1);
    }
    else if (button == &removeChannelButton)
    {
        // Remove the last input channel
        if (inputChannels.size() > 1)
        {
            setNumInputChannels(static_cast<int>(inputChannels.size()) - 1);
        }
    }
    else if (button == &addSendButton)
    {
        // Add a new effect send
        setNumEffectSends(numEffectSends + 1);
    }
    else if (button == &removeSendButton)
    {
        // Remove the last effect send
        if (numEffectSends > 0)
        {
            setNumEffectSends(numEffectSends - 1);
        }
    }
}

void MixerView::createDefaultChannels()
{
    // Create a master channel
    masterChannel = std::make_unique<MixerChannel>("Master");
    masterChannel->setNumSends(0); // Master doesn't have sends
    
    // Set up callbacks for the master channel
    masterChannel->setLevelChangeCallback([this](float level) {
        // Handle master level change
        // This would be implemented to control the master level
    });
    
    masterChannel->setPanChangeCallback([this](float pan) {
        // Handle master pan change
        // This would be implemented to control the master pan
    });
    
    masterChannel->setMuteChangeCallback([this](bool muted) {
        // Handle master mute change
        // This would be implemented to control the master mute
    });
    
    channelsContainer.addAndMakeVisible(masterChannel.get());
    
    // Create default input channels and effect returns
    setNumInputChannels(8); // Default to 8 input channels
    setNumEffectSends(2);   // Default to 2 effect sends
}

void MixerView::updateChannelLayout()
{
    // Calculate the total width needed for all channels
    int numChannels = static_cast<int>(inputChannels.size() + effectReturnChannels.size() + 1); // +1 for master
    int channelWidth = 100; // Fixed width per channel
    int channelHeight = channelsViewport.getHeight();
    
    // Set the container size
    channelsContainer.setSize(numChannels * channelWidth, channelHeight);
    
    // Position input channels
    int x = 0;
    for (auto& channel : inputChannels)
    {
        channel->setBounds(x, 0, channelWidth, channelHeight);
        x += channelWidth;
    }
    
    // Position effect return channels
    for (auto& channel : effectReturnChannels)
    {
        channel->setBounds(x, 0, channelWidth, channelHeight);
        x += channelWidth;
    }
    
    // Position master channel
    masterChannel->setBounds(x, 0, channelWidth, channelHeight);
}

void MixerView::handleLevelChange(int channelIndex, float level)
{
    // Pass the level change to the audio engine
    if (audioEngine)
    {
        // This would be implemented to control the channel level in the audio engine
    }
}

void MixerView::handlePanChange(int channelIndex, float pan)
{
    // Pass the pan change to the audio engine
    if (audioEngine)
    {
        // This would be implemented to control the channel pan in the audio engine
    }
}

void MixerView::handleMuteChange(int channelIndex, bool muted)
{
    // Pass the mute change to the audio engine
    if (audioEngine)
    {
        // This would be implemented to control the channel mute in the audio engine
    }
}

void MixerView::handleSoloChange(int channelIndex, bool soloed)
{
    // Pass the solo change to the audio engine
    if (audioEngine)
    {
        // This would be implemented to control the channel solo in the audio engine
    }
    
    // When a channel is soloed, we need to update the other channels
    for (size_t i = 0; i < inputChannels.size(); ++i)
    {
        if (i != static_cast<size_t>(channelIndex))
        {
            // This would be implemented to update the other channels based on solo state
        }
    }
}

void MixerView::handleSendLevelChange(int channelIndex, int sendIndex, float level)
{
    // Pass the send level change to the audio engine
    if (audioEngine)
    {
        // This would be implemented to control the send level in the audio engine
    }
}

} // namespace UndergroundBeats
