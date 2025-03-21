#pragma once

#include <JuceHeader.h>
#include "audio-engine/Engine.h"
#include "synthesis/Oscillator.h"
#include "synthesis/Envelope.h"
#include "synthesis/Filter.h"
#include "effects/Delay.h"
#include "effects/Reverb.h"
#include "sequencer/Sequencer.h"
#include "sequencer/MidiEngine.h"
#include "project/ProjectManager.h"
#include "ui/views/MixerView.h"
#include "ui/views/PatternEditorView.h"

using namespace UndergroundBeats;

//==============================================================================
class MainComponent  : public juce::AudioAppComponent,
                       private juce::Timer,
                       private juce::ChangeListener
{
public:
    //==============================================================================
    MainComponent();
    ~MainComponent() override;

    //==============================================================================
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    //==============================================================================
    void paint (juce::Graphics& g) override;
    void resized() override;
    
    // Timer callback for handling envelope trigger
    void timerCallback() override;
    
    // ChangeListener callback for project changes
    void changeListenerCallback(juce::ChangeBroadcaster* source) override;

private:
    //==============================================================================
    Engine audioEngine;
    std::unique_ptr<Oscillator> oscillator;
    std::unique_ptr<Envelope> envelope;
    std::unique_ptr<Filter> filter;
    std::unique_ptr<Delay> delay;
    std::unique_ptr<Reverb> reverb;
    
    // Sequencer components
    std::unique_ptr<Sequencer> sequencer;
    std::unique_ptr<MidiEngine> midiEngine;
    std::shared_ptr<Timeline> timeline;
    std::unique_ptr<ProjectManager> projectManager;
    
    // UI components
    juce::TextButton startButton { "Start Engine" };
    
    // Main menu bar
    juce::MenuBarComponent menuBar;
    
    // Oscillator controls
    juce::Slider frequencySlider;
    juce::Label frequencyLabel { {}, "Frequency (Hz)" };
    juce::ComboBox waveformSelector;
    juce::Label waveformLabel { {}, "Waveform" };
    juce::Slider pulseWidthSlider;
    juce::Label pulseWidthLabel { {}, "Pulse Width" };
    juce::Slider detuneSlider;
    juce::Label detuneLabel { {}, "Detune (cents)" };
    juce::Slider gainSlider;
    juce::Label gainLabel { {}, "Gain" };
    
    // Envelope controls
    juce::Slider attackSlider;
    juce::Label attackLabel { {}, "Attack (ms)" };
    juce::Slider decaySlider;
    juce::Label decayLabel { {}, "Decay (ms)" };
    juce::Slider sustainSlider;
    juce::Label sustainLabel { {}, "Sustain" };
    juce::Slider releaseSlider;
    juce::Label releaseLabel { {}, "Release (ms)" };
    juce::TextButton triggerButton { "Trigger" };
    
    // Filter controls
    juce::Slider filterFreqSlider;
    juce::Label filterFreqLabel { {}, "Filter Freq (Hz)" };
    juce::Slider resonanceSlider;
    juce::Label resonanceLabel { {}, "Resonance" };
    juce::ComboBox filterTypeSelector;
    juce::Label filterTypeLabel { {}, "Filter Type" };
    
    // Effect selector
    juce::ComboBox effectSelector;
    juce::Label effectLabel { {}, "Effect" };
    
    // Delay controls
    juce::Slider delayTimeSlider;
    juce::Label delayTimeLabel { {}, "Delay Time (ms)" };
    juce::Slider feedbackSlider;
    juce::Label feedbackLabel { {}, "Feedback" };
    juce::Slider delayMixSlider;
    juce::Label delayMixLabel { {}, "Mix" };
    
    // Reverb controls
    juce::Slider roomSizeSlider;
    juce::Label roomSizeLabel { {}, "Room Size" };
    juce::Slider dampingSlider;
    juce::Label dampingLabel { {}, "Damping" };
    juce::Slider widthSlider;
    juce::Label widthLabel { {}, "Width" };
    juce::Slider reverbMixSlider;
    juce::Label reverbMixLabel { {}, "Mix" };
    
    // Sequencer controls
    juce::TextButton playButton { "Play" };
    juce::TextButton stopButton { "Stop" };
    juce::TextButton recordButton { "Record" };
    juce::Slider tempoSlider;
    juce::Label tempoLabel { {}, "Tempo (BPM)" };
    
    // Tab component for different sections
    juce::TabbedComponent tabs;
    
    // Container components for each tab
    std::unique_ptr<juce::Component> oscillatorTab;
    std::unique_ptr<juce::Component> envelopeTab;
    std::unique_ptr<juce::Component> filterTab;
    std::unique_ptr<juce::Component> effectsTab;
    std::unique_ptr<juce::Component> sequencerTab;
    std::unique_ptr<juce::Component> mixerTab;
    
    // View components
    std::unique_ptr<MixerView> mixerView;
    std::unique_ptr<PatternEditorView> patternEditorView;
    
    // Node IDs for the processor graph
    NodeID oscillatorNodeId;
    NodeID envelopeNodeId;
    NodeID filterNodeId;
    NodeID delayNodeId;
    NodeID reverbNodeId;
    
    // Menu creation and handling
    juce::ApplicationCommandManager commandManager;
    juce::StringArray getMenuBarNames();
    juce::PopupMenu getMenuForIndex(int menuIndex, const juce::String& menuName);
    void menuItemSelected(int menuItemID, int topLevelMenuIndex);
    
    // Command IDs for menu items
    enum CommandIDs
    {
        NewProject = 1,
        OpenProject,
        SaveProject,
        SaveProjectAs,
        ExportAudio,
        Preferences,
        
        Undo,
        Redo,
        Cut,
        Copy,
        Paste,
        Delete,
        SelectAll,
        
        AddPattern,
        AddTrack,
        DeletePattern,
        DeleteTrack
    };
    
    // Create processors
    void createOscillator();
    void createEnvelope();
    void createFilter();
    void createDelay();
    void createReverb();
    
    // Connect processors in the graph
    void connectProcessors();
    
    // Update UI based on current effect
    void updateEffectsUI();
    
    // Project management
    void newProject();
    void openProject();
    void saveProject(bool saveAs = false);
    void exportAudio();
    
    // Handle sequencer events
    void handleNoteEvent(const NoteEvent& event);
    void handleParameterChange(const std::string& paramId, float value);
    
    // Active effect type
    enum EffectType
    {
        NoEffect,
        DelayEffect,
        ReverbEffect
    };
    
    EffectType currentEffect = NoEffect;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
