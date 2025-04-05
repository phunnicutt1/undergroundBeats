# Test directory CMakeLists.txt

# Download Catch2 for testing
include(FetchContent)
FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG v2.13.7
)
FetchContent_MakeAvailable(Catch2)

# Define source files for tests
set(TEST_SOURCES
    # Original tests
    audio/AudioEngineTest.cpp
    
    # New tests
    audio/AudioSourceSeparatorTest.cpp
    audio/AudioComponentProcessorTest.cpp
    ml/VariationGeneratorTest.cpp
    ml/ONNXSourceSeparatorTest.cpp # Added this test
    core/UndergroundBeatsControllerTest.cpp
    core/UndergroundBeatsProcessorTest.cpp # Added this test
)

# Create test executable
add_executable(undergroundBeats_tests
    ${TEST_SOURCES}
)

# Link against JUCE modules and our main library
target_link_libraries(undergroundBeats_tests
    PRIVATE
        undergroundBeatsLib
        Catch2::Catch2
)

# Add a main function for Catch
target_sources(undergroundBeats_tests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/test_main.cpp
)

# Add tests to CTest
add_test(NAME undergroundBeats_tests COMMAND undergroundBeats_tests)