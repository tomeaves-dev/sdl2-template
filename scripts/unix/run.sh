#!/bin/bash

# SDL2 Template Run Script for Unix (macOS and Linux)
# Builds (if needed) and runs the game

set -e  # Exit on any error

echo "🎮 Running SDL2 Template..."

# Get project root directory
PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
cd "$PROJECT_ROOT"

# Function to extract project name from CMakeLists.txt
get_project_name() {
    if [ ! -f "CMakeLists.txt" ]; then
        echo "SDL2Template"
        return
    fi
    
    # Look for project(Name or project("Name" or project( Name
    local project_line=$(grep -E "^project\(" CMakeLists.txt | head -1)
    if [ -z "$project_line" ]; then
        echo "SDL2Template"
        return
    fi
    
    # Extract the project name - handle quotes and spaces
    local name=$(echo "$project_line" | sed -E 's/^project\([ ]*["]?([^",) ]+)["]?.*$/\1/')
    
    # Validate we got a real name
    if [ -z "$name" ] || [ "$name" = "project" ]; then
        echo "SDL2Template"
    else
        echo "$name"
    fi
}

# Get the actual project name
EXECUTABLE_NAME=$(get_project_name)

echo "🎯 Looking for executable: $EXECUTABLE_NAME"

# Check if executable exists
EXECUTABLE="build/$EXECUTABLE_NAME"
if [ ! -f "$EXECUTABLE" ]; then
    echo "📦 Executable not found. Building first..."
    ./scripts/unix/build.sh
fi

# Check if executable is older than source files (simple dependency check)
if [ -d "src" ]; then
    NEWEST_SOURCE=$(find src -name "*.cpp" -o -name "*.h" 2>/dev/null | xargs ls -t 2>/dev/null | head -n1)
    if [ ! -z "$NEWEST_SOURCE" ] && [ "$NEWEST_SOURCE" -nt "$EXECUTABLE" ]; then
        echo "🔄 Source files updated. Rebuilding..."
        ./scripts/unix/build.sh
    fi
fi

echo "🚀 Starting game..."
echo ""

# Run the game - handle names with spaces properly
cd build
if [[ "$EXECUTABLE_NAME" == *" "* ]]; then
    # Name has spaces, use quotes
    "./$EXECUTABLE_NAME" "$@"
else
    # Name has no spaces, no quotes needed
    ./"$EXECUTABLE_NAME" "$@"
fi

GAME_EXIT_CODE=$?

echo ""
if [ $GAME_EXIT_CODE -eq 0 ]; then
    echo "🏁 Game finished successfully."
else
    echo "🏁 Game finished with exit code: $GAME_EXIT_CODE"
fi