#include <catch2/catch.hpp>
#include "undergroundBeats/audio/AudioEngine.h"

TEST_CASE("AudioEngine initialization", "[AudioEngine]")
{
    undergroundBeats::audio::AudioEngine engine;
    
    SECTION("Audio engine should initialize successfully")
    {
        bool result = engine.initialize();
        REQUIRE(result == true);
    }
    
    SECTION("Audio engine should report correct sample rate after initialization")
    {
        engine.initialize();
        double sampleRate = engine.getSampleRate();
        REQUIRE(sampleRate > 0.0);
    }
    
    SECTION("Audio engine should report correct buffer size after initialization")
    {
        engine.initialize();
        int bufferSize = engine.getBufferSize();
        REQUIRE(bufferSize > 0);
    }
}

TEST_CASE("AudioEngine test tone generation", "[AudioEngine]")
{
    undergroundBeats::audio::AudioEngine engine;
    
    SECTION("Test tone should be active after generation")
    {
        engine.initialize();
        engine.generateTestTone(440.0f, 0.5f);
        REQUIRE(engine.isRunning() == false);
        
        engine.start();
        REQUIRE(engine.isRunning() == true);
        
        engine.stop();
        REQUIRE(engine.isRunning() == false);
    }
}