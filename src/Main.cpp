#include "undergroundBeats/UndergroundBeatsProcessor.h"
#include <juce_audio_devices/juce_audio_devices.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_utils/juce_audio_utils.h>

class UndergroundBeatsApplication : public juce::JUCEApplication {
public:
    UndergroundBeatsApplication() = default;
    
    const juce::String getApplicationName() override {
        return "Underground Beats";
    }
    
    const juce::String getApplicationVersion() override {
        return "1.0.0";
    }
    
    bool moreThanOneInstanceAllowed() override {
        return true;
    }
    
    void initialise(const juce::String& commandLine) override {
        // Initialize the main window
        mainWindow = std::make_unique<MainWindow>(getApplicationName());
    }
    
    void shutdown() override {
        // Clean up resources
        mainWindow = nullptr;
    }
    
    void systemRequestedQuit() override {
        quit();
    }
    
    void anotherInstanceStarted(const juce::String& commandLine) override {
        // Nothing to do for now
    }
    
    class MainWindow : public juce::DocumentWindow {
    public:
        MainWindow(juce::String name)
            : DocumentWindow(name,
                             juce::Colours::darkgrey,
                             DocumentWindow::allButtons) {
            // Create the processor and editor
            processor = std::make_unique<undergroundBeats::UndergroundBeatsProcessor>();
            setContentOwned(processor->createEditor(), true);
            
            // Center window on screen
            centreWithSize(getWidth(), getHeight());
            
            // Make window visible
            setVisible(true);
            
            // Setup audio device manager and audio device setup window
            setupAudio();
        }
        
        ~MainWindow() override {
            // Clean up resources
            processor = nullptr;
        }
        
        void closeButtonPressed() override {
            JUCEApplication::getInstance()->systemRequestedQuit();
        }
        
    private:
        void setupAudio() {
            // Initialize the audio device manager
            deviceManager.initialiseWithDefaultDevices(2, 2);
            
            // Create the audio processor player
            player.setProcessor(processor.get());
            
            // Connect the player to the device manager
            deviceManager.addAudioCallback(&player);
        }
        
        std::unique_ptr<undergroundBeats::UndergroundBeatsProcessor> processor;
        juce::AudioDeviceManager deviceManager;
        juce::AudioProcessorPlayer player;
        
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainWindow)
    };
    
private:
    std::unique_ptr<MainWindow> mainWindow;
};

// This creates the application instance
START_JUCE_APPLICATION(UndergroundBeatsApplication)
