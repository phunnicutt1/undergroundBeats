# Underground Beats: Build Guide

This document provides comprehensive instructions for building and running the Underground Beats application. Follow these steps to get the application up and running on your system.

## Prerequisites

Before attempting to build the project, ensure you have the following prerequisites installed:

- **C++ Compiler** - Supporting C++17 standard
  - GCC 7+ (Linux)
  - Clang 5+ (macOS)
  - MSVC 2019+ (Windows)
- **CMake** - Version 3.15 or higher
- **Git** - For cloning the repository and submodules

## Setting Up the Build Environment

### 1. Clone the Repository

First, clone the repository with the `--recursive` flag to include the JUCE submodule:

```bash
git clone --recursive https://github.com/your-username/underground-beats.git
cd underground-beats
```

If you've already cloned the repository without submodules, initialize them with:

```bash
git submodule update --init --recursive
```

### 2. Building with the Build Script (Recommended)

For convenience, we've provided a build script that handles the build process:

```bash
# Make the script executable
chmod +x build.sh

# Run the build script
./build.sh
```

This will create a `build` directory, configure CMake, and build the project.

### 3. Building Manually

If you prefer to build manually:

```bash
# Create and enter build directory
mkdir build
cd build

# Configure CMake
cmake ..

# Build the project
cmake --build . --config Release
```

## Running the Application

### 1. Using the Run Script

We've provided a run script for launching the application:

```bash
# Make the script executable
chmod +x run.sh

# Run the application
./run.sh
```

### 2. Running Manually

Alternatively, you can run the application directly:

```bash
cd build
./UndergroundBeats
```

## Troubleshooting Common Build Issues

### Missing JUCE Submodule

If you see errors about missing JUCE files, ensure you've properly initialized the submodule:

```bash
git submodule update --init --recursive
```

### CMake Version Issues

If CMake reports a version error, make sure you have CMake 3.15 or newer:

```bash
cmake --version
```

### Compiler Compatibility

The project requires C++17 support. If you encounter compiler errors, check your compiler version:

```bash
# GCC
g++ --version

# Clang
clang++ --version

# MSVC (Windows)
cl
```

## Platform-Specific Notes

### macOS

On macOS, you may need the Xcode Command Line Tools:

```bash
xcode-select --install
```

### Windows

On Windows, you may want to generate Visual Studio project files:

```bash
mkdir build
cd build
cmake .. -G "Visual Studio 16 2019"
```

Then open the solution file in Visual Studio.

### Linux

On Linux, you'll need JUCE dependencies:

```bash
# Debian/Ubuntu
sudo apt-get install libasound2-dev libjack-jackd2-dev \
    libcurl4-openssl-dev \
    libfreetype6-dev \
    libx11-dev libxcomposite-dev libxcursor-dev libxcursor-dev libxext-dev libxinerama-dev libxrandr-dev libxrender-dev \
    libwebkit2gtk-4.0-dev \
    libglu1-mesa-dev mesa-common-dev
```

## Development Environment Setup

For the best development experience, consider:

1. **IDE Configuration**: 
   - Visual Studio Code with C/C++ extensions
   - CLion with CMake integration
   - Visual Studio with the C++ workload

2. **Debugging**:
   The project includes debug symbols when built in Debug mode:
   ```bash
   cmake .. -DCMAKE_BUILD_TYPE=Debug
   cmake --build .
   ```

## Next Steps

After building successfully, you'll see a tabbed interface with the following main sections:

1. **Oscillator/Envelope/Filter tabs**: Control synthesis parameters
2. **Effects tab**: Apply and configure audio effects
3. **Sequencer tab**: Basic pattern editing (partially implemented)
4. **Mixer tab**: Channel control and routing

Enjoy exploring the Underground Beats application!
