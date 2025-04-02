cmake_minimum_required(VERSION 3.15)

# Force update the compilers
set(CMAKE_C_COMPILER "/Library/Developer/CommandLineTools/usr/bin/cc" CACHE FILEPATH "C compiler" FORCE)
set(CMAKE_CXX_COMPILER "/Library/Developer/CommandLineTools/usr/bin/c++" CACHE FILEPATH "C++ compiler" FORCE)

# Set languages explicitly
project(undergroundBeats VERSION 0.1.0 LANGUAGES C CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Ensure these critical variables are set
set(CMAKE_C_COMPILE_OBJECT "<CMAKE_C_COMPILER> <DEFINES> <INCLUDES> <FLAGS> -o <OBJECT> -c <SOURCE>")
set(CMAKE_CXX_COMPILE_OBJECT "<CMAKE_CXX_COMPILER> <DEFINES> <INCLUDES> <FLAGS> -o <OBJECT> -c <SOURCE>")

# Set architecture for Mac
if(APPLE)
    set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64" CACHE STRING "Build architectures for Mac OS X" FORCE)
endif()

# JUCE setup
set(JUCE_MODULES_DIRECTORY "${CMAKE_SOURCE_DIR}/external/JUCE/modules" CACHE PATH "JUCE modules directory")

# Check if JUCE is already available
if(NOT EXISTS "${JUCE_MODULES_DIRECTORY}")
    message(STATUS "JUCE not found. Will attempt to fetch it via submodule.")
    execute_process(
        COMMAND git submodule update --init --recursive
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        RESULT_VARIABLE GIT_SUBMODULE_RESULT
    )
    if(NOT GIT_SUBMODULE_RESULT EQUAL "0")
        message(FATAL_ERROR "Failed to initialize JUCE submodule. Please make sure git is installed and clone with --recursive, or install JUCE manually.")
    endif()
endif()

# Add JUCE as an external dependency
add_subdirectory(external/JUCE)

# Set up JUCE-specific CMake functions and macros
include(external/JUCE/extras/Build/CMake/JUCEUtils.cmake)

# Add include directories
include_directories(
    ${CMAKE_SOURCE_DIR}/include
    ${JUCE_MODULES_DIRECTORY}
)

# Option to enable ONNX Runtime
option(USE_ONNX "Enable ONNX Runtime for machine learning features" OFF)
if(USE_ONNX)
    add_definitions(-DUSE_ONNX)
    # TODO: Find and link ONNX Runtime library
endif()

# Define source files
set(SOURCES
    # Original sources
    src/audio/AudioEngine.cpp
    src/audio/SamplePlayer.cpp
    src/audio/file/AudioFile.cpp
    
    # New sources
    src/Main.cpp
    src/UndergroundBeatsProcessor.cpp
    src/audio/AudioSourceSeparator.cpp
    src/audio/AudioComponentProcessor.cpp
    src/ml/VariationGenerator.cpp
    src/core/UndergroundBeatsController.cpp
    src/gui/MainEditor.cpp
)

# Define header files
set(HEADERS
    # Original headers
    include/undergroundBeats/audio/AudioEngine.h
    include/undergroundBeats/audio/SamplePlayer.h
    include/undergroundBeats/audio/file/AudioFile.h
    
    # New headers
    include/undergroundBeats/UndergroundBeatsProcessor.h
    include/undergroundBeats/audio/AudioSourceSeparator.h
    include/undergroundBeats/audio/AudioComponentProcessor.h
    include/undergroundBeats/ml/VariationGenerator.h
    include/undergroundBeats/core/UndergroundBeatsController.h
    include/undergroundBeats/gui/MainEditor.h
)

# Create a library target for the core code
add_library(undergroundBeatsLib STATIC
    ${SOURCES}
    ${HEADERS}
)

# Set properties for the library
target_include_directories(undergroundBeatsLib PUBLIC ${CMAKE_SOURCE_DIR}/include)

# Link JUCE modules to the library
target_link_libraries(undergroundBeatsLib
    PUBLIC
        juce::juce_audio_basics
        juce::juce_audio_devices
        juce::juce_audio_formats
        juce::juce_audio_processors
        juce::juce_audio_utils
        juce::juce_core
        juce::juce_data_structures
        juce::juce_dsp
        juce::juce_events
        juce::juce_graphics
        juce::juce_gui_basics
        juce::juce_gui_extra
)

# Create the main executable
juce_add_gui_app(undergroundBeats
    PRODUCT_NAME "Underground Beats"
    COMPANY_NAME "UndergroundProductions"
    BUNDLE_ID "com.undergroundproductions.undergroundbeats"
    VERSION ${PROJECT_VERSION}
    MICROPHONE_PERMISSION_ENABLED TRUE
    MICROPHONE_PERMISSION_TEXT "Underground Beats needs microphone access to record audio."
)

# Make the executable use the library
target_link_libraries(undergroundBeats
    PRIVATE
        undergroundBeatsLib
)

# We're using src/Main.cpp for the entry point
target_sources(undergroundBeats
    PRIVATE
        src/Main.cpp
)

# Create a plugin target
juce_add_plugin(undergroundBeatsPlugin
    PRODUCT_NAME "Underground Beats"
    COMPANY_NAME "UndergroundProductions"
    BUNDLE_ID "com.undergroundproductions.undergroundbeats"
    VERSION ${PROJECT_VERSION}
    PLUGIN_MANUFACTURER_CODE "UgPr"
    PLUGIN_CODE "UgBt"
    FORMATS AU VST3 Standalone
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT TRUE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS TRUE
    COPY_PLUGIN_AFTER_BUILD TRUE
)

# Make the plugin use the library
target_link_libraries(undergroundBeatsPlugin
    PRIVATE
        undergroundBeatsLib
)

# Add tests directory if testing is enabled
option(BUILD_TESTING "Build the testing tree." ON)
if(BUILD_TESTING)
    enable_testing()
    add_subdirectory(test)
endif()