#!/bin/bash

# SDL2 Template Clean Script for Unix (macOS and Linux)
# Removes build artifacts and temporary files

set -e  # Exit on any error

echo "🧹 Cleaning SDL2 Template..."

# Get project root directory
PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
cd "$PROJECT_ROOT"

# Remove build directory
if [ -d "build" ]; then
    echo "🗑️  Removing build directory..."
    rm -rf build
    echo "✅ Build directory removed"
else
    echo "ℹ️  Build directory doesn't exist"
fi

# Remove vcpkg installed packages (optional - ask user)
if [ -d "vcpkg_installed" ]; then
    echo ""
    read -p "🤔 Remove vcpkg_installed directory? (y/N): " -n 1 -r
    echo
    if [[ $REPLY =~ ^[Yy]$ ]]; then
        echo "🗑️  Removing vcpkg_installed directory..."
        rm -rf vcpkg_installed
        echo "✅ vcpkg_installed directory removed"
    else
        echo "ℹ️  Keeping vcpkg_installed directory"
    fi
fi

# Clean other common temporary files
echo "🧽 Cleaning temporary files..."

# Remove common temporary files
find . -name "*.tmp" -delete 2>/dev/null || true
find . -name "*.log" -delete 2>/dev/null || true
find . -name ".DS_Store" -delete 2>/dev/null || true
find . -name "Thumbs.db" -delete 2>/dev/null || true

# Remove IDE temporary files
rm -rf .vs/ 2>/dev/null || true
rm -rf cmake-build-* 2>/dev/null || true

echo ""
echo "✨ Cleaning complete!"
echo "🔄 To rebuild: ./scripts/build"