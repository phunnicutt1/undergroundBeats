#include "undergroundBeats/gui/SampleLoaderComponent.h"

namespace undergroundBeats {
namespace gui {

SampleLoaderComponent::SampleLoaderComponent(audio::AudioEngine& engine)
    : audioEngine(engine),
      currentPlayer(nullptr),
      thumbnailCache(5),  // Cache size in number of thumbnails
      thumbnail(512, engine.getSampleRate(), thumbnailCache)
{
    // Set up load button
    loadButton.setButtonText("Load Sample");
    loadButton.addListener(this);
    addAndMakeVisible(loadButton);
    
    // Set up play button
    playButton.setButtonText("Play");
    playButton.addListener(this);
    playButton.setEnabled(false);
    addAndMakeVisible(playButton);
    
    // Set up stop button
    stopButton.setButtonText("Stop");
    stopButton.addListener(this);
    stopButton.setEnabled(false);
    addAndMakeVisible(stopButton);
    
    // Set up file name label
    fileNameLabel.setText("No sample loaded", juce::dontSendNotification);
    fileNameLabel.setJustificationType(juce::Justification::centredLeft);
    addAndMakeVisible(fileNameLabel);
    
    // Set up status label
    statusLabel.setText("Drag and drop audio files here", juce::dontSendNotification);
    statusLabel.setJustificationType(juce::Justification::centredLeft);
    addAndMakeVisible(statusLabel);
    
    // Set up speed slider
    speedLabel.setText("Speed", juce::dontSendNotification);
    speedLabel.setJustificationType(juce::Justification::centredLeft);
    addAndMakeVisible(speedLabel);
    
    speedSlider.setRange(0.25, 2.0, 0.01);
    speedSlider.setValue(1.0);
    speedSlider.setSliderStyle(juce::Slider::SliderStyle::LinearHorizontal);
    speedSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 60, 20);
    speedSlider.addListener(this);
    speedSlider.setEnabled(false);
    addAndMakeVisible(speedSlider);
    
    // Set up volume slider
    volumeLabel.setText("Volume", juce::dontSendNotification);
    volumeLabel.setJustificationType(juce::Justification::centredLeft);
    addAndMakeVisible(volumeLabel);
    
    volumeSlider.setRange(0.0, 1.0, 0.01);
    volumeSlider.setValue(0.8);
    volumeSlider.setSliderStyle(juce::Slider::SliderStyle::LinearHorizontal);
    volumeSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 60, 20);
    volumeSlider.addListener(this);
    volumeSlider.setEnabled(false);
    addAndMakeVisible(volumeSlider);
    
    // Set up loop toggle
    loopToggle.setButtonText("Loop");
    loopToggle.setToggleState(false, juce::dontSendNotification);
    loopToggle.addListener(this);
    loopToggle.setEnabled(false);
    addAndMakeVisible(loopToggle);
    
    // Set component size
    setSize(600, 300);
}

SampleLoaderComponent::~SampleLoaderComponent()
{
    // Stop any playback
    if (currentPlayer)
    {
        currentPlayer->stop();
    }
}

void SampleLoaderComponent::paint(juce::Graphics& g)
{
    // Fill background
    g.fillAll(juce::Colour(40, 40, 40));
    
    // Draw border
    g.setColour(juce::Colours::grey);
    g.drawRect(getLocalBounds(), 1);
    
    // Draw thumbnail area
    juce::Rectangle<int> thumbnailBounds(20, 130, getWidth() - 40, 100);
    g.setColour(juce::Colour(30, 30, 30));
    g.fillRect(thumbnailBounds);
    
    g.setColour(juce::Colours::white);
    g.drawRect(thumbnailBounds, 1);
    
    // Draw waveform if sample is loaded
    if (currentPlayer)
    {
        g.setColour(juce::Colours::lightblue);
        thumbnail.drawChannels(g, thumbnailBounds,
                               0.0, thumbnail.getTotalLength(), 
                               1.0f);
    }
    else
    {
        g.setColour(juce::Colours::grey);
        g.drawFittedText("No Waveform", thumbnailBounds, juce::Justification::centred, 1);
    }
}

void SampleLoaderComponent::resized()
{
    // Layout constants
    const int margin = 20;
    const int buttonWidth = 100;
    const int buttonHeight = 30;
    const int labelHeight = 20;
    const int sliderHeight = 24;
    const int spacing = 10;
    
    // Get component bounds
    auto bounds = getLocalBounds();
    bounds.reduce(margin, margin);
    
    // Layout for file info
    fileNameLabel.setBounds(bounds.removeFromTop(labelHeight));
    
    bounds.removeFromTop(spacing);
    statusLabel.setBounds(bounds.removeFromTop(labelHeight));
    
    bounds.removeFromTop(spacing);
    
    // Layout for buttons
    auto buttonsRow = bounds.removeFromTop(buttonHeight);
    loadButton.setBounds(buttonsRow.removeFromLeft(buttonWidth));
    buttonsRow.removeFromLeft(spacing);
    playButton.setBounds(buttonsRow.removeFromLeft(buttonWidth));
    buttonsRow.removeFromLeft(spacing);
    stopButton.setBounds(buttonsRow.removeFromLeft(buttonWidth));
    buttonsRow.removeFromLeft(spacing);
    loopToggle.setBounds(buttonsRow.removeFromLeft(buttonWidth));
    
    bounds.removeFromTop(spacing);
    
    // Layout for sliders
    auto speedRow = bounds.removeFromTop(sliderHeight);
    speedLabel.setBounds(speedRow.removeFromLeft(60));
    speedSlider.setBounds(speedRow);
    
    bounds.removeFromTop(spacing);
    
    auto volumeRow = bounds.removeFromTop(sliderHeight);
    volumeLabel.setBounds(volumeRow.removeFromLeft(60));
    volumeSlider.setBounds(volumeRow);
    
    // The waveform display area is painted directly
}

bool SampleLoaderComponent::loadSample(const juce::File& file)
{
    if (!file.existsAsFile())
    {
        statusLabel.setText("File does not exist: " + file.getFileName(), juce::dontSendNotification);
        return false;
    }
    
    // Try to load the sample
    currentPlayer = audioEngine.loadSample(file.getFullPathName());
    
    if (currentPlayer)
    {
        // Update thumbnail
        thumbnail.clear();
        
        // Open the file as a stream
        std::unique_ptr<juce::FileInputStream> fileStream(file.createInputStream());
        if (fileStream)
        {
            thumbnail.setSource(new juce::FileInputSource(file));
        }
        
        // Update UI
        fileNameLabel.setText("Sample: " + file.getFileName(), juce::dontSendNotification);
        statusLabel.setText("Sample loaded successfully", juce::dontSendNotification);
        
        // Set initial values from sliders
        currentPlayer->setSpeed(speedSlider.getValue());
        currentPlayer->setVolume(static_cast<float>(volumeSlider.getValue()));
        currentPlayer->setLooping(loopToggle.getToggleState());
        
        // Enable controls
        playButton.setEnabled(true);
        stopButton.setEnabled(false);
        speedSlider.setEnabled(true);
        volumeSlider.setEnabled(true);
        loopToggle.setEnabled(true);
        
        repaint();
        return true;
    }
    else
    {
        statusLabel.setText("Failed to load sample: " + file.getFileName(), juce::dontSendNotification);
        return false;
    }
}

bool SampleLoaderComponent::isInterestedInFileDrag(const juce::StringArray& files)
{
    // Check if any of the files are audio files
    for (const auto& path : files)
    {
        juce::File file(path);
        if (file.hasFileExtension("wav;mp3;aiff;ogg;flac"))
        {
            return true;
        }
    }
    return false;
}

void SampleLoaderComponent::filesDropped(const juce::StringArray& files, int /*x*/, int /*y*/)
{
    // Try to load the first valid audio file
    for (const auto& path : files)
    {
        juce::File file(path);
        if (file.hasFileExtension("wav;mp3;aiff;ogg;flac"))
        {
            loadSample(file);
            break;
        }
    }
}

void SampleLoaderComponent::buttonClicked(juce::Button* button)
{
    if (button == &loadButton)
    {
        // Open file chooser dialog
        juce::FileChooser chooser("Select a sample to load...",
                                  juce::File::getSpecialLocation(juce::File::userHomeDirectory),
                                  "*.wav;*.mp3;*.aiff;*.ogg;*.flac");
        
        if (chooser.browseForFileToOpen())
        {
            loadSample(chooser.getResult());
        }
    }
    else if (button == &playButton)
    {
        // Start playback
        if (currentPlayer)
        {
            currentPlayer->play();
            playButton.setEnabled(false);
            stopButton.setEnabled(true);
        }
    }
    else if (button == &stopButton)
    {
        // Stop playback
        if (currentPlayer)
        {
            currentPlayer->stop();
            playButton.setEnabled(true);
            stopButton.setEnabled(false);
        }
    }
    else if (button == &loopToggle)
    {
        // Set looping state
        if (currentPlayer)
        {
            currentPlayer->setLooping(loopToggle.getToggleState());
        }
    }
}

void SampleLoaderComponent::sliderValueChanged(juce::Slider* slider)
{
    if (slider == &speedSlider)
    {
        // Update playback speed
        if (currentPlayer)
        {
            currentPlayer->setSpeed(speedSlider.getValue());
        }
    }
    else if (slider == &volumeSlider)
    {
        // Update volume
        if (currentPlayer)
        {
            currentPlayer->setVolume(static_cast<float>(volumeSlider.getValue()));
        }
    }
}

void SampleLoaderComponent::updateButtons()
{
    if (currentPlayer)
    {
        playButton.setEnabled(!currentPlayer->isPlaying());
        stopButton.setEnabled(currentPlayer->isPlaying());
    }
    else
    {
        playButton.setEnabled(false);
        stopButton.setEnabled(false);
    }
}

void SampleLoaderComponent::updateLabels()
{
    if (currentPlayer)
    {
        fileNameLabel.setText("Sample: " + currentPlayer->getFileName(), juce::dontSendNotification);
    }
    else
    {
        fileNameLabel.setText("No sample loaded", juce::dontSendNotification);
    }
}

} // namespace gui
} // namespace undergroundBeats