#!/bin/bash
# Build script for WizardMerge C++ backend with Drogon support

set -e

echo "=== WizardMerge C++ Backend Build ==="
echo

# Check for required tools
command -v cmake >/dev/null 2>&1 || { echo "Error: cmake not found."; exit 1; }
command -v ninja >/dev/null 2>&1 || { echo "Error: ninja not found. Install with: apt-get install ninja-build / brew install ninja"; exit 1; }

# Check if Drogon is installed
if ! pkg-config --exists drogon 2>/dev/null && ! ldconfig -p 2>/dev/null | grep -q libdrogon; then
    echo "WARNING: Drogon framework not found."
    echo "The library will be built, but the HTTP server will be skipped."
    echo
    echo "To build the HTTP server, install Drogon first:"
    echo "  Option 1: Run ./install_drogon.sh"
    echo "  Option 2: Use Docker: docker-compose up --build"
    echo "  Option 3: Use Conan: conan install . --output-folder=build --build=missing"
    echo
    read -p "Continue building without Drogon? (y/n) " -n 1 -r
    echo
    if [[ ! $REPLY =~ ^[Yy]$ ]]; then
        exit 1
    fi
fi

# Create build directory
mkdir -p build
cd build

# Check if we should use Conan
if command -v conan >/dev/null 2>&1 && [ -f ../conanfile.py ]; then
    echo "Installing dependencies with Conan..."
    conan install .. --output-folder=. --build=missing 2>/dev/null && CONAN_SUCCESS=true || CONAN_SUCCESS=false
    
    if [ "$CONAN_SUCCESS" = true ]; then
        echo "Configuring with CMake (Conan toolchain)..."
        cmake .. -G Ninja -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Release
    else
        echo "Conan installation failed, trying without Conan..."
        echo "Configuring with CMake..."
        cmake .. -G Ninja -DCMAKE_BUILD_TYPE=Release
    fi
else
    # Configure with CMake (without Conan)
    echo "Configuring with CMake..."
    cmake .. -G Ninja -DCMAKE_BUILD_TYPE=Release
fi

# Build with Ninja
echo "Building with Ninja..."
ninja

echo
echo "=== Build Complete ==="
if [ -f wizardmerge-cli ]; then
    echo "HTTP Server: build/wizardmerge-cli"
    echo "Run with: cd build && ./wizardmerge-cli"
else
    echo "Library: build/libwizardmerge.a"
    echo "HTTP server not built (Drogon not found)"
fi
if [ -f wizardmerge-tests ]; then
    echo "Tests: build/wizardmerge-tests"
    echo "Run with: cd build && ./wizardmerge-tests"
fi
