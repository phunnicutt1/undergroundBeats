#!/bin/bash

# Run script for undergroundBeats

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
BUILD_DIR="$SCRIPT_DIR/build"

# Check if the build directory exists
if [ ! -d "$BUILD_DIR" ]; then
    echo "Build directory not found. Running build script first..."
    "$SCRIPT_DIR/build.sh"
    
    if [ $? -ne 0 ]; then
        echo "Build failed. Trying to clean CMake cache and rebuild..."
        
        # Try cleaning CMake cache and rebuilding
        if [ -f "$SCRIPT_DIR/scripts/clean_cmake.sh" ]; then
            "$SCRIPT_DIR/scripts/clean_cmake.sh"
            "$SCRIPT_DIR/build.sh"
            
            if [ $? -ne 0 ]; then
                echo "Rebuild failed after cleaning cache. Please check the error messages above."
                exit 1
            fi
        else
            echo "CMake cleaning script not found. Please fix the build issues manually."
            exit 1
        fi
    fi
fi

# Check if the executable exists
if [ ! -f "$BUILD_DIR/undergroundBeats" ]; then
    echo "Executable not found. Please make sure the build completed successfully."
    exit 1
fi

# Run the application
echo "Running Underground Beats..."
"$BUILD_DIR/undergroundBeats"
