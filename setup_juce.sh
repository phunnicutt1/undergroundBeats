#!/bin/bash

# Script to set up JUCE framework for undergroundBeats

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
JUCE_DIR="$SCRIPT_DIR/external/JUCE"

# Check if JUCE directory already exists
if [ -d "$JUCE_DIR" ]; then
    echo "JUCE directory already exists. Updating..."
    cd "$JUCE_DIR"
    git pull
    echo "JUCE updated successfully."
else
    echo "Cloning JUCE..."
    mkdir -p "$SCRIPT_DIR/external"
    cd "$SCRIPT_DIR/external"
    git clone https://github.com/juce-framework/JUCE.git
    
    if [ $? -ne 0 ]; then
        echo "Failed to clone JUCE. Please check your internet connection and git installation."
        exit 1
    fi
    
    echo "JUCE cloned successfully."
fi

echo "Setup complete. You can now build the project using CMake."
