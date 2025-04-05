cmake_minimum_required(VERSION 3.15)
project(UndergroundBeats VERSION 1.0.0 LANGUAGES CXX C)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Specify target architecture for macOS (match ONNX Runtime)
set(CMAKE_OSX_ARCHITECTURES arm64)

# Add JUCE subdirectory
add_subdirectory(external/JUCE)

# Include ONNXRuntime
include(cmake/onnxruntime.cmake)

# Create a standalone application
juce_add_gui_app(UndergroundBeats
    VERSION ${PROJECT_VERSION}
    COMPANY_NAME "UndergroundBeats"
    PRODUCT_NAME "UndergroundBeats"
    BUNDLE_ID "com.undergroundbeats.app"
)

# Source files for the app
target_sources(UndergroundBeats PRIVATE
    src/Main.cpp
    src/UndergroundBeatsProcessor.cpp
    src/ml/ONNXModelLoader.cpp
    src/gui/WaveformDisplay.cpp
    src/gui/MainEditor.cpp
    src/gui/TopBarComponent.cpp
    src/gui/SidebarComponent.cpp
    src/gui/SampleBrowserComponent.cpp
    src/gui/StemControlPanel.cpp
    src/gui/TransportControls.cpp
    src/gui/panels/EQPanelComponent.cpp
    src/gui/panels/CompressorPanelComponent.cpp
    src/gui/panels/StyleTransferPanelComponent.cpp
    src/gui/panels/VariationExplorerComponent.cpp
)

# Include directories for the standalone app
target_include_directories(UndergroundBeats PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Link JUCE modules
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
    onnxruntime_imported
)

# App definitions
target_compile_definitions(UndergroundBeats PRIVATE
    JUCE_APPLICATION_NAME="UndergroundBeats"
    JUCE_STANDALONE_APPLICATION=1
    JUCE_GLOBAL_MODULE_SETTINGS_INCLUDED=1
)