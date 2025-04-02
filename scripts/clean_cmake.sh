#!/bin/bash

# Script to completely clean CMake cache and build files
# Useful when encountering persistent CMake configuration issues

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
PROJECT_DIR="$(dirname "$SCRIPT_DIR")"
BUILD_DIR="$PROJECT_DIR/build"

echo "Cleaning CMake cache and build files..."

# Remove build directory
if [ -d "$BUILD_DIR" ]; then
    echo "Removing build directory..."
    rm -rf "$BUILD_DIR"
fi

# Also clean any CMake cache files in the project directory
find "$PROJECT_DIR" -name "CMakeCache.txt" -delete
find "$PROJECT_DIR" -name "cmake_install.cmake" -delete
find "$PROJECT_DIR" -name "CMakeFiles" -type d -exec rm -rf {} +

echo "CMake cache cleaning complete."
echo "You can now rebuild the project with ./build.sh" 