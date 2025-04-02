#include "undergroundBeats/gui/MainComponent.h"

namespace undergroundBeats {
namespace gui {

// Private Component class for the test tone tab
class TestToneComponent : public juce::Component
{
public:
    TestToneComponent(audio::AudioEngine& audioEngine)
        : engine(audioEngine)
    {
        // Set up start button
        startButton.setButtonText("Start");
        startButton.onClick = [this] { startButtonClicked(); };
        addAndMakeVisible(startButton);
        
        // Set up stop button
        stopButton.setButtonText("Stop");
        stopButton.onClick = [this] { stopButtonClicked(); };
        addAndMakeVisible(stopButton);
        
        // Set up frequency slider
        frequencyLabel.setText("Frequency (Hz)", juce::dontSendNotification);
        frequencyLabel.setJustificationType(juce::Justification::centred);
        addAndMakeVisible(frequencyLabel);
        
        frequencySlider.setRange(50.0, 5000.0, 1.0);
        frequencySlider.setValue(440.0);
        frequencySlider.setSliderStyle(juce::Slider::SliderStyle::LinearHorizontal);
        frequencySlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 80, 20);
        frequencySlider.onValueChange = [this] { frequencySliderChanged(); };
        addAndMakeVisible(frequencySlider);
        
        // Set up amplitude slider
        amplitudeLabel.setText("Amplitude", juce::dontSendNotification);
        amplitudeLabel.setJustificationType(juce::Justification::centred);
        addAndMakeVisible(amplitudeLabel);
        
        amplitudeSlider.setRange(0.0, 1.0, 0.01);
        amplitudeSlider.setValue(0.5);
        amplitudeSlider.setSliderStyle(juce::Slider::SliderStyle::LinearHorizontal);
        amplitudeSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 80, 20);
        amplitudeSlider.onValueChange = [this] { amplitudeSliderChanged(); };
        addAndMakeVisible(amplitudeSlider);
    }
    
    void paint(juce::Graphics& g) override
    {
        // Fill background with dark color
        g.fillAll(juce::Colour(40, 40, 40));
        
        // Draw test tone section title
        g.setColour(juce::Colours::lightblue);
        g.setFont(juce::Font(24.0f, juce::Font::bold));
        g.drawText("Test Tone Generator", getLocalBounds().removeFromTop(50), juce::Justification::centred, true);
    }
    
    void resized() override
    {
        // Layout constants
        const int margin = 20;
        const int buttonHeight = 40;
        const int sliderHeight = 24;
        const int labelHeight = 20;
        const int spacing = 10;
        
        // Get component bounds
        auto bounds = getLocalBounds();
        bounds.reduce(margin, margin);
        
        // Reserve space for title
        bounds.removeFromTop(50);
        
        // Layout for buttons
        auto buttonArea = bounds.removeFromTop(buttonHeight);
        auto halfWidth = buttonArea.getWidth() / 2 - spacing / 2;
        
        startButton.setBounds(buttonArea.removeFromLeft(halfWidth));
        buttonArea.removeFromLeft(spacing);
        stopButton.setBounds(buttonArea);
        
        bounds.removeFromTop(spacing * 2);
        
        // Layout for frequency controls
        frequencyLabel.setBounds(bounds.removeFromTop(labelHeight));
        frequencySlider.setBounds(bounds.removeFromTop(sliderHeight));
        
        bounds.removeFromTop(spacing * 2);
        
        // Layout for amplitude controls
        amplitudeLabel.setBounds(bounds.removeFromTop(labelHeight));
        amplitudeSlider.setBounds(bounds.removeFromTop(sliderHeight));
    }
    
private:
    audio::AudioEngine& engine;
    
    juce::TextButton startButton;
    juce::TextButton stopButton;
    juce::Slider frequencySlider;
    juce::Slider amplitudeSlider;
    juce::Label frequencyLabel;
    juce::Label amplitudeLabel;
    
    void startButtonClicked()
    {
        // Generate test tone based on current slider values
        float frequency = static_cast<float>(frequencySlider.getValue());
        float amplitude = static_cast<float>(amplitudeSlider.getValue());
        
        engine.generateTestTone(frequency, amplitude);
        engine.start();
    }
    
    void stopButtonClicked()
    {
        engine.stop();
    }
    
    void frequencySliderChanged()
    {
        // Update frequency if audio engine is running
        if (engine.isRunning())
        {
            float frequency = static_cast<float>(frequencySlider.getValue());
            float amplitude = static_cast<float>(amplitudeSlider.getValue());
            
            engine.generateTestTone(frequency, amplitude);
        }
    }
    
    void amplitudeSliderChanged()
    {
        // Update amplitude if audio engine is running
        if (engine.isRunning())
        {
            float frequency = static_cast<float>(frequencySlider.getValue());
            float amplitude = static_cast<float>(amplitudeSlider.getValue());
            
            engine.generateTestTone(frequency, amplitude);
        }
    }
};

MainComponent::MainComponent(audio::AudioEngine& engine)
    : audioEngine(engine),
      tabbedComponent(juce::TabbedButtonBar::TabsAtTop),
      sampleLoaderComponent(engine)
{
    // Configure tabbed component
    tabbedComponent.setOutline(0);
    tabbedComponent.setTabBarDepth(30);
    tabbedComponent.setCurrentTabIndex(0);
    addAndMakeVisible(tabbedComponent);
    
    // Create and add the test tone tab
    auto* testToneComponent = new TestToneComponent(audioEngine);
    tabbedComponent.addTab("Test Tone", juce::Colours::darkgrey, testToneComponent, true);
    
    // Add the sample player tab
    tabbedComponent.addTab("Sample Player", juce::Colours::darkgrey, &sampleLoaderComponent, false);
    
    // Set component size
    setSize(800, 600);
}

MainComponent::~MainComponent()
{
    // Stop the audio engine when component is destroyed
    audioEngine.stop();
}

void MainComponent::paint(juce::Graphics& g)
{
    // Fill background with dark color
    g.fillAll(juce::Colour(30, 30, 30));
    
    // Draw logo or title
    g.setColour(juce::Colours::electricBlue);
    g.setFont(juce::Font(36.0f, juce::Font::bold));
    g.drawText("Underground Beats", getLocalBounds(), juce::Justification::centredTop, true);
    
    // Draw version
    g.setColour(juce::Colours::grey);
    g.setFont(juce::Font(12.0f));
    g.drawText("v0.1.0", getLocalBounds().removeFromBottom(20), juce::Justification::bottomRight, true);
}

void MainComponent::resized()
{
    // Layout constants
    const int margin = 20;
    const int titleHeight = 60;
    
    // Get component bounds
    auto bounds = getLocalBounds();
    bounds.reduce(margin, margin);
    
    // Reserve space for title
    bounds.removeFromTop(titleHeight);
    
    // Set the tabbed component to fill the remaining space
    tabbedComponent.setBounds(bounds);
}

} // namespace gui
} // namespace undergroundBeats