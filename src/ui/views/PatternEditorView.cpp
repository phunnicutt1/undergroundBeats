/*
 * Underground Beats
 * PatternEditorView.cpp
 * 
 * Implementation of the PatternEditorView class
 */

#include "PatternEditorView.h"

namespace UndergroundBeats {

PatternEditorView::PatternEditorView()
    : sequencer(nullptr)
{
    setupComponents();
}

PatternEditorView::~PatternEditorView()
{
    // Remove listeners
    newPatternButton.removeListener(this);
    deletePatternButton.removeListener(this);
    duplicatePatternButton.removeListener(this);
}

void PatternEditorView::setSequencer(Sequencer* seq)
{
    sequencer = seq;
    
    // Update pattern list
    updatePatternList();
}

void PatternEditorView::setPattern(std::shared_ptr<Pattern> pattern)
{
    currentPattern = pattern;
    
    if (pattern && patternEditor)
    {
        patternEditor->setPattern(pattern.get());
    }
    
    if (pattern && controlPanel)
    {
        controlPanel->setPattern(pattern.get());
    }
}

void PatternEditorView::resized()
{
    auto area = getLocalBounds();
    
    // Pattern list on the left
    auto leftPanel = area.removeFromLeft(200);
    
    // Buttons at the bottom of left panel
    auto buttonRow = leftPanel.removeFromBottom(30);
    auto buttonWidth = buttonRow.getWidth() / 3;
    
    newPatternButton.setBounds(buttonRow.removeFromLeft(buttonWidth).reduced(2));
    duplicatePatternButton.setBounds(buttonRow.removeFromLeft(buttonWidth).reduced(2));
    deletePatternButton.setBounds(buttonRow.reduced(2));
    
    // Pattern list takes the rest of the left panel
    patternList.setBounds(leftPanel);
    
    // Control panel at the top of the main area
    auto controlPanelArea = area.removeFromTop(120);
    if (controlPanel)
    {
        controlPanel->setBounds(controlPanelArea);
    }
    
    // Pattern editor takes the rest of the main area
    if (patternEditor)
    {
        patternEditor->setBounds(area);
    }
}

void PatternEditorView::paint(juce::Graphics& g)
{
    // Background fill
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
    
    // Border
    g.setColour(juce::Colours::grey);
    g.drawRect(getLocalBounds().toFloat(), 1.0f);
}

void PatternEditorView::changeListenerCallback(juce::ChangeBroadcaster* source)
{
    // Handle changes from components
    if (source == patternEditor.get())
    {
        // Pattern editor has changed
        if (currentPattern)
        {
            // Update pattern
        }
    }
    else if (source == controlPanel.get())
    {
        // Control panel has changed
        if (currentPattern)
        {
            // Update pattern
            if (patternEditor)
            {
                patternEditor->updateDisplay();
            }
        }
    }
}

void PatternEditorView::buttonClicked(juce::Button* button)
{
    if (button == &newPatternButton)
    {
        // Create a new pattern
        if (sequencer)
        {
            // In a real implementation, this would create a new pattern
            // in the sequencer and update the pattern list
            juce::AlertWindow::showMessageBoxAsync(juce::AlertWindow::InfoIcon,
                "New Pattern",
                "This would create a new pattern in a real implementation.");
            
            // Update pattern list
            updatePatternList();
        }
    }
    else if (button == &deletePatternButton)
    {
        // Delete the current pattern
        if (sequencer && currentPattern)
        {
            // In a real implementation, this would delete the pattern
            // from the sequencer and update the pattern list
            juce::AlertWindow::showMessageBoxAsync(juce::AlertWindow::InfoIcon,
                "Delete Pattern",
                "This would delete the current pattern in a real implementation.");
            
            // Update pattern list
            updatePatternList();
        }
    }
    else if (button == &duplicatePatternButton)
    {
        // Duplicate the current pattern
        if (sequencer && currentPattern)
        {
            // In a real implementation, this would duplicate the pattern
            // in the sequencer and update the pattern list
            juce::AlertWindow::showMessageBoxAsync(juce::AlertWindow::InfoIcon,
                "Duplicate Pattern",
                "This would duplicate the current pattern in a real implementation.");
            
            // Update pattern list
            updatePatternList();
        }
    }
}

void PatternEditorView::setupComponents()
{
    // Create pattern editor
    patternEditor = std::make_unique<PatternEditor>();
    addAndMakeVisible(patternEditor.get());
    patternEditor->addChangeListener(this);
    
    // Create control panel
    controlPanel = std::make_unique<PatternControlPanel>();
    addAndMakeVisible(controlPanel.get());
    controlPanel->addChangeListener(this);
    
    // Set up pattern list
    listModel = std::make_unique<PatternListModel>(patternNames, *this);
    patternList.setModel(listModel.get());
    patternList.setRowHeight(24);
    patternList.setColour(juce::ListBox::outlineColourId, juce::Colours::grey);
    patternList.setMultipleSelectionEnabled(false);
    addAndMakeVisible(patternList);
    
    // Set up buttons
    newPatternButton.setButtonText("New");
    deletePatternButton.setButtonText("Delete");
    duplicatePatternButton.setButtonText("Duplicate");
    
    addAndMakeVisible(newPatternButton);
    addAndMakeVisible(deletePatternButton);
    addAndMakeVisible(duplicatePatternButton);
    
    newPatternButton.addListener(this);
    deletePatternButton.addListener(this);
    duplicatePatternButton.addListener(this);
    
    // Populate pattern list with dummy data
    patternNames.add("Pattern 1");
    patternNames.add("Pattern 2");
    patternNames.add("Pattern 3");
    patternList.updateContent();
}

void PatternEditorView::updatePatternList()
{
    // In a real implementation, this would get pattern names from the sequencer
    patternList.updateContent();
    patternList.repaint();
}

void PatternEditorView::handleNoteSelection(int note, double time, double duration)
{
    // Handle note selection from the pattern editor
    if (currentPattern)
    {
        // Update the control panel with the selected note's properties
        controlPanel->setNoteProperties(note, time, duration);
    }
}

//==============================================================================
// PatternListModel implementation
//==============================================================================

void PatternEditorView::PatternListModel::paintListBoxItem(int rowNumber, juce::Graphics& g,
                                                         int width, int height, bool rowIsSelected)
{
    if (rowNumber >= 0 && rowNumber < patternNames.size())
    {
        if (rowIsSelected)
        {
            g.fillAll(juce::Colours::lightblue);
        }
        
        g.setColour(rowIsSelected ? juce::Colours::black : juce::Colours::white);
        g.setFont(14.0f);
        g.drawText(patternNames[rowNumber], 5, 0, width - 10, height, juce::Justification::centredLeft, true);
    }
}

void PatternEditorView::PatternListModel::selectedRowsChanged(int lastRowSelected)
{
    if (lastRowSelected >= 0 && lastRowSelected < patternNames.size())
    {
        // In a real implementation, this would set the current pattern
        // in the sequencer and update the pattern editor
        
        // For now, just show a message
        juce::AlertWindow::showMessageBoxAsync(juce::AlertWindow::InfoIcon,
            "Pattern Selected",
            "Selected pattern: " + patternNames[lastRowSelected]);
    }
}

} // namespace UndergroundBeats
