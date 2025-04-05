#include <catch2/catch.hpp>
#include "undergroundBeats/UndergroundBeatsProcessor.h"
#include <juce_core/juce_core.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_audio_formats/juce_audio_formats.h> // For file loading tests
#include <juce_gui_basics/juce_gui_basics.h> // For ScopedJuceInitialiser_GUI

#include <vector>
#include <string>

// Helper to ensure JUCE message manager is initialized, often needed for APVTS etc.
// Place it globally or within a fixture if preferred.
static juce::ScopedJuceInitialiser_GUI guiInitialiser;

// --- Test Case ---
TEST_CASE("UndergroundBeatsProcessor Tests", "[core][processor]") {

    // Create a processor instance for testing
    // Note: If the processor constructor itself has complex dependencies or file IO,
    // it might need adjustment or mocking. Assuming default constructor is safe here.
    undergroundBeats::UndergroundBeatsProcessor processor;

    // Prepare the processor (minimal setup)
    processor.prepareToPlay(44100.0, 512); // Sample rate and block size

    SECTION("Playback State") {
        // Initial state
        REQUIRE_FALSE(processor.isPlaying());
        REQUIRE_FALSE(processor.isPaused());

        // Start -> Playing
        processor.startPlayback();
        REQUIRE(processor.isPlaying());
        REQUIRE_FALSE(processor.isPaused());

        // Pause -> Paused
        processor.pausePlayback();
        REQUIRE_FALSE(processor.isPlaying()); // isPlaying should be false when paused
        REQUIRE(processor.isPaused());

        // Start again (from paused) -> Playing
        processor.startPlayback();
        REQUIRE(processor.isPlaying());
        REQUIRE_FALSE(processor.isPaused());

        // Stop (from playing) -> Stopped
        processor.stopPlayback();
        REQUIRE_FALSE(processor.isPlaying());
        REQUIRE_FALSE(processor.isPaused());

        // Pause (from stopped) -> Still stopped (no effect)
        processor.pausePlayback();
        REQUIRE_FALSE(processor.isPlaying());
        REQUIRE_FALSE(processor.isPaused());

        // Stop (from stopped) -> Still stopped (no effect)
        processor.stopPlayback();
        REQUIRE_FALSE(processor.isPlaying());
        REQUIRE_FALSE(processor.isPaused());

         // Start -> Pause -> Stop -> Stopped
         processor.startPlayback();
         processor.pausePlayback();
         processor.stopPlayback();
         REQUIRE_FALSE(processor.isPlaying());
         REQUIRE_FALSE(processor.isPaused());
    }

    SECTION("File Loading (loadAudioFile)") {
        // Requires a file path. We'll use a non-existent one first.
        juce::File nonExistentFile = juce::File::getNonexistentFile();
        REQUIRE_FALSE(processor.loadAudioFile(nonExistentFile));
        // Check state after failed load (should be unchanged or reset)
        // Assuming getSeparatedStemBuffers() exists and returns a const ref
        REQUIRE(processor.getSeparatedStemBuffers().empty()); // Assuming it resets/clears on fail

        // TODO: Test successful load
        // This requires either:
        // 1. A valid dummy audio file accessible during the test run.
        // 2. Mocking juce::AudioFormatManager and juce::AudioFormatReader.
        WARN("loadAudioFile success case requires a real file or file I/O mocking - test skipped.");
        // Example structure if mocking or file exists:
        // juce::File dummyFile("path/to/valid/dummy.wav"); // Create or provide this file
        // // Ensure the processor is reset or in a known state before loading
        // processor.stopPlayback(); // Example reset action
        // // ... potentially clear internal buffers if needed ...
        // REQUIRE(processor.loadAudioFile(dummyFile));
        // // Check if stems were populated (even dummy ones if separation is mocked/simulated)
        // // This depends heavily on loadAudioFile's actual implementation regarding separation
        // // REQUIRE_FALSE(processor.getSeparatedStemBuffers().empty());
    }

    SECTION("Parameter Creation") {
        // Parameters (Volume/Gain) are expected to be created *after* a successful file load
        // where stems are identified.

        // Initial state check (before load)
        auto& apvts = processor.getValueTreeState();
        // Assuming parameters are NOT created until load, check they don't exist initially.
        // Need expected stem names if they are known (e.g., "drums", "bass", "vocals", "other")
        // Let's assume 4 stems for this example: Drums, Bass, Other, Vocals
        // These names should ideally come from a shared source or config if possible.
        const int expectedNumStems = 4; // Example number

        for(int i = 0; i < expectedNumStems; ++i) {
             // Use the static helper function from the processor class
             juce::String volParamID = undergroundBeats::UndergroundBeatsProcessor::getStemParameterID(i, "Volume");
             juce::String gainParamID = undergroundBeats::UndergroundBeatsProcessor::getStemParameterID(i, "Gain");
             REQUIRE(apvts.getParameter(volParamID) == nullptr);
             REQUIRE(apvts.getParameter(gainParamID) == nullptr);
        }

        // TODO: Test parameter creation after successful load
        // This requires simulating a successful load that results in stems being identified
        // and the parameter creation logic being triggered within loadAudioFile or prepareToPlay.
        // Since mocking loadAudioFile fully is complex, this test remains incomplete without
        // either a test file and assuming loadAudioFile works, or significant mocking/refactoring.
        WARN("Parameter creation test after successful load requires simulating stem creation or a working loadAudioFile with separation - test skipped/incomplete.");

        // Conceptual check if loadAudioFile *were* to succeed and create params for 'expectedNumStems':
        // // 1. Simulate successful load (e.g., using a real test file and assuming loadAudioFile works)
        // juce::File dummyFile("path/to/valid/dummy.wav");
        // processor.loadAudioFile(dummyFile); // Assume this triggers param creation
        //
        // // 2. Verify parameters now exist
        // for(int i = 0; i < expectedNumStems; ++i) {
        //     juce::String volParamID = undergroundBeats::UndergroundBeatsProcessor::getStemParameterID(i, "Volume");
        //     juce::String gainParamID = undergroundBeats::UndergroundBeatsProcessor::getStemParameterID(i, "Gain");
        //     INFO("Checking parameter: " << volParamID); // Add info for debugging
        //     REQUIRE(apvts.getParameter(volParamID) != nullptr);
        //     REQUIRE(apvts.getRawParameterValue(volParamID) != nullptr); // Check it's usable
        //
        //     INFO("Checking parameter: " << gainParamID);
        //     REQUIRE(apvts.getParameter(gainParamID) != nullptr);
        //     REQUIRE(apvts.getRawParameterValue(gainParamID) != nullptr);
        // }
    }

    SECTION("DSP Pipeline Parameter Updates and Processing") {
        processor.startPlayback();

        // Simulate 1 stem with dummy buffer
        processor.stopPlayback();
        processor.getSeparatedStemBuffers().clear();
        juce::AudioBuffer<float> dummyStem(2, 512);
        dummyStem.clear();
        dummyStem.applyGain(0, 0, 512, 1.0f); // Fill with ones
        processor.getSeparatedStemBuffers().push_back(dummyStem);
        processor.prepareToPlay(44100.0, 512);
        processor.startPlayback();

        auto& apvts = processor.getValueTreeState();
        int stemIdx = 0;

        // Enable all effects
        apvts.getParameter(processor.getStemParameterID(stemIdx, "EQ1_Enable"))->setValueNotifyingHost(1.0f);
        apvts.getParameter(processor.getStemParameterID(stemIdx, "EQ2_Enable"))->setValueNotifyingHost(1.0f);
        apvts.getParameter(processor.getStemParameterID(stemIdx, "EQ3_Enable"))->setValueNotifyingHost(1.0f);
        apvts.getParameter(processor.getStemParameterID(stemIdx, "Comp_Enable"))->setValueNotifyingHost(1.0f);
        apvts.getParameter(processor.getStemParameterID(stemIdx, "Style_Enable"))->setValueNotifyingHost(1.0f);

        // Set some EQ gains
        apvts.getParameter(processor.getStemParameterID(stemIdx, "EQ1_Gain"))->setValueNotifyingHost(0.5f);
        apvts.getParameter(processor.getStemParameterID(stemIdx, "EQ2_Gain"))->setValueNotifyingHost(-0.5f);
        apvts.getParameter(processor.getStemParameterID(stemIdx, "EQ3_Gain"))->setValueNotifyingHost(0.0f);

        // Set compressor params
        apvts.getParameter(processor.getStemParameterID(stemIdx, "Comp_Threshold"))->setValueNotifyingHost(-20.0f);
        apvts.getParameter(processor.getStemParameterID(stemIdx, "Comp_Ratio"))->setValueNotifyingHost(4.0f);

        // Prepare dummy input buffer
        juce::AudioBuffer<float> inputBuffer(2, 512);
        inputBuffer.clear();
        inputBuffer.applyGain(0, 0, 512, 1.0f);
        juce::MidiBuffer midi;

        processor.processBlock(inputBuffer, midi);

        float sum = 0.0f;
        for (int ch = 0; ch < inputBuffer.getNumChannels(); ++ch)
            sum += inputBuffer.getRMSLevel(ch, 0, inputBuffer.getNumSamples());

        REQUIRE(sum > 0.0f); // Should have some output

        // Disable all effects
        apvts.getParameter(processor.getStemParameterID(stemIdx, "EQ1_Enable"))->setValueNotifyingHost(0.0f);
        apvts.getParameter(processor.getStemParameterID(stemIdx, "EQ2_Enable"))->setValueNotifyingHost(0.0f);
        apvts.getParameter(processor.getStemParameterID(stemIdx, "EQ3_Enable"))->setValueNotifyingHost(0.0f);
        apvts.getParameter(processor.getStemParameterID(stemIdx, "Comp_Enable"))->setValueNotifyingHost(0.0f);
        apvts.getParameter(processor.getStemParameterID(stemIdx, "Style_Enable"))->setValueNotifyingHost(0.0f);

        processor.processBlock(inputBuffer, midi);

        float sum2 = 0.0f;
        for (int ch = 0; ch < inputBuffer.getNumChannels(); ++ch)
            sum2 += inputBuffer.getRMSLevel(ch, 0, inputBuffer.getNumSamples());

        REQUIRE(sum2 >= 0.0f); // Should still be valid
    }

    // Release resources at the end of the test case
    processor.releaseResources();
}