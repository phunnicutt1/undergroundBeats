#!/bin/bash

# Build script for undergroundBeats

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
BUILD_DIR="$SCRIPT_DIR/build"

# Check if JUCE is set up
if [ ! -d "$SCRIPT_DIR/external/JUCE" ]; then
    echo "JUCE not found. Setting up JUCE first..."
    "$SCRIPT_DIR/setup_juce.sh"
fi

# Remove previous build directory if it exists
if [ -d "$BUILD_DIR" ]; then
    echo "Removing previous build directory..."
    rm -rf "$BUILD_DIR"
fi

# Create a fresh build directory
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

# Create necessary directories for JUCE with proper permissions
mkdir -p "$BUILD_DIR/undergroundBeats_artefacts/JuceLibraryCode/undergroundBeats"
chmod -R 755 "$BUILD_DIR/undergroundBeats_artefacts"

# Set environment variables to ensure proper compiler configuration
export CC=/Library/Developer/CommandLineTools/usr/bin/cc
export CXX=/Library/Developer/CommandLineTools/usr/bin/c++

# Generate CMake files with clean configuration
cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release \
      -DCMAKE_C_COMPILER=${CC} \
      -DCMAKE_CXX_COMPILER=${CXX} \
      -DCMAKE_OSX_ARCHITECTURES="arm64;x86_64" \
      ..

if [ $? -ne 0 ]; then
    echo "Failed to generate CMake files. Exiting."
    exit 1
fi

# Build the project
cmake --build .

if [ $? -ne 0 ]; then
    echo "Build failed. Please check the error messages above."
    exit 1
fi

echo "Build successful. You can run the application with:"
echo "./undergroundBeats"
