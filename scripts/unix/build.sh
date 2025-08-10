#!/bin/bash

# SDL2 Template Build Script for Unix (macOS and Linux)
# Builds the project using CMake

set -e  # Exit on any error

echo "🔨 Building SDL2 Template..."

# Get project root directory
PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
cd "$PROJECT_ROOT"

# Check if vcpkg exists
VCPKG_PATH="$HOME/vcpkg"
if [ ! -d "$VCPKG_PATH" ]; then
    echo "❌ vcpkg not found at $VCPKG_PATH"
    echo "Please run setup first: ./scripts/setup"
    exit 1
fi

TOOLCHAIN_FILE="$VCPKG_PATH/scripts/buildsystems/vcpkg.cmake"

# Create build directory if it doesn't exist
mkdir -p build
cd build

# Configure build type (default to Release if not specified)
BUILD_TYPE=${1:-Release}
echo "📋 Build type: $BUILD_TYPE"

# Configure project
echo "⚙️  Configuring project..."
cmake .. \
    -DCMAKE_TOOLCHAIN_FILE="$TOOLCHAIN_FILE" \
    -DCMAKE_BUILD_TYPE="$BUILD_TYPE"

# Build project
echo "🔧 Compiling..."
if command -v make &> /dev/null; then
    make -j$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)
elif command -v ninja &> /dev/null; then
    ninja
else
    cmake --build . --config "$BUILD_TYPE" --parallel
fi

echo ""
echo "✅ Build complete!"
echo "🎮 Run with: ./scripts/run"