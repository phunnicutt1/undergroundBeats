#!/bin/bash

# Simple make build script for undergroundBeats

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

# Check if JUCE is set up
if [ ! -d "$SCRIPT_DIR/external/JUCE" ]; then
    echo "JUCE not found. Setting up JUCE first..."
    "$SCRIPT_DIR/setup_juce.sh"
fi

# Run make clean first
make -C "$SCRIPT_DIR" clean

# Build the project
make -C "$SCRIPT_DIR"

if [ $? -ne 0 ]; then
    echo "Build failed. Please check the error messages above."
    exit 1
fi

echo "Build successful. You can run the application with:"
echo "./build/undergroundBeats"
