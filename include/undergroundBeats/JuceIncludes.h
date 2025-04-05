#pragma once

// Include the JUCE module paths
#include "juce_module_paths.h"

// This is a helper file to include JUCE modules with proper configuration
// Include this file instead of individual JUCE modules

// Core JUCE modules
#include JUCE_CORE_PATH "/juce_core.h"
#include JUCE_EVENTS_PATH "/juce_events.h"
#include JUCE_DATA_STRUCTURES_PATH "/juce_data_structures.h"

// Audio modules
#include JUCE_AUDIO_BASICS_PATH "/juce_audio_basics.h"
#include JUCE_AUDIO_DEVICES_PATH "/juce_audio_devices.h"
#include JUCE_AUDIO_FORMATS_PATH "/juce_audio_formats.h"
#include JUCE_AUDIO_PROCESSORS_PATH "/juce_audio_processors.h"
#include JUCE_AUDIO_UTILS_PATH "/juce_audio_utils.h"
#include JUCE_DSP_PATH "/juce_dsp.h"

// GUI modules
#include JUCE_GRAPHICS_PATH "/juce_graphics.h"
#include JUCE_GUI_BASICS_PATH "/juce_gui_basics.h"
#include JUCE_GUI_EXTRA_PATH "/juce_gui_extra.h" 