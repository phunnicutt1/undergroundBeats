#!/bin/bash

# Create build directory if it doesn't exist
mkdir -p build

# Change to build directory
cd build

# Run CMake
cmake ..

# Build the project
cmake --build .
