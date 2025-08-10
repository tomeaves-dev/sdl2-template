#!/bin/bash

# SDL2 Template Run Script for Unix (macOS and Linux)
# Builds (if needed) and runs the game

set -e  # Exit on any error

echo "🎮 Running SDL2 Template..."

# Get project root directory
PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
cd "$PROJECT_ROOT"

# Detect the actual executable name from CMakeLists.txt
EXECUTABLE_NAME="SDL2Template"  # Default fallback
if [ -f "CMakeLists.txt" ]; then
    # Extract project name from CMakeLists.txt
    DETECTED_NAME=$(grep -E "^project\(" CMakeLists.txt | sed -E 's/project\(([^[:space:]]+).*/\1/' | head -1)
    if [ ! -z "$DETECTED_NAME" ]; then
        EXECUTABLE_NAME="$DETECTED_NAME"
    fi
fi

echo "🎯 Looking for executable: $EXECUTABLE_NAME"

# Check if executable exists
EXECUTABLE="build/$EXECUTABLE_NAME"
if [ ! -f "$EXECUTABLE" ]; then
    echo "📦 Executable not found. Building first..."
    ./scripts/unix/build.sh
fi

# Check if executable is older than source files (simple dependency check)
if [ -d "src" ]; then
    NEWEST_SOURCE=$(find src -name "*.cpp" -o -name "*.h" | xargs ls -t 2>/dev/null | head -n1)
    if [ ! -z "$NEWEST_SOURCE" ] && [ "$NEWEST_SOURCE" -nt "$EXECUTABLE" ]; then
        echo "🔄 Source files updated. Rebuilding..."
        ./scripts/unix/build.sh
    fi
fi

echo "🚀 Starting game..."
echo ""

# Run the game
cd build
./$EXECUTABLE_NAME "$@"  # Pass any command line arguments to the game

echo ""
echo "🏁 Game finished."