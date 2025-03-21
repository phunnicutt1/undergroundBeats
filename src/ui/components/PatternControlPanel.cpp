/*
 * Underground Beats
 * PatternControlPanel.cpp
 * 
 * Implementation of the PatternControlPanel class
 */

#include "PatternControlPanel.h"

namespace UndergroundBeats {

PatternControlPanel::PatternControlPanel()
    : pattern(nullptr),
      hasSelectedNote(false),
      selectedNote(0),
      selectedNoteStart(0.0),
      selectedNoteDuration(0.0)
{
    // Create and set up controls
    createControls();
}

PatternControlPanel::~PatternControlPanel()
{
    // Remove listeners
    lengthSlider.removeListener(this);
    noteSlider.removeListener(this);
    velocitySlider.removeListener(this);
    startTimeSlider.removeListener(this);
    durationSlider.removeListener(this);
    quantizeButton.removeListener(this);
    nameEditor.removeListener(this);
}

void PatternControlPanel::setPattern(Pattern* newPattern)
{
    pattern = newPattern;
    updateControls();
}

void PatternControlPanel::setNoteProperties(int note, double startTime, double duration)
{
    selectedNote = note;
    selectedNoteStart = startTime;
    selectedNoteDuration = duration;
    hasSelectedNote = true;
    
    updateNoteControls();
}

void PatternControlPanel::resized()
{
    // Set up a grid layout
    auto area = getLocalBounds().reduced(10);
    
    // Two-column layout
    int leftColumnWidth = area.getWidth() / 2 - 5;
    auto leftColumn = area.removeFromLeft(leftColumnWidth);
    area.removeFromLeft(10); // Spacing between columns
    auto rightColumn = area;
    
    // Pattern settings in left column
    auto nameArea = leftColumn.removeFromTop(30);
    nameLabel.setBounds(nameArea.removeFromLeft(80));
    nameEditor.setBounds(nameArea);
    
    leftColumn.removeFromTop(10); // Spacing
    
    auto lengthArea = leftColumn.removeFromTop(30);
    lengthLabel.setBounds(lengthArea.removeFromLeft(80));
    lengthSlider.setBounds(lengthArea);
    
    leftColumn.removeFromTop(10); // Spacing
    
    // Quantization in left column
    auto quantizeArea = leftColumn.removeFromTop(30);
    quantizeLabel.setBounds(quantizeArea.removeFromLeft(80));
    quantizeComboBox.setBounds(quantizeArea.removeFromLeft(100));
    quantizeButton.setBounds(quantizeArea);
    
    // Note properties in right column
    auto noteArea = rightColumn.removeFromTop(30);
    noteLabel.setBounds(noteArea.removeFromLeft(80));
    noteSlider.setBounds(noteArea);
    
    rightColumn.removeFromTop(10); // Spacing
    
    auto velocityArea = rightColumn.removeFromTop(30);
    velocityLabel.setBounds(velocityArea.removeFromLeft(80));
    velocitySlider.setBounds(velocityArea);
    
    rightColumn.removeFromTop(10); // Spacing
    
    auto startTimeArea = rightColumn.removeFromTop(30);
    startTimeLabel.setBounds(startTimeArea.removeFromLeft(80));
    startTimeSlider.setBounds(startTimeArea);
    
    rightColumn.removeFromTop(10); // Spacing
    
    auto durationArea = rightColumn.removeFromTop(30);
    durationLabel.setBounds(durationArea.removeFromLeft(80));
    durationSlider.setBounds(durationArea);
}

void PatternControlPanel::paint(juce::Graphics& g)
{
    // Background fill
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId).brighter(0.1f));
    
    // Border
    g.setColour(juce::Colours::grey);
    g.drawRect(getLocalBounds().toFloat(), 1.0f);
    
    // Section titles
    g.setColour(juce::Colours::white);
    g.setFont(juce::Font(14.0f, juce::Font::bold));
    
    auto area = getLocalBounds().reduced(10);
    int leftColumnWidth = area.getWidth() / 2 - 5;
    
    g.drawText("Pattern Settings", 10, 5, leftColumnWidth, 20, 
              juce::Justification::centredLeft, true);
    
    g.drawText("Note Properties", leftColumnWidth + 20, 5, leftColumnWidth, 20, 
              juce::Justification::centredLeft, true);
}

void PatternControlPanel::sliderValueChanged(juce::Slider* slider)
{
    if (!pattern)
        return;
    
    if (slider == &lengthSlider)
    {
        // Update pattern length
        // In a real implementation, this would update the pattern length
        sendChangeMessage();
    }
    else if (slider == &noteSlider)
    {
        // Update note
        if (hasSelectedNote)
        {
            selectedNote = static_cast<int>(noteSlider.getValue());
            sendChangeMessage();
        }
    }
    else if (slider == &velocitySlider)
    {
        // Update velocity
        if (hasSelectedNote)
        {
            // Update note velocity
            sendChangeMessage();
        }
    }
    else if (slider == &startTimeSlider)
    {
        // Update start time
        if (hasSelectedNote)
        {
            selectedNoteStart = startTimeSlider.getValue();
            sendChangeMessage();
        }
    }
    else if (slider == &durationSlider)
    {
        // Update duration
        if (hasSelectedNote)
        {
            selectedNoteDuration = durationSlider.getValue();
            sendChangeMessage();
        }
    }
}

void PatternControlPanel::buttonClicked(juce::Button* button)
{
    if (button == &quantizeButton)
    {
        // Quantize notes
        if (pattern)
        {
            // In a real implementation, this would quantize the notes
            sendChangeMessage();
        }
    }
}

void PatternControlPanel::textEditorTextChanged(juce::TextEditor& editor)
{
    if (&editor == &nameEditor && pattern)
    {
        // Update pattern name
        // In a real implementation, this would update the pattern name
    }
}

void PatternControlPanel::textEditorReturnKeyPressed(juce::TextEditor& editor)
{
    if (&editor == &nameEditor && pattern)
    {
        // Update pattern name and notify
        // In a real implementation, this would update the pattern name
        sendChangeMessage();
    }
}

void PatternControlPanel::textEditorFocusLost(juce::TextEditor& editor)
{
    if (&editor == &nameEditor && pattern)
    {
        // Update pattern name and notify
        // In a real implementation, this would update the pattern name
        sendChangeMessage();
    }
}

void PatternControlPanel::createControls()
{
    // Pattern name controls
    nameLabel.setText("Name:", juce::dontSendNotification);
    nameEditor.setMultiLine(false);
    nameEditor.setReturnKeyStartsNewLine(false);
    nameEditor.setText("Untitled Pattern");
    nameEditor.addListener(this);
    
    // Pattern length controls
    lengthLabel.setText("Length:", juce::dontSendNotification);
    lengthSlider.setRange(1.0, 64.0, 1.0);
    lengthSlider.setValue(4.0);
    lengthSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 60, 20);
    lengthSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    lengthSlider.addListener(this);
    
    // Quantization controls
    quantizeLabel.setText("Quantize:", juce::dontSendNotification);
    quantizeComboBox.addItem("1/4", 1);
    quantizeComboBox.addItem("1/8", 2);
    quantizeComboBox.addItem("1/16", 3);
    quantizeComboBox.addItem("1/32", 4);
    quantizeComboBox.setSelectedId(3); // 1/16 by default
    quantizeButton.setButtonText("Apply");
    quantizeButton.addListener(this);
    
    // Note controls
    noteLabel.setText("Note:", juce::dontSendNotification);
    noteSlider.setRange(0.0, 127.0, 1.0);
    noteSlider.setValue(60.0); // Middle C by default
    noteSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 60, 20);
    noteSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    noteSlider.addListener(this);
    
    // Velocity controls
    velocityLabel.setText("Velocity:", juce::dontSendNotification);
    velocitySlider.setRange(1.0, 127.0, 1.0);
    velocitySlider.setValue(100.0);
    velocitySlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 60, 20);
    velocitySlider.setSliderStyle(juce::Slider::LinearHorizontal);
    velocitySlider.addListener(this);
    
    // Start time controls
    startTimeLabel.setText("Start Time:", juce::dontSendNotification);
    startTimeSlider.setRange(0.0, 64.0, 0.25);
    startTimeSlider.setValue(0.0);
    startTimeSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 60, 20);
    startTimeSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    startTimeSlider.addListener(this);
    
    // Duration controls
    durationLabel.setText("Duration:", juce::dontSendNotification);
    durationSlider.setRange(0.25, 16.0, 0.25);
    durationSlider.setValue(1.0);
    durationSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 60, 20);
    durationSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    durationSlider.addListener(this);
    
    // Add components
    addAndMakeVisible(nameLabel);
    addAndMakeVisible(nameEditor);
    addAndMakeVisible(lengthLabel);
    addAndMakeVisible(lengthSlider);
    addAndMakeVisible(quantizeLabel);
    addAndMakeVisible(quantizeComboBox);
    addAndMakeVisible(quantizeButton);
    addAndMakeVisible(noteLabel);
    addAndMakeVisible(noteSlider);
    addAndMakeVisible(velocityLabel);
    addAndMakeVisible(velocitySlider);
    addAndMakeVisible(startTimeLabel);
    addAndMakeVisible(startTimeSlider);
    addAndMakeVisible(durationLabel);
    addAndMakeVisible(durationSlider);
    
    // Initially disable note controls until a note is selected
    noteSlider.setEnabled(false);
    velocitySlider.setEnabled(false);
    startTimeSlider.setEnabled(false);
    durationSlider.setEnabled(false);
}

void PatternControlPanel::updateControls()
{
    if (pattern)
    {
        // Update the pattern name and length
        // In a real implementation, this would get values from the pattern
        nameEditor.setText("Untitled Pattern", false);
        lengthSlider.setValue(4.0, juce::dontSendNotification);
    }
}

void PatternControlPanel::updateNoteControls()
{
    // Enable note controls
    noteSlider.setEnabled(hasSelectedNote);
    velocitySlider.setEnabled(hasSelectedNote);
    startTimeSlider.setEnabled(hasSelectedNote);
    durationSlider.setEnabled(hasSelectedNote);
    
    if (hasSelectedNote)
    {
        // Update note control values
        noteSlider.setValue(selectedNote, juce::dontSendNotification);
        velocitySlider.setValue(100, juce::dontSendNotification); // Default velocity
        startTimeSlider.setValue(selectedNoteStart, juce::dontSendNotification);
        durationSlider.setValue(selectedNoteDuration, juce::dontSendNotification);
    }
}

} // namespace UndergroundBeats
