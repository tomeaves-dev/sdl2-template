#!/bin/bash

# SDL2 Template Install Script for Unix (macOS and Linux)
# Installs or updates project dependencies via vcpkg

set -e  # Exit on any error

echo "📦 Installing/updating dependencies..."

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

# Check if vcpkg.json exists
if [ ! -f "vcpkg.json" ]; then
    echo "❌ vcpkg.json not found in project root"
    exit 1
fi

echo "📋 Installing dependencies from vcpkg.json..."

# Install/update dependencies
"$VCPKG_PATH/vcpkg" install

echo ""
echo "✅ Dependencies installed successfully!"
echo "🔨 Rebuild recommended: ./scripts/build"