/*
 * Underground Beats
 * PatternEditor.h
 * 
 * Grid-based pattern editor for note entry
 */

#pragma once

#include <JuceHeader.h>
#include "../../sequencer/Pattern.h"
#include <vector>
#include <functional>

namespace UndergroundBeats {

/**
 * @class PatternEditor
 * @brief Grid-based pattern editor for note entry
 * 
 * The PatternEditor class provides a piano-roll style grid interface
 * for entering and editing notes in a pattern.
 */
class PatternEditor : public juce::Component,
                     public juce::ChangeBroadcaster {
public:
    PatternEditor();
    ~PatternEditor() override;
    
    /**
     * @brief Set the pattern to edit
     * 
     * @param pattern The pattern to edit
     */
    void setPattern(Pattern* pattern);
    
    /**
     * @brief Update the display
     */
    void updateDisplay();
    
    /**
     * @brief Set the grid size
     * 
     * @param gridSize Grid size in beats (e.g. 0.25 for 16th notes)
     */
    void setGridSize(double gridSize);
    
    /**
     * @brief Get the grid size
     * 
     * @return Grid size in beats
     */
    double getGridSize() const;
    
    /**
     * @brief Set the zoom level
     * 
     * @param zoomX Horizontal zoom level
     * @param zoomY Vertical zoom level
     */
    void setZoom(float zoomX, float zoomY);
    
    /**
     * @brief Get the horizontal zoom level
     * 
     * @return Horizontal zoom level
     */
    float getZoomX() const;
    
    /**
     * @brief Get the vertical zoom level
     * 
     * @return Vertical zoom level
     */
    float getZoomY() const;
    
    /**
     * @brief Set the scroll position
     * 
     * @param x Horizontal scroll position in beats
     * @param y Vertical scroll position in notes
     */
    void setScrollPosition(double x, int y);
    
    /**
     * @brief Get the horizontal scroll position
     * 
     * @return Horizontal scroll position in beats
     */
    double getScrollPositionX() const;
    
    /**
     * @brief Get the vertical scroll position
     * 
     * @return Vertical scroll position in notes
     */
    int getScrollPositionY() const;
    
    /**
     * @brief Set a callback for when a note is selected
     * 
     * @param callback The callback function
     */
    void setNoteSelectedCallback(std::function<void(int, double, double)> callback);
    
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
     * @brief Mouse down callback
     * 
     * @param e Mouse event
     */
    void mouseDown(const juce::MouseEvent& e) override;
    
    /**
     * @brief Mouse drag callback
     * 
     * @param e Mouse event
     */
    void mouseDrag(const juce::MouseEvent& e) override;
    
    /**
     * @brief Mouse up callback
     * 
     * @param e Mouse event
     */
    void mouseUp(const juce::MouseEvent& e) override;
    
private:
    Pattern* pattern;
    
    // Grid settings
    double gridSize;
    float zoomX;
    float zoomY;
    double scrollX;
    int scrollY;
    
    // Note display settings
    static constexpr int keyboardWidth = 60;
    static constexpr int headerHeight = 30;
    static constexpr int noteHeight = 16;
    static constexpr int octaveHeight = 12 * noteHeight;
    static constexpr int minNote = 24; // C1
    static constexpr int maxNote = 108; // C8
    
    // Selection state
    bool dragging;
    bool resizing;
    int selectedNote;
    double selectedNoteStart;
    double selectedNoteDuration;
    
    // Callbacks
    std::function<void(int, double, double)> noteSelectedCallback;
    
    // Convert beat to x-coordinate
    float beatToX(double beat) const;
    
    // Convert x-coordinate to beat
    double xToBeat(float x) const;
    
    // Convert note number to y-coordinate
    float noteToY(int note) const;
    
    // Convert y-coordinate to note number
    int yToNote(float y) const;
    
    // Draw the grid
    void drawGrid(juce::Graphics& g);
    
    // Draw the piano keyboard
    void drawKeyboard(juce::Graphics& g);
    
    // Draw the timeline header
    void drawHeader(juce::Graphics& g);
    
    // Draw the notes
    void drawNotes(juce::Graphics& g);
    
    // Get the note rectangle
    juce::Rectangle<float> getNoteRect(int note, double startBeat, double duration) const;
    
    // Find a note at the given position
    int findNoteAt(float x, float y) const;
    
    // Check if a position is near the end of a note for resizing
    bool isNearNoteEnd(float x, float y, int noteIndex) const;
    
    // Add a note at the given position
    void addNoteAt(float x, float y);
    
    // Remove a note
    void removeNote(int index);
    
    // Move a note
    void moveNote(int index, double newStartBeat, int newNote);
    
    // Resize a note
    void resizeNote(int index, double newDuration);
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PatternEditor)
};

} // namespace UndergroundBeats
