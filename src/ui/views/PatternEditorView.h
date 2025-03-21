/*
 * Underground Beats
 * PatternEditorView.h
 * 
 * Sequencer pattern editor view
 */

#pragma once

#include <JuceHeader.h>
#include "../../sequencer/Pattern.h"
#include "../../sequencer/Sequencer.h"
#include "../components/PatternEditor.h"
#include "../components/PatternControlPanel.h"
#include <memory>
#include <vector>

namespace UndergroundBeats {

/**
 * @class PatternEditorView
 * @brief Full pattern editor view with grid and controls
 * 
 * The PatternEditorView class provides a complete interface for editing
 * patterns, including a grid for note entry and controls for pattern settings.
 */
class PatternEditorView : public juce::Component,
                         public juce::ChangeListener,
                         public juce::Button::Listener {
public:
    PatternEditorView();
    ~PatternEditorView() override;
    
    /**
     * @brief Set the sequencer to use for playback
     * 
     * @param seq The sequencer
     */
    void setSequencer(Sequencer* seq);
    
    /**
     * @brief Set the current pattern to edit
     * 
     * @param pattern The pattern to edit
     */
    void setPattern(std::shared_ptr<Pattern> pattern);
    
    /**
     * @brief Component resized callback
     */
    void resized() override;
    
    /**
     * @brief Component paint callback
     * 
     * @param g Graphics context to paint to
     */
    void paint(juce::Graphics& g) override;
    
    /**
     * @brief Change listener callback
     * 
     * @param source The change broadcaster
     */
    void changeListenerCallback(juce::ChangeBroadcaster* source) override;
    
    /**
     * @brief Button clicked callback
     * 
     * @param button The button that was clicked
     */
    void buttonClicked(juce::Button* button) override;
    
private:
    Sequencer* sequencer;
    std::shared_ptr<Pattern> currentPattern;
    
    // Main components
    std::unique_ptr<PatternEditor> patternEditor;
    std::unique_ptr<PatternControlPanel> controlPanel;
    
    // Pattern list
    juce::ListBox patternList;
    juce::StringArray patternNames;
    
    // Control buttons
    juce::TextButton newPatternButton;
    juce::TextButton deletePatternButton;
    juce::TextButton duplicatePatternButton;
    
    // Set up the components
    void setupComponents();
    
    // Update the pattern list
    void updatePatternList();
    
    // Handle note selection
    void handleNoteSelection(int note, double time, double duration);
    
    // Pattern list model
    class PatternListModel : public juce::ListBoxModel {
    public:
        PatternListModel(juce::StringArray& names, PatternEditorView& owner)
            : patternNames(names), owner(owner) {}
        
        int getNumRows() override { return patternNames.size(); }
        
        void paintListBoxItem(int rowNumber, juce::Graphics& g, int width, int height, 
                              bool rowIsSelected) override;
        
        void selectedRowsChanged(int lastRowSelected) override;
        
    private:
        juce::StringArray& patternNames;
        PatternEditorView& owner;
    };
    
    std::unique_ptr<PatternListModel> listModel;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PatternEditorView)
};

} // namespace UndergroundBeats
