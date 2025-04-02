#include "undergroundBeats/core/Application.h"
#include "undergroundBeats/gui/MainComponent.h"
#include <juce_gui_basics/juce_gui_basics.h>

namespace undergroundBeats {
namespace core {

// Main window component
class MainWindow : public juce::DocumentWindow
{
public:
    MainWindow(const juce::String& name, audio::AudioEngine& audioEngine)
        : DocumentWindow(name,
                         juce::Desktop::getInstance().getDefaultLookAndFeel()
                            .findColour(ResizableWindow::backgroundColourId),
                         DocumentWindow::allButtons)
    {
        setUsingNativeTitleBar(true);
        setResizable(true, true);
        setContentOwned(new gui::MainComponent(audioEngine), true);
        
        // Center window on screen
        centreWithSize(800, 600);
        
        // Make window visible
        setVisible(true);
    }
    
    void closeButtonPressed() override
    {
        juce::JUCEApplication::getInstance()->systemRequestedQuit();
    }
    
private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainWindow)
};

// Application implementation
Application::Application()
{
    // Create and initialize the audio engine
    audioEngine = std::make_unique<audio::AudioEngine>();
}

Application::~Application()
{
    // Audio engine will be automatically deleted by unique_ptr
}

const juce::String Application::getApplicationName()
{
    return "Underground Beats";
}

const juce::String Application::getApplicationVersion()
{
    return "0.1.0";
}

void Application::initialise(const juce::String& /*commandLineParameters*/)
{
    // Initialize the audio engine
    if (!audioEngine->initialize())
    {
        // Handle audio initialization failure
        juce::AlertWindow::showMessageBoxAsync(
            juce::AlertWindow::WarningIcon,
            "Audio Initialization Error",
            "Failed to initialize audio system. Please check your audio device settings.",
            "OK");
    }
    
    // Initialize but don't start audio yet (UI will control this)
    
    // Create main window and pass audio engine to it
    mainWindow = std::make_unique<MainWindow>(getApplicationName(), *audioEngine);
}

void Application::shutdown()
{
    // Stop audio processing
    if (audioEngine)
    {
        audioEngine->stop();
    }
    
    // Delete main window (must be done before audio engine is destroyed)
    mainWindow = nullptr;
}

void Application::systemRequestedQuit()
{
    quit();
}

Application* Application::getInstance()
{
    return dynamic_cast<Application*>(juce::JUCEApplication::getInstance());
}

audio::AudioEngine& Application::getAudioEngine()
{
    return *audioEngine;
}

} // namespace core
} // namespace undergroundBeats