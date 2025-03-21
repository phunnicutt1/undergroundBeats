/*
 * Underground Beats
 * Sequencer.cpp
 * 
 * Implementation of the Sequencer class
 */

#include "Sequencer.h"
#include "../utils/AudioMath.h"
#include <algorithm>
#include <cmath>

namespace UndergroundBeats {

Sequencer::Sequencer()
    : currentPosition(0.0),
      tempo(120.0),
      timeSignatureNumerator(4),
      timeSignatureDenominator(4),
      playing(false),
      looping(false),
      loopStart(0.0),
      loopEnd(4.0),
      quantizationGrid(0.25), // 16th notes
      lastEventPosition(0.0),
      currentSampleRate(44100.0),
      currentBlockSize(512)
{
    // Initialize the temp MIDI buffer
    tempMidiBuffer.ensureSize(256);
}

Sequencer::~Sequencer()
{
    // Stop playback and timer
    stop();
}

void Sequencer::setTimeline(std::shared_ptr<Timeline> newTimeline)
{
    // Update the timeline and reset position if needed
    timeline = newTimeline;
    
    // If we're looping, check that loop points are valid
    if (looping && timeline)
    {
        double timelineLength = timeline->getLength();
        loopEnd = std::min(loopEnd, timelineLength);
    }
}

std::shared_ptr<Timeline> Sequencer::getTimeline() const
{
    return timeline;
}

void Sequencer::play()
{
    if (!playing)
    {
        // Reset some state
        lastEventPosition = currentPosition;
        activeNotes.clear();
        
        // Start the timer for playback
        startTimer(timerIntervalMs);
        
        // Mark as playing
        playing = true;
    }
}

void Sequencer::stop()
{
    if (playing)
    {
        // Stop the timer
        stopTimer();
        
        // Send note-offs for any active notes
        juce::MidiBuffer buffer;
        for (const auto& note : activeNotes)
        {
            buffer.addEvent(juce::MidiMessage::noteOff(1, note.note, 0.0f), 0);
            
            // If we have a callback, notify it as well
            if (noteEventCallback)
            {
                NoteEvent event;
                event.note = note.note;
                event.velocity = 0;
                event.startTime = currentPosition;
                event.duration = 0.0;
                noteEventCallback(event);
            }
        }
        
        // Process these note-offs
        if (parameterCallback)
        {
            processMidi(juce::MidiBuffer(), buffer);
        }
        
        // Clear active notes
        activeNotes.clear();
        
        // Mark as stopped
        playing = false;
    }
}

void Sequencer::togglePlayStop()
{
    if (playing)
        stop();
    else
        play();
}

void Sequencer::setPosition(double positionInBeats)
{
    bool wasPlaying = playing;
    
    // Stop playback to clear any active notes
    if (wasPlaying)
        stop();
    
    // Update position
    currentPosition = positionInBeats;
    lastEventPosition = currentPosition;
    
    // Resume playback if needed
    if (wasPlaying)
        play();
}

double Sequencer::getPosition() const
{
    return currentPosition;
}

bool Sequencer::isPlaying() const
{
    return playing;
}

void Sequencer::setTempo(double bpm)
{
    tempo = std::max(1.0, std::min(999.0, bpm));
}

double Sequencer::getTempo() const
{
    return tempo;
}

void Sequencer::setTimeSignature(int numerator, int denominator)
{
    timeSignatureNumerator = std::max(1, numerator);
    timeSignatureDenominator = std::max(1, denominator);
}

int Sequencer::getTimeSignatureNumerator() const
{
    return timeSignatureNumerator;
}

int Sequencer::getTimeSignatureDenominator() const
{
    return timeSignatureDenominator;
}

void Sequencer::setLooping(bool shouldLoop)
{
    looping = shouldLoop;
}

bool Sequencer::isLooping() const
{
    return looping;
}

void Sequencer::setLoopStart(double startInBeats)
{
    loopStart = std::max(0.0, startInBeats);
    
    // Ensure loop start is before loop end
    if (loopStart >= loopEnd)
    {
        loopEnd = loopStart + 1.0;
    }
}

double Sequencer::getLoopStart() const
{
    return loopStart;
}

void Sequencer::setLoopEnd(double endInBeats)
{
    // Ensure loop end is positive and after loop start
    loopEnd = std::max(loopStart + 0.1, endInBeats);
}

double Sequencer::getLoopEnd() const
{
    return loopEnd;
}

void Sequencer::setQuantizationGrid(double gridSize)
{
    quantizationGrid = std::max(0.0, gridSize);
}

double Sequencer::getQuantizationGrid() const
{
    return quantizationGrid;
}

void Sequencer::processMidi(const juce::MidiBuffer& midiInput, juce::MidiBuffer& midiOutput)
{
    // If we have a timeline and we're playing, generate events
    if (timeline && playing)
    {
        // Calculate how much time this buffer represents
        double blockDuration = static_cast<double>(currentBlockSize) / currentSampleRate;
        double blockDurationBeats = secondsToBeats(blockDuration);
        
        // Generate events from the current position to the next position
        double nextPosition = currentPosition + blockDurationBeats;
        generateEvents(lastEventPosition, nextPosition, midiOutput);
        
        // Check for loop points
        if (looping && nextPosition >= loopEnd)
        {
            // We've reached the loop end, jump back to loop start
            double overshoot = nextPosition - loopEnd;
            currentPosition = loopStart + overshoot;
            lastEventPosition = loopStart;
            
            // Send note-offs for any active notes
            for (const auto& note : activeNotes)
            {
                midiOutput.addEvent(juce::MidiMessage::noteOff(1, note.note, 0.0f), 0);
                
                // If we have a callback, notify it as well
                if (noteEventCallback)
                {
                    NoteEvent event;
                    event.note = note.note;
                    event.velocity = 0;
                    event.startTime = loopEnd;
                    event.duration = 0.0;
                    noteEventCallback(event);
                }
            }
            
            // Clear active notes
            activeNotes.clear();
            
            // Generate events for the new position
            generateEvents(loopStart, currentPosition, midiOutput);
        }
        else
        {
            currentPosition = nextPosition;
        }
        
        lastEventPosition = currentPosition;
    }
    
    // Pass through any incoming MIDI messages
    if (!midiInput.isEmpty())
    {
        midiOutput.addEvents(midiInput, 0, -1, 0);
    }
}

void Sequencer::setNoteEventCallback(std::function<void(const NoteEvent&)> callback)
{
    noteEventCallback = callback;
}

void Sequencer::setParameterCallback(std::function<void(const std::string&, float)> callback)
{
    parameterCallback = callback;
}

double Sequencer::quantizeTime(double time) const
{
    if (quantizationGrid <= 0.0)
        return time;
    
    return std::round(time / quantizationGrid) * quantizationGrid;
}

void Sequencer::handleNoteOn(juce::MidiKeyboardState* keyboardState, int midiChannel, int midiNoteNumber, float velocity)
{
    // Create a MIDI message for the note-on
    juce::MidiMessage message = juce::MidiMessage::noteOn(midiChannel, midiNoteNumber, velocity);
    
    // Add it to the MIDI buffer
    tempMidiBuffer.clear();
    tempMidiBuffer.addEvent(message, 0);
    
    // If we have a callback, notify it as well
    if (noteEventCallback)
    {
        NoteEvent event;
        event.note = midiNoteNumber;
        event.velocity = static_cast<int>(velocity * 127.0f);
        event.startTime = currentPosition;
        event.duration = 0.0; // We don't know the duration yet
        noteEventCallback(event);
    }
}

void Sequencer::handleNoteOff(juce::MidiKeyboardState* keyboardState, int midiChannel, int midiNoteNumber, float velocity)
{
    // Create a MIDI message for the note-off
    juce::MidiMessage message = juce::MidiMessage::noteOff(midiChannel, midiNoteNumber);
    
    // Add it to the MIDI buffer
    tempMidiBuffer.clear();
    tempMidiBuffer.addEvent(message, 0);
    
    // If we have a callback, notify it as well
    if (noteEventCallback)
    {
        NoteEvent event;
        event.note = midiNoteNumber;
        event.velocity = 0;
        event.startTime = currentPosition;
        event.duration = 0.0;
        noteEventCallback(event);
    }
}

void Sequencer::timerCallback()
{
    // This just advances the clock when we're playing without audio callbacks
    if (playing)
    {
        // Calculate how much time has passed
        double elapsedSeconds = timerIntervalMs / 1000.0;
        double elapsedBeats = secondsToBeats(elapsedSeconds);
        
        // Update position
        double nextPosition = currentPosition + elapsedBeats;
        
        // Check for loop points
        if (looping && nextPosition >= loopEnd)
        {
            // We've reached the loop end, jump back to loop start
            double overshoot = nextPosition - loopEnd;
            nextPosition = loopStart + overshoot;
        }
        
        currentPosition = nextPosition;
        
        // Generate parameter automation events
        generateParameterEvents(currentPosition);
    }
}

void Sequencer::prepare(double sampleRate, int blockSize)
{
    currentSampleRate = sampleRate;
    currentBlockSize = blockSize;
}

std::unique_ptr<juce::XmlElement> Sequencer::createStateXml() const
{
    auto xml = std::make_unique<juce::XmlElement>("Sequencer");
    
    // Add basic properties
    xml->setAttribute("tempo", tempo);
    xml->setAttribute("timeSignatureNumerator", timeSignatureNumerator);
    xml->setAttribute("timeSignatureDenominator", timeSignatureDenominator);
    xml->setAttribute("position", currentPosition);
    xml->setAttribute("looping", looping);
    xml->setAttribute("loopStart", loopStart);
    xml->setAttribute("loopEnd", loopEnd);
    xml->setAttribute("quantizationGrid", quantizationGrid);
    
    return xml;
}

bool Sequencer::restoreStateFromXml(const juce::XmlElement* xml)
{
    if (xml == nullptr || xml->getTagName() != "Sequencer")
        return false;
    
    // Read basic properties
    tempo = xml->getDoubleAttribute("tempo", 120.0);
    timeSignatureNumerator = xml->getIntAttribute("timeSignatureNumerator", 4);
    timeSignatureDenominator = xml->getIntAttribute("timeSignatureDenominator", 4);
    currentPosition = xml->getDoubleAttribute("position", 0.0);
    looping = xml->getBoolAttribute("looping", false);
    loopStart = xml->getDoubleAttribute("loopStart", 0.0);
    loopEnd = xml->getDoubleAttribute("loopEnd", 4.0);
    quantizationGrid = xml->getDoubleAttribute("quantizationGrid", 0.25);
    
    // Reset last event position
    lastEventPosition = currentPosition;
    
    return true;
}

double Sequencer::beatsToSeconds(double beats) const
{
    // Convert beats to seconds based on tempo
    // (60.0 / tempo) = seconds per beat
    return beats * (60.0 / tempo);
}

double Sequencer::secondsToBeats(double seconds) const
{
    // Convert seconds to beats based on tempo
    // (tempo / 60.0) = beats per second
    return seconds * (tempo / 60.0);
}

int Sequencer::secondsToSamples(double seconds) const
{
    return static_cast<int>(seconds * currentSampleRate);
}

void Sequencer::generateEvents(double startPosition, double endPosition, juce::MidiBuffer& midiBuffer)
{
    if (!timeline)
        return;
    
    // Get all notes in the time range
    std::vector<NoteEvent> notes = timeline->getNotesInRange(startPosition, endPosition);
    
    // Generate MIDI messages for each note
    for (const auto& note : notes)
    {
        // Calculate sample offsets
        double noteStartSecs = beatsToSeconds(note.startTime - startPosition);
        int noteStartSample = std::max(0, secondsToSamples(noteStartSecs));
        
        // Add note-on message
        midiBuffer.addEvent(juce::MidiMessage::noteOn(1, note.note, static_cast<float>(note.velocity) / 127.0f), noteStartSample);
        
        // Add to active notes
        ActiveNote activeNote;
        activeNote.note = note.note;
        activeNote.velocity = note.velocity;
        activeNote.startTime = note.startTime;
        activeNote.endTime = note.startTime + note.duration;
        activeNotes.push_back(activeNote);
        
        // If we have a callback, notify it as well
        if (noteEventCallback)
        {
            noteEventCallback(note);
        }
    }
    
    // Check for note-offs that should happen in this time range
    checkNoteOffs(endPosition, midiBuffer);
    
    // Generate parameter automation events
    generateParameterEvents(endPosition);
}

void Sequencer::checkNoteOffs(double currentTime, juce::MidiBuffer& midiBuffer)
{
    // Find and process note-offs
    std::vector<ActiveNote> remainingNotes;
    
    for (const auto& note : activeNotes)
    {
        if (note.endTime <= currentTime)
        {
            // This note should end now
            double noteOffSecs = beatsToSeconds(note.endTime - lastEventPosition);
            int noteOffSample = std::max(0, secondsToSamples(noteOffSecs));
            
            // Add note-off message
            midiBuffer.addEvent(juce::MidiMessage::noteOff(1, note.note), noteOffSample);
            
            // If we have a callback, notify it as well
            if (noteEventCallback)
            {
                NoteEvent event;
                event.note = note.note;
                event.velocity = 0;
                event.startTime = note.endTime;
                event.duration = 0.0;
                noteEventCallback(event);
            }
        }
        else
        {
            // This note is still active
            remainingNotes.push_back(note);
        }
    }
    
    // Update the active notes list
    activeNotes = std::move(remainingNotes);
}

void Sequencer::generateParameterEvents(double currentTime)
{
    if (!timeline || !parameterCallback)
        return;
    
    // Get all automated parameters
    if (auto patterns = timeline->getPatternInstances(); !patterns.empty())
    {
        for (const auto& instance : patterns)
        {
            if (instance.muted)
                continue;
            
            if (auto pattern = timeline->getPattern(instance.patternId))
            {
                // Get all automated parameters
                auto params = pattern->getAutomatedParameters();
                
                for (const auto& paramId : params)
                {
                    // Get the value at the current time
                    float value = pattern->getParameterValueAtTime(paramId, currentTime - instance.startTime);
                    
                    // Notify the callback
                    parameterCallback(paramId, value);
                }
            }
        }
    }
}

} // namespace UndergroundBeats
