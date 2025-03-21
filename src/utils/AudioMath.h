/*
 * Underground Beats
 * AudioMath.h
 * 
 * Utility functions for audio processing mathematics
 */

#pragma once

#include <JuceHeader.h>
#include <cmath>

namespace UndergroundBeats {
namespace AudioMath {

/**
 * @brief Convert a MIDI note number to frequency in Hz
 * 
 * @param midiNote MIDI note number (0-127)
 * @return Frequency in Hz
 */
inline float midiNoteToFrequency(float midiNote)
{
    return 440.0f * std::pow(2.0f, (midiNote - 69.0f) / 12.0f);
}

/**
 * @brief Convert a frequency in Hz to a MIDI note number
 * 
 * @param frequency Frequency in Hz
 * @return MIDI note number (can be fractional for detuned notes)
 */
inline float frequencyToMidiNote(float frequency)
{
    return 69.0f + 12.0f * std::log2(frequency / 440.0f);
}

/**
 * @brief Convert a value in decibels to a linear gain value
 * 
 * @param decibels Value in decibels
 * @return Linear gain value
 */
inline float dbToGain(float decibels)
{
    return std::pow(10.0f, decibels / 20.0f);
}

/**
 * @brief Convert a linear gain value to decibels
 * 
 * @param gain Linear gain value
 * @return Value in decibels
 */
inline float gainToDb(float gain)
{
    return 20.0f * std::log10(gain);
}

/**
 * @brief Map a value from one range to another
 * 
 * @param value The value to map
 * @param sourceMin Minimum of the source range
 * @param sourceMax Maximum of the source range
 * @param destMin Minimum of the destination range
 * @param destMax Maximum of the destination range
 * @return Mapped value in the destination range
 */
inline float mapRange(float value, float sourceMin, float sourceMax, float destMin, float destMax)
{
    return destMin + (value - sourceMin) * (destMax - destMin) / (sourceMax - sourceMin);
}

/**
 * @brief Convert a tempo in BPM and a note duration to milliseconds
 * 
 * @param bpm Tempo in beats per minute
 * @param noteDuration Note duration (1.0 = quarter note, 0.5 = eighth note, etc.)
 * @return Duration in milliseconds
 */
inline float noteToMs(float bpm, float noteDuration)
{
    // Duration of a quarter note in ms
    float quarterNoteMs = 60000.0f / bpm;
    
    // Scale by note duration
    return quarterNoteMs * noteDuration;
}

/**
 * @brief Create a logarithmic knob response for UI controls
 * 
 * @param value The normalized input value (0-1)
 * @param min The minimum output value
 * @param max The maximum output value
 * @param midpoint The center value (where normalized value = 0.5)
 * @return The mapped value
 */
inline float logKnobResponse(float value, float min, float max, float midpoint)
{
    // Check for valid inputs
    if (min >= max || midpoint <= min || midpoint >= max || value < 0.0f || value > 1.0f)
    {
        return value * (max - min) + min;
    }
    
    // Calculate skew factor
    float skewFactor = std::log(0.5f) / std::log((midpoint - min) / (max - min));
    
    // Apply skew
    return min + (max - min) * std::pow(value, skewFactor);
}

/**
 * @brief Get the next power of 2 greater than or equal to the input
 * 
 * @param n The input value
 * @return The next power of 2
 */
inline int nextPowerOf2(int n)
{
    n--;
    n |= n >> 1;
    n |= n >> 2;
    n |= n >> 4;
    n |= n >> 8;
    n |= n >> 16;
    return n + 1;
}

} // namespace AudioMath
} // namespace UndergroundBeats
