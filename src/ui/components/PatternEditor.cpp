/*
 * Underground Beats
 * PatternEditor.cpp
 * 
 * Implementation of the PatternEditor class
 */

#include "PatternEditor.h"

namespace UndergroundBeats {

PatternEditor::PatternEditor()
    : pattern(nullptr),
      gridSize(0.25), // Default to 16th notes
      zoomX(30.0f),   // Pixels per beat
      zoomY(1.0f),
      scrollX(0.0),
      scrollY(60),    // Middle C by default
      dragging(false),
      resizing(false),
      selectedNote(-1),
      selectedNoteStart(0.0),
      selectedNoteDuration(0.0)
{
    // Set up the component
    setWantsKeyboardFocus(true);
    
    // We'll just create a minimal implementation for now to get the app running
}

PatternEditor::~PatternEditor()
{
    // Nothing to clean up
}

void PatternEditor::setPattern(Pattern* newPattern)
{
    pattern = newPattern;
    updateDisplay();
}

void PatternEditor::updateDisplay()
{
    repaint();
}

void PatternEditor::setGridSize(double newGridSize)
{
    gridSize = newGridSize;
    updateDisplay();
}

double PatternEditor::getGridSize() const
{
    return gridSize;
}

void PatternEditor::setZoom(float newZoomX, float newZoomY)
{
    zoomX = newZoomX;
    zoomY = newZoomY;
    updateDisplay();
}

float PatternEditor::getZoomX() const
{
    return zoomX;
}

float PatternEditor::getZoomY() const
{
    return zoomY;
}

void PatternEditor::setScrollPosition(double x, int y)
{
    scrollX = x;
    scrollY = y;
    updateDisplay();
}

double PatternEditor::getScrollPositionX() const
{
    return scrollX;
}

int PatternEditor::getScrollPositionY() const
{
    return scrollY;
}

void PatternEditor::setNoteSelectedCallback(std::function<void(int, double, double)> callback)
{
    noteSelectedCallback = callback;
}

void PatternEditor::resized()
{
    // Nothing to do for now
}

void PatternEditor::paint(juce::Graphics& g)
{
    // Background fill
    g.fillAll(juce::Colours::black);
    
    // For now, just draw a simple message
    g.setColour(juce::Colours::white);
    g.setFont(16.0f);
    g.drawText("Pattern Editor - Implementation in progress", 
               getLocalBounds().reduced(10), 
               juce::Justification::centred, true);
    
    // Draw the grid (minimal implementation)
    drawGrid(g);
    
    // Draw the keyboard
    drawKeyboard(g);
    
    // Draw the header
    drawHeader(g);
    
    // Draw the notes
    if (pattern != nullptr) {
        drawNotes(g);
    }
}

void PatternEditor::mouseDown(const juce::MouseEvent& e)
{
    // Basic implementation for now
    if (e.x < keyboardWidth || e.y < headerHeight) {
        // Clicked on keyboard or header
        return;
    }
    
    selectedNote = -1;
    dragging = false;
    resizing = false;
    
    // Notify about changes
    sendChangeMessage();
}

void PatternEditor::mouseDrag(const juce::MouseEvent& e)
{
    // Basic implementation - nothing for now
}

void PatternEditor::mouseUp(const juce::MouseEvent& e)
{
    // Basic implementation - nothing for now
    dragging = false;
    resizing = false;
}

float PatternEditor::beatToX(double beat) const
{
    return static_cast<float>(keyboardWidth + (beat - scrollX) * zoomX);
}

double PatternEditor::xToBeat(float x) const
{
    return scrollX + (x - keyboardWidth) / zoomX;
}

float PatternEditor::noteToY(int note) const
{
    return static_cast<float>(headerHeight + (maxNote - note) * noteHeight);
}

int PatternEditor::yToNote(float y) const
{
    return maxNote - static_cast<int>((y - headerHeight) / noteHeight);
}

void PatternEditor::drawGrid(juce::Graphics& g)
{
    // Draw a simple grid
    g.setColour(juce::Colours::darkgrey.withAlpha(0.3f));
    
    // Vertical lines (beats)
    double startBeat = std::floor(scrollX);
    double endBeat = startBeat + getWidth() / zoomX;
    
    for (double beat = startBeat; beat <= endBeat; beat += gridSize) {
        float x = beatToX(beat);
        
        // Highlight whole beats
        if (std::fmod(beat, 1.0) < 0.001) {
            g.setColour(juce::Colours::darkgrey.withAlpha(0.5f));
            g.drawLine(x, static_cast<float>(headerHeight), x, static_cast<float>(getHeight()), 1.0f);
            g.setColour(juce::Colours::darkgrey.withAlpha(0.3f));
        } else {
            g.drawLine(x, static_cast<float>(headerHeight), x, static_cast<float>(getHeight()), 0.5f);
        }
    }
    
    // Horizontal lines (notes)
    g.setColour(juce::Colours::darkgrey.withAlpha(0.3f));
    
    for (int note = minNote; note <= maxNote; ++note) {
        float y = noteToY(note);
        
        // Highlight C notes
        if (note % 12 == 0) {
            g.setColour(juce::Colours::darkgrey.withAlpha(0.5f));
            g.drawLine(static_cast<float>(keyboardWidth), y, static_cast<float>(getWidth()), y, 1.0f);
            g.setColour(juce::Colours::darkgrey.withAlpha(0.3f));
        } else {
            g.drawLine(static_cast<float>(keyboardWidth), y, static_cast<float>(getWidth()), y, 0.5f);
        }
    }
}

void PatternEditor::drawKeyboard(juce::Graphics& g)
{
    // Draw a simple piano keyboard
    g.setColour(juce::Colours::darkgrey);
    g.fillRect(0, headerHeight, keyboardWidth, getHeight() - headerHeight);
    
    for (int note = minNote; note <= maxNote; ++note) {
        float y = noteToY(note);
        
        // White keys
        if (note % 12 == 0 || note % 12 == 2 || note % 12 == 4 || 
            note % 12 == 5 || note % 12 == 7 || note % 12 == 9 || note % 12 == 11) {
            g.setColour(juce::Colours::white);
        } 
        // Black keys
        else {
            g.setColour(juce::Colours::black);
        }
        
        g.fillRect(0, y, keyboardWidth - 1, noteHeight);
        
        // Draw key borders
        g.setColour(juce::Colours::grey);
        g.drawRect(0, y, keyboardWidth - 1, noteHeight);
        
        // Draw note names for C notes
        if (note % 12 == 0) {
            g.setColour(juce::Colours::black);
            g.setFont(10.0f);
            g.drawText("C" + juce::String(note / 12 - 1), 5, static_cast<int>(y), 
                      keyboardWidth - 10, noteHeight, juce::Justification::centredLeft, true);
        }
    }
}

void PatternEditor::drawHeader(juce::Graphics& g)
{
    // Draw the timeline header
    g.setColour(juce::Colours::darkgrey);
    g.fillRect(0, 0, getWidth(), headerHeight);
    
    g.setColour(juce::Colours::white);
    g.setFont(12.0f);
    
    // Draw beat numbers
    double startBeat = std::floor(scrollX);
    double endBeat = startBeat + getWidth() / zoomX;
    
    for (double beat = startBeat; beat <= endBeat; ++beat) {
        if (std::fmod(beat, 1.0) < 0.001) {
            float x = beatToX(beat);
            g.drawText(juce::String(static_cast<int>(beat) + 1), 
                      static_cast<int>(x - 10), 5, 20, headerHeight - 10, 
                      juce::Justification::centred, true);
        }
    }
}

void PatternEditor::drawNotes(juce::Graphics& g)
{
    // In a real implementation, this would draw the notes from the pattern
    // For now, just draw a placeholder note
    g.setColour(juce::Colours::orange);
    auto rect = getNoteRect(60, 1.0, 1.0); // Middle C, beat 1, duration 1
    g.fillRect(rect);
    g.setColour(juce::Colours::white);
    g.drawRect(rect, 1.0f);
}

juce::Rectangle<float> PatternEditor::getNoteRect(int note, double startBeat, double duration) const
{
    float x = beatToX(startBeat);
    float y = noteToY(note);
    float width = static_cast<float>(duration * zoomX);
    
    return juce::Rectangle<float>(x, y, width, static_cast<float>(noteHeight));
}

int PatternEditor::findNoteAt(float x, float y) const
{
    // In a real implementation, this would find notes at the given position
    // For now, just return -1 (no note)
    return -1;
}

bool PatternEditor::isNearNoteEnd(float x, float y, int noteIndex) const
{
    // In a real implementation, this would check if the position is near the end of a note
    // For now, just return false
    return false;
}

void PatternEditor::addNoteAt(float x, float y)
{
    // In a real implementation, this would add a note at the given position
    // For now, do nothing
}

void PatternEditor::removeNote(int index)
{
    // In a real implementation, this would remove a note at the given index
    // For now, do nothing
}

void PatternEditor::moveNote(int index, double newStartBeat, int newNote)
{
    // In a real implementation, this would move a note to a new position
    // For now, do nothing
}

void PatternEditor::resizeNote(int index, double newDuration)
{
    // In a real implementation, this would resize a note
    // For now, do nothing
}

} // namespace UndergroundBeats
