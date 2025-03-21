/*
 * Underground Beats
 * MixerChannel.cpp
 * 
 * Implementation of the MixerChannel class
 */

#include "MixerChannel.h"

namespace UndergroundBeats {

MixerChannel::MixerChannel(const juce::String& name)
    : channelName(name)
{
    // Create and set up controls
    createControls();
}

MixerChannel::~MixerChannel()
{
    // Remove listeners
    levelSlider.removeListener(this);
    panSlider.removeListener(this);
    muteButton.removeListener(this);
    soloButton.removeListener(this);
    
    for (auto& send : sends)
    {
        send.slider.removeListener(this);
    }
}

void MixerChannel::setChannelName(const juce::String& name)
{
    channelName = name;
    nameLabel.setText(channelName, juce::dontSendNotification);
}

juce::String MixerChannel::getChannelName() const
{
    return channelName;
}

void MixerChannel::setLevel(float level, juce::NotificationType notification)
{
    levelSlider.setValue(level, notification);
}

float MixerChannel::getLevel() const
{
    return static_cast<float>(levelSlider.getValue());
}

void MixerChannel::setPan(float pan, juce::NotificationType notification)
{
    panSlider.setValue(pan, notification);
}

float MixerChannel::getPan() const
{
    return static_cast<float>(panSlider.getValue());
}

void MixerChannel::setMute(bool mute, juce::NotificationType notification)
{
    muteButton.setToggleState(mute, notification);
}

bool MixerChannel::isMuted() const
{
    return muteButton.getToggleState();
}

void MixerChannel::setSolo(bool solo, juce::NotificationType notification)
{
    soloButton.setToggleState(solo, notification);
}

bool MixerChannel::isSoloed() const
{
    return soloButton.getToggleState();
}

void MixerChannel::setSendLevel(int sendIndex, float level, juce::NotificationType notification)
{
    if (sendIndex >= 0 && sendIndex < static_cast<int>(sends.size()))
    {
        sends[sendIndex].slider.setValue(level, notification);
    }
}

float MixerChannel::getSendLevel(int sendIndex) const
{
    if (sendIndex >= 0 && sendIndex < static_cast<int>(sends.size()))
    {
        return static_cast<float>(sends[sendIndex].slider.getValue());
    }
    
    return 0.0f;
}

void MixerChannel::setLevelChangeCallback(std::function<void(float)> callback)
{
    levelChangeCallback = callback;
}

void MixerChannel::setPanChangeCallback(std::function<void(float)> callback)
{
    panChangeCallback = callback;
}

void MixerChannel::setMuteChangeCallback(std::function<void(bool)> callback)
{
    muteChangeCallback = callback;
}

void MixerChannel::setSoloChangeCallback(std::function<void(bool)> callback)
{
    soloChangeCallback = callback;
}

void MixerChannel::setSendLevelChangeCallback(int sendIndex, std::function<void(float)> callback)
{
    if (sendIndex >= 0 && sendIndex < static_cast<int>(sends.size()))
    {
        sends[sendIndex].callback = callback;
    }
}

void MixerChannel::setNumSends(int numSends)
{
    // Ensure we have at least 0 sends
    numSends = std::max(0, numSends);
    
    // Resize the sends vector
    while (sends.size() < static_cast<size_t>(numSends))
    {
        SendControl send;
        
        // Set up slider
        send.slider.setSliderStyle(juce::Slider::LinearVertical);
        send.slider.setRange(0.0, 1.0);
        send.slider.setValue(0.0);
        send.slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 40, 20);
        send.slider.addListener(this);
        
        // Set up label
        send.label.setText("FX " + juce::String(sends.size() + 1), juce::dontSendNotification);
        send.label.setJustificationType(juce::Justification::centred);
        
        addAndMakeVisible(send.slider);
        addAndMakeVisible(send.label);
        
        sends.push_back(send);
    }
    
    // Remove excess sends
    while (sends.size() > static_cast<size_t>(numSends))
    {
        sends.pop_back();
    }
    
    // Update the layout
    updateLayout();
}

int MixerChannel::getNumSends() const
{
    return static_cast<int>(sends.size());
}

void MixerChannel::resized()
{
    updateLayout();
}

void MixerChannel::paint(juce::Graphics& g)
{
    // Background fill
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId).brighter(0.1f));
    
    // Border
    g.setColour(juce::Colours::grey);
    g.drawRect(getLocalBounds().toFloat(), 1.0f);
}

void MixerChannel::sliderValueChanged(juce::Slider* slider)
{
    if (slider == &levelSlider)
    {
        if (levelChangeCallback)
        {
            levelChangeCallback(static_cast<float>(levelSlider.getValue()));
        }
    }
    else if (slider == &panSlider)
    {
        if (panChangeCallback)
        {
            panChangeCallback(static_cast<float>(panSlider.getValue()));
        }
    }
    else
    {
        // Check if it's a send slider
        for (size_t i = 0; i < sends.size(); ++i)
        {
            if (slider == &sends[i].slider)
            {
                if (sends[i].callback)
                {
                    sends[i].callback(static_cast<float>(sends[i].slider.getValue()));
                }
                break;
            }
        }
    }
}

void MixerChannel::buttonClicked(juce::Button* button)
{
    if (button == &muteButton)
    {
        if (muteChangeCallback)
        {
            muteChangeCallback(muteButton.getToggleState());
        }
    }
    else if (button == &soloButton)
    {
        if (soloChangeCallback)
        {
            soloChangeCallback(soloButton.getToggleState());
        }
    }
}

void MixerChannel::createControls()
{
    // Level slider
    levelSlider.setSliderStyle(juce::Slider::LinearVertical);
    levelSlider.setRange(0.0, 1.0);
    levelSlider.setValue(0.8);
    levelSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 40, 20);
    levelSlider.addListener(this);
    
    // Level label
    levelLabel.setText("Level", juce::dontSendNotification);
    levelLabel.setJustificationType(juce::Justification::centred);
    
    // Pan slider
    panSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    panSlider.setRange(-1.0, 1.0);
    panSlider.setValue(0.0);
    panSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 40, 20);
    panSlider.addListener(this);
    
    // Pan label
    panLabel.setText("Pan", juce::dontSendNotification);
    panLabel.setJustificationType(juce::Justification::centred);
    
    // Mute button
    muteButton.setButtonText("M");
    muteButton.setClickingTogglesState(true);
    muteButton.addListener(this);
    
    // Solo button
    soloButton.setButtonText("S");
    soloButton.setClickingTogglesState(true);
    soloButton.addListener(this);
    
    // Channel name label
    nameLabel.setText(channelName, juce::dontSendNotification);
    nameLabel.setJustificationType(juce::Justification::centred);
    nameLabel.setFont(juce::Font(14.0f, juce::Font::bold));
    
    // Add components
    addAndMakeVisible(levelSlider);
    addAndMakeVisible(levelLabel);
    addAndMakeVisible(panSlider);
    addAndMakeVisible(panLabel);
    addAndMakeVisible(muteButton);
    addAndMakeVisible(soloButton);
    addAndMakeVisible(nameLabel);
}

void MixerChannel::updateLayout()
{
    auto area = getLocalBounds();
    
    // Name label at the top
    nameLabel.setBounds(area.removeFromTop(20));
    
    // Mute/solo buttons
    auto buttonRow = area.removeFromTop(30).reduced(2);
    auto buttonWidth = buttonRow.getWidth() / 2;
    muteButton.setBounds(buttonRow.removeFromLeft(buttonWidth).reduced(2));
    soloButton.setBounds(buttonRow.reduced(2));
    
    // Level control
    auto levelArea = area.removeFromBottom(150).reduced(2);
    levelLabel.setBounds(levelArea.removeFromBottom(20));
    levelSlider.setBounds(levelArea);
    
    // Pan control
    auto panArea = area.removeFromBottom(70).reduced(2);
    panLabel.setBounds(panArea.removeFromBottom(20));
    panSlider.setBounds(panArea);
    
    // Remaining area for send controls
    if (!sends.empty())
    {
        int sendHeight = area.getHeight() / static_cast<int>(sends.size());
        
        for (auto& send : sends)
        {
            auto sendArea = area.removeFromTop(sendHeight).reduced(2);
            send.label.setBounds(sendArea.removeFromBottom(20));
            send.slider.setBounds(sendArea);
        }
    }
}

} // namespace UndergroundBeats
