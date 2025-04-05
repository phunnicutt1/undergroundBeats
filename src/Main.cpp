#include "../include/undergroundBeats/UndergroundBeatsProcessor.h"
#include "../include/undergroundBeats/gui/MainEditor.h" // Include the editor header
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_utils/juce_audio_utils.h> // Needed for AudioAppComponent base

//==============================================================================
class UndergroundBeatsApplication : public juce::JUCEApplication
{
public:
    UndergroundBeatsApplication() = default;

    const juce::String getApplicationName() override { return "UndergroundBeats"; }
    const juce::String getApplicationVersion() override { return "1.0.0"; }
    bool moreThanOneInstanceAllowed() override { return true; }

    //==============================================================================
    void initialise(const juce::String& commandLine) override
    {
        juce::ignoreUnused(commandLine);
        // Initialize the global AudioDeviceManager *before* creating the window/component
        // that might use it.
        // NOTE: This is typically handled by AudioAppComponent derived classes, removing explicit call.
        
        mainWindow = std::make_unique<MainWindow>(getApplicationName());
    }

    void shutdown() override
    {
        // Before shutting down, make sure the main window is deleted.
        // This will trigger the cleanup of the processor and editor/component.
        mainWindow = nullptr;
        // Clean up the global device manager singleton instance
        // NOTE: This is typically handled by AudioAppComponent derived classes, removing explicit call.
    }

    //==============================================================================
    void systemRequestedQuit() override
    {
        quit();
    }

    void anotherInstanceStarted(const juce::String& commandLine) override
    {
        juce::ignoreUnused(commandLine);
    }

    //==============================================================================
    /*
        This class implements the desktop window that contains an instance of
        our MainEditor.
    */
    class MainWindow : public juce::DocumentWindow
    {
    public:
        MainWindow(juce::String name)
            : DocumentWindow(name,
                             juce::Desktop::getInstance().getDefaultLookAndFeel()
                                                          .findColour(juce::ResizableWindow::backgroundColourId),
                             DocumentWindow::allButtons)
        {
            setUsingNativeTitleBar(true);
            
            // Create the processor instance *before* the editor
            processor = std::make_unique<undergroundBeats::UndergroundBeatsProcessor>();

            // Create the editor, which now internally creates the MainComponent
            // that handles audio setup via AudioAppComponent.
            juce::AudioProcessorEditor* editor = processor->createEditorIfNeeded(); // Returns MainEditor*
            // Check if editor creation was successful
            if (editor != nullptr) {
                setContentOwned(editor, true); // DocumentWindow takes ownership
            } else {
                // Handle error: Editor creation failed
                // For now, just log or assert
                jassertfalse; 
            }


           #if JUCE_IOS || JUCE_ANDROID
            setFullScreen(true);
           #else
            setResizable(true, true);
            // Use the editor's size if available, otherwise a default
            if (editor)
                centreWithSize(editor->getWidth(), editor->getHeight());
            else
                centreWithSize(600, 400); // Default size if editor failed
           #endif

            setVisible(true);

            // Audio setup is now handled automatically by MainComponent inheriting AudioAppComponent
        }

        ~MainWindow() override
        {
            // The DocumentWindow automatically deletes the content component (MainEditor).
            // The MainEditor owns the MainComponent.
            // The MainComponent's destructor calls shutdownAudio().
            // We just need to ensure the processor is deleted after the editor.
            // Unique_ptr handles this automatically when mainWindow goes out of scope in shutdown().
            // Explicitly reset content component before processor if needed, but setContentOwned should handle it.
            // setContentOwned(nullptr, true); // Optional: Explicitly release editor ownership
            processor = nullptr; // Ensure processor is destroyed after editor/component
        }

        void closeButtonPressed() override
        {
            // This is called when the user tries to close this window. Here, we'll just
            // ask the app to quit when this happens, but you can change this to do
            // whatever you need.
            JUCEApplication::getInstance()->systemRequestedQuit();
        }

    private:
        //==============================================================================
        std::unique_ptr<undergroundBeats::UndergroundBeatsProcessor> processor;

        // Note: No explicit AudioDeviceManager or AudioProcessorPlayer needed here anymore.
        // MainComponent, derived from AudioAppComponent, manages the audio device interaction.

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainWindow)
    };

private:
    std::unique_ptr<MainWindow> mainWindow;
};

//==============================================================================
// This macro generates the main() routine that launches the app.
START_JUCE_APPLICATION(UndergroundBeatsApplication)
