#!/bin/bash

# SDL2 Template Run Script for Unix (macOS and Linux)
# Builds (if needed) and runs the game

set -e  # Exit on any error

echo "🎮 Running SDL2 Template..."

# Get project root directory
PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
cd "$PROJECT_ROOT"

# Check if executable exists
EXECUTABLE="build/SDL2Template"
if [ ! -f "$EXECUTABLE" ]; then
    echo "📦 Executable not found. Building first..."
    ./scripts/unix/build.sh
fi

# Check if executable is older than source files (simple dependency check)
if [ -d "src" ]; then
    NEWEST_SOURCE=$(find src -name "*.cpp" -o -name "*.h" | xargs ls -t | head -n1)
    if [ "$NEWEST_SOURCE" -nt "$EXECUTABLE" ]; then
        echo "🔄 Source files updated. Rebuilding..."
        ./scripts/unix/build.sh
    fi
fi

echo "🚀 Starting game..."
echo ""

# Run the game
cd build
./SDL2Template "$@"  # Pass any command line arguments to the game

echo ""
echo "🏁 Game finished."