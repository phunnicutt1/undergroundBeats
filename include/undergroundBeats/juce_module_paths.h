#pragma once

// This header sets up JUCE module paths
// We manually add the full path to JUCE modules here

// We'll use relative paths from this file's location to JUCE modules
#define JUCE_MODULE_PATH "../../external/JUCE/modules"

// Full paths to each module
#define JUCE_CORE_PATH JUCE_MODULE_PATH "/juce_core"
#define JUCE_EVENTS_PATH JUCE_MODULE_PATH "/juce_events"
#define JUCE_DATA_STRUCTURES_PATH JUCE_MODULE_PATH "/juce_data_structures"
#define JUCE_AUDIO_BASICS_PATH JUCE_MODULE_PATH "/juce_audio_basics"
#define JUCE_AUDIO_DEVICES_PATH JUCE_MODULE_PATH "/juce_audio_devices"
#define JUCE_AUDIO_FORMATS_PATH JUCE_MODULE_PATH "/juce_audio_formats"
#define JUCE_AUDIO_PROCESSORS_PATH JUCE_MODULE_PATH "/juce_audio_processors"
#define JUCE_AUDIO_UTILS_PATH JUCE_MODULE_PATH "/juce_audio_utils"
#define JUCE_DSP_PATH JUCE_MODULE_PATH "/juce_dsp"
#define JUCE_GRAPHICS_PATH JUCE_MODULE_PATH "/juce_graphics"
#define JUCE_GUI_BASICS_PATH JUCE_MODULE_PATH "/juce_gui_basics"
#define JUCE_GUI_EXTRA_PATH JUCE_MODULE_PATH "/juce_gui_extra" 