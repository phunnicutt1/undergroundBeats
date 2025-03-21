cmake_minimum_required(VERSION 3.15)
project(UndergroundBeats VERSION 0.1.0)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add JUCE as a subdirectory
# Note: You'll need to clone JUCE into a subdirectory named "JUCE"
# git clone https://github.com/juce-framework/JUCE.git
add_subdirectory(JUCE)

# Initialize JUCE
juce_add_gui_app(UndergroundBeats
    PRODUCT_NAME "Underground Beats"
    COMPANY_NAME "Underground Audio"
    VERSION "0.1.0"
)

# Include directories
target_include_directories(UndergroundBeats PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/audio-engine
    ${CMAKE_CURRENT_SOURCE_DIR}/src/synthesis
    ${CMAKE_CURRENT_SOURCE_DIR}/src/effects
    ${CMAKE_CURRENT_SOURCE_DIR}/src/sequencer
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ui
    ${CMAKE_CURRENT_SOURCE_DIR}/src/project
    ${CMAKE_CURRENT_SOURCE_DIR}/src/utils
)

# JUCE modules
target_link_libraries(UndergroundBeats PRIVATE
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

# Source files
target_sources(UndergroundBeats PRIVATE
    # Core application
    src/Main.cpp
    src/MainComponent.cpp
    src/MainComponent.h
    
    # Audio Engine
    src/audio-engine/Engine.cpp
    src/audio-engine/Engine.h
    src/audio-engine/ProcessorNode.cpp
    src/audio-engine/ProcessorNode.h
    src/audio-engine/ProcessorGraph.cpp
    src/audio-engine/ProcessorGraph.h
    src/audio-engine/AudioDeviceManager.cpp
    src/audio-engine/AudioDeviceManager.h
    
    # Synthesis
    src/synthesis/Oscillator.cpp
    src/synthesis/Oscillator.h
    src/synthesis/Envelope.cpp
    src/synthesis/Envelope.h
    src/synthesis/Filter.cpp
    src/synthesis/Filter.h
    src/synthesis/SynthModule.cpp
    src/synthesis/SynthModule.h
    
    # Effects
    src/effects/Effect.cpp
    src/effects/Effect.h
    src/effects/EffectsChain.cpp
    src/effects/EffectsChain.h
    src/effects/Delay.cpp
    src/effects/Delay.h
    src/effects/Reverb.cpp
    src/effects/Reverb.h
    
    # Sequencer
    src/sequencer/Sequencer.cpp
    src/sequencer/Sequencer.h
    src/sequencer/Pattern.cpp
    src/sequencer/Pattern.h
    src/sequencer/Timeline.cpp
    src/sequencer/Timeline.h
    src/sequencer/MidiEngine.cpp
    src/sequencer/MidiEngine.h
    
    # Project Management
    src/project/ProjectManager.cpp
    src/project/ProjectManager.h
    src/project/ProjectState.cpp
    src/project/ProjectState.h
    
    # UI Components
    src/ui/AppComponent.cpp
    src/ui/AppComponent.h
    src/ui/components/MixerChannel.cpp
    src/ui/components/MixerChannel.h
    src/ui/components/PatternEditor.cpp
    src/ui/components/PatternEditor.h
    src/ui/components/PatternControlPanel.cpp
    src/ui/components/PatternControlPanel.h
    src/ui/views/MixerView.cpp
    src/ui/views/MixerView.h
    src/ui/views/PatternEditorView.cpp
    src/ui/views/PatternEditorView.h
    
    # Utilities
    src/utils/AudioMath.h
    src/utils/Concurrency.h
)

# Add platform-specific settings
juce_generate_juce_header(UndergroundBeats)

# Enable additional compiler warnings
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(UndergroundBeats PRIVATE -Wall -Wextra)
elseif(MSVC)
    target_compile_options(UndergroundBeats PRIVATE /W4)
endif()
