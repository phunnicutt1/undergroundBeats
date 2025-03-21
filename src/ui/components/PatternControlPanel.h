/*
 * Underground Beats
 * PatternControlPanel.h
 * 
 * Control panel for pattern settings
 */

#pragma once

#include <JuceHeader.h>
#include "../../sequencer/Pattern.h"

namespace UndergroundBeats {

/**
 * @class PatternControlPanel
 * @brief Control panel for pattern settings
 * 
 * The PatternControlPanel class provides controls for pattern settings
 * such as length, name, and selected note properties.
 */
class PatternControlPanel : public juce::Component,
                           public juce::Slider::Listener,
                           public juce::Button::Listener,
                           public juce::TextEditor::Listener,
                           public juce::ChangeBroadcaster {
public:
    PatternControlPanel();
    ~PatternControlPanel() override;
    
    /**
     * @brief Set the pattern to edit
     * 
     * @param pattern The pattern to edit
     */
    void setPattern(Pattern* pattern);
    
    /**
     * @brief Set the properties of the selected note
     * 
     * @param note MIDI note number
     * @param startTime Start time in beats
     * @param duration Duration in beats
     */
    void setNoteProperties(int note, double startTime, double duration);
    
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
     * @brief Slider value changed callback
     * 
     * @param slider The slider that changed
     */
    void sliderValueChanged(juce::Slider* slider) override;
    
    /**
     * @brief Button clicked callback
     * 
     * @param button The button that was clicked
     */
    void buttonClicked(juce::Button* button) override;
    
    /**
     * @brief Text editor text changed callback
     * 
     * @param editor The text editor that changed
     */
    void textEditorTextChanged(juce::TextEditor& editor) override;
    
    /**
     * @brief Text editor return key pressed callback
     * 
     * @param editor The text editor where return was pressed
     */
    void textEditorReturnKeyPressed(juce::TextEditor& editor) override;
    
    /**
     * @brief Text editor focus lost callback
     * 
     * @param editor The text editor that lost focus
     */
    void textEditorFocusLost(juce::TextEditor& editor) override;
    
private:
    Pattern* pattern;
    
    // Pattern settings
    juce::Label nameLabel;
    juce::TextEditor nameEditor;
    juce::Label lengthLabel;
    juce::Slider lengthSlider;
    
    // Note properties
    juce::Label noteLabel;
    juce::Slider noteSlider;
    juce::Label velocityLabel;
    juce::Slider velocitySlider;
    juce::Label startTimeLabel;
    juce::Slider startTimeSlider;
    juce::Label durationLabel;
    juce::Slider durationSlider;
    
    // Quantization settings
    juce::Label quantizeLabel;
    juce::ComboBox quantizeComboBox;
    juce::TextButton quantizeButton;
    
    // Note selection state
    bool hasSelectedNote;
    int selectedNote;
    double selectedNoteStart;
    double selectedNoteDuration;
    
    // Create and set up the controls
    void createControls();
    
    // Update the controls from the pattern
    void updateControls();
    
    // Update the note controls from the selected note
    void updateNoteControls();
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PatternControlPanel)
};

} // namespace UndergroundBeats
