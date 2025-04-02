#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "undergroundBeats/audio/AudioEngine.h"

namespace undergroundBeats {
namespace core {

/**
 * @class Application
 * @brief Main application class for Underground Beats.
 * 
 * This class serves as the entry point for the application and manages
 * the overall lifecycle of the app including audio processing and UI.
 */
class Application : public juce::JUCEApplication
{
public:
    /**
     * @brief Constructor for the Application class.
     */
    Application();
    
    /**
     * @brief Destructor for the Application class.
     */
    ~Application() override;

    /**
     * @brief Get the application name.
     * @return The name of the application.
     */
    const juce::String getApplicationName() override;
    
    /**
     * @brief Get the application version.
     * @return The version of the application.
     */
    const juce::String getApplicationVersion() override;
    
    /**
     * @brief Initialize the application.
     * @param commandLineParameters Parameters passed from the command line.
     */
    void initialise(const juce::String& commandLineParameters) override;
    
    /**
     * @brief Shutdown the application.
     */
    void shutdown() override;
    
    /**
     * @brief Handle system requests to quit.
     */
    void systemRequestedQuit() override;
    
    /**
     * @brief Get the current instance of the application.
     * @return A pointer to the current Application instance.
     */
    static Application* getInstance();
    
    /**
     * @brief Get the audio engine.
     * @return A reference to the audio engine.
     */
    audio::AudioEngine& getAudioEngine();

private:
    std::unique_ptr<audio::AudioEngine> audioEngine;
    std::unique_ptr<juce::DocumentWindow> mainWindow;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Application)
};

} // namespace core
} // namespace undergroundBeats