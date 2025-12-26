#!/bin/bash
# Script to install Drogon framework from source
# Run this script before building WizardMerge if Drogon is not installed

set -e

echo "=== Installing Drogon Framework from Source ==="
echo

# Check for required tools
command -v git >/dev/null 2>&1 || { echo "Error: git not found."; exit 1; }
command -v cmake >/dev/null 2>&1 || { echo "Error: cmake not found."; exit 1; }
command -v make >/dev/null 2>&1 || { echo "Error: make not found."; exit 1; }

# Install system dependencies (Ubuntu/Debian)
if command -v apt-get >/dev/null 2>&1; then
    echo "Installing system dependencies..."
    sudo apt-get update
    sudo apt-get install -y \
        libjsoncpp-dev \
        uuid-dev \
        zlib1g-dev \
        libssl-dev
fi

# Clone Drogon
TEMP_DIR=$(mktemp -d)
cd "$TEMP_DIR"

echo "Cloning Drogon from GitHub..."
git clone https://github.com/drogonframework/drogon.git
cd drogon
git submodule update --init

# Build and install
echo "Building Drogon..."
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)

echo "Installing Drogon..."
sudo make install

# Cleanup
cd /
rm -rf "$TEMP_DIR"

echo
echo "=== Drogon Installation Complete ==="
echo "You can now build WizardMerge with: ./build.sh"
