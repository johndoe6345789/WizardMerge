#!/bin/bash
# Build script for WizardMerge C++ backend using Conan and Ninja

set -e

echo "=== WizardMerge C++ Backend Build ==="
echo

# Check for required tools
command -v conan >/dev/null 2>&1 || { echo "Error: conan not found. Install with: pip install conan"; exit 1; }
command -v ninja >/dev/null 2>&1 || { echo "Error: ninja not found. Install with: apt-get install ninja-build / brew install ninja"; exit 1; }
command -v cmake >/dev/null 2>&1 || { echo "Error: cmake not found."; exit 1; }

# Create build directory
mkdir -p build
cd build

# Install dependencies with Conan
echo "Installing dependencies with Conan..."
conan install .. --output-folder=. --build=missing

# Configure with CMake
echo "Configuring with CMake..."
cmake .. -G Ninja -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Release

# Build with Ninja
echo "Building with Ninja..."
ninja

echo
echo "=== Build Complete ==="
echo "Binary: build/wizardmerge-cli"
