#!/bin/bash

# SDL2 Template Setup Script for Unix (macOS and Linux)
# This script handles first-time setup including vcpkg installation and initial build

set -e  # Exit on any error

echo "🚀 Setting up SDL2 Template..."

# Detect platform
if [[ "$OSTYPE" == "darwin"* ]]; then
    PLATFORM="macOS"
    VCPKG_PATH="$HOME/vcpkg"
    TOOLCHAIN_FILE="$HOME/vcpkg/scripts/buildsystems/vcpkg.cmake"
elif [[ "$OSTYPE" == "linux"* ]]; then
    PLATFORM="Linux"
    VCPKG_PATH="$HOME/vcpkg"
    TOOLCHAIN_FILE="$HOME/vcpkg/scripts/buildsystems/vcpkg.cmake"
else
    echo "❌ Unsupported Unix platform: $OSTYPE"
    exit 1
fi

echo "📱 Platform detected: $PLATFORM"

# Get project root directory (parent of scripts directory)
PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
echo "📁 Project root: $PROJECT_ROOT"

# Check if vcpkg exists
if [ ! -d "$VCPKG_PATH" ]; then
    echo "📦 vcpkg not found. Installing vcpkg..."
    
    # Install system dependencies
    if [[ "$PLATFORM" == "macOS" ]]; then
        if ! command -v brew &> /dev/null; then
            echo "❌ Homebrew not found. Please install Homebrew first:"
            echo "   /bin/bash -c \"\$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)\""
            exit 1
        fi
        echo "🍺 Installing dependencies via Homebrew..."
        brew install cmake pkg-config git
    elif [[ "$PLATFORM" == "Linux" ]]; then
        echo "📦 Installing dependencies..."
        if command -v apt &> /dev/null; then
            sudo apt update
            sudo apt install -y cmake pkg-config build-essential git curl zip unzip tar
        elif command -v yum &> /dev/null; then
            sudo yum install -y cmake pkgconfig gcc-c++ git curl zip unzip tar
        elif command -v pacman &> /dev/null; then
            sudo pacman -S cmake pkgconfig base-devel git curl zip unzip tar
        elif command -v zypper &> /dev/null; then
            sudo zypper install -y cmake pkg-config gcc-c++ git curl zip unzip tar
        else
            echo "❌ Package manager not supported. Please install these manually:"
            echo "   - cmake, pkg-config, build-essential, git, curl, zip, unzip, tar"
            exit 1
        fi
    fi
    
    # Clone and setup vcpkg
    echo "⬇️  Cloning vcpkg..."
    git clone https://github.com/Microsoft/vcpkg.git "$VCPKG_PATH"
    cd "$VCPKG_PATH"
    ./bootstrap-vcpkg.sh
    
    # Add to PATH suggestion
    echo ""
    echo "💡 Consider adding vcpkg to your PATH by adding this to your shell config:"
    echo "   echo 'export PATH=\"\$HOME/vcpkg:\$PATH\"' >> ~/.$(basename $SHELL)rc"
    echo "   source ~/.$(basename $SHELL)rc"
    echo ""
else
    echo "✅ vcpkg found at $VCPKG_PATH"
fi

# Go to project directory
cd "$PROJECT_ROOT"

# Install dependencies
echo "📚 Installing project dependencies..."
"$VCPKG_PATH/vcpkg" install

# Git Repository Setup
echo ""
echo "🔗 Git Repository Setup"
echo ""
echo "Currently this project is connected to the SDL2 template repository."
echo "You have two options:"
echo ""
echo "  1. Keep connection to template (you can pull future template updates)"
echo "  2. Create your own independent repository (recommended for new projects)"
echo ""
read -p "Would you like to disconnect from the template and create your own repository? (Y/n): " -n 1 -r
echo ""

if [[ $REPLY =~ ^[Yy]$ ]] || [[ -z $REPLY ]]; then
    echo "🔄 Creating independent repository..."
    
    # Remove the template's git history
    rm -rf .git
    
    # Initialize a new git repository
    git init
    
    # Add all files to the new repository
    git add .
    
    # Create initial commit
    git commit -m "Initial commit: SDL2 game template setup"
    
    echo ""
    echo "✅ Independent repository created!"
    echo ""
    echo "To connect to your own remote repository:"
    echo "  1. Create a new repository on GitHub/GitLab/etc."
    echo "  2. Run: git remote add origin <your-repo-url>"
    echo "  3. Run: git push -u origin main"
    echo ""
else
    echo "ℹ️  Keeping connection to template repository."
    echo "   You can pull template updates with: git pull"
    echo ""
fi

# Project Configuration with validation
echo ""
echo "🏷️  Project Configuration"
echo ""
echo "Current project name: SDL2Template"

# Function to validate project name
validate_project_name() {
    local name="$1"
    
    # Check if name is empty
    if [[ -z "$name" ]]; then
        return 1
    fi
    
    # Check if name starts with a letter
    if [[ ! "$name" =~ ^[a-zA-Z] ]]; then
        echo "❌ Project name must start with a letter"
        return 1
    fi
    
    # Check if name contains only valid characters
    if [[ ! "$name" =~ ^[a-zA-Z][a-zA-Z0-9_-]*$ ]]; then
        echo "❌ Project name can only contain letters, numbers, hyphens, and underscores"
        return 1
    fi
    
    # Check for reserved names
    local reserved_names=("make" "cmake" "build" "run" "clean" "install" "test" "package")
    for reserved in "${reserved_names[@]}"; do
        if [[ "$name" == "$reserved" ]]; then
            echo "❌ '$name' is a reserved name and cannot be used"
            return 1
        fi
    done
    
    return 0
}

# Get project name with validation
while true; do
    read -p "Enter your project name (or press Enter to keep 'SDL2Template'): " PROJECT_NAME
    
    # Trim whitespace
    PROJECT_NAME=$(echo "$PROJECT_NAME" | xargs)
    
    # If empty, keep default
    if [[ -z "$PROJECT_NAME" ]]; then
        PROJECT_NAME="SDL2Template"
        break
    fi
    
    # Validate the name
    if validate_project_name "$PROJECT_NAME"; then
        break
    else
        echo ""
        echo "Please enter a valid project name:"
        echo "  - Must start with a letter"
        echo "  - Can contain letters, numbers, hyphens, and underscores"
        echo "  - Cannot be a reserved name (make, cmake, build, run, etc.)"
        echo ""
    fi
done

if [[ "$PROJECT_NAME" != "SDL2Template" ]]; then
    echo "🔧 Renaming project to: $PROJECT_NAME"
    
    # Update CMakeLists.txt with proper escaping for spaces
    if [ -f "CMakeLists.txt" ]; then
        # Use a more robust sed pattern that handles project names with special characters
        sed -i.bak "s/project(SDL2Template/project(\"$PROJECT_NAME\"/g" CMakeLists.txt
        # Also add LANGUAGES CXX to fix the CMake issue
        sed -i.bak "s/project(\"$PROJECT_NAME\" VERSION 1.0.0)/project(\"$PROJECT_NAME\" VERSION 1.0.0 LANGUAGES CXX)/g" CMakeLists.txt
        rm -f CMakeLists.txt.bak
        echo "  ✅ Updated CMakeLists.txt"
    fi
    
    # Update vcpkg.json
    if [ -f "vcpkg.json" ]; then
        # Convert to lowercase and replace spaces/special chars with hyphens for package name
        PACKAGE_NAME=$(echo "$PROJECT_NAME" | tr '[:upper:]' '[:lower:]' | sed 's/[^a-z0-9]/-/g' | sed 's/--*/-/g' | sed 's/^-\|-$//g')
        sed -i.bak "s/\"name\": \"sdl2-template\"/\"name\": \"$PACKAGE_NAME\"/g" vcpkg.json
        rm -f vcpkg.json.bak
        echo "  ✅ Updated vcpkg.json (package name: $PACKAGE_NAME)"
    fi
    
    # Update README.md title
    if [ -f "README.md" ]; then
        sed -i.bak "s/# SDL2 Game Development Template/# $PROJECT_NAME/g" README.md
        sed -i.bak "s/SDL2 Template/$PROJECT_NAME/g" README.md
        rm -f README.md.bak
        echo "  ✅ Updated README.md"
    fi
    
    # Update window title in settings.json
    if [ -f "assets/settings.json" ]; then
        sed -i.bak "s/\"title\": \"SDL2 Game\"/\"title\": \"$PROJECT_NAME\"/g" assets/settings.json
        rm -f assets/settings.json.bak
        echo "  ✅ Updated window title in assets/settings.json"
    fi
    
    echo ""
    echo "✅ Project renamed successfully!"
    echo "   Executable will be named: $PROJECT_NAME"
    echo ""
else
    echo "ℹ️  Keeping project name as 'SDL2Template'"
    echo ""
    
    # Still fix the CMakeLists.txt LANGUAGES issue
    if [ -f "CMakeLists.txt" ]; then
        sed -i.bak "s/project(SDL2Template VERSION 1.0.0)/project(SDL2Template VERSION 1.0.0 LANGUAGES CXX)/g" CMakeLists.txt
        rm -f CMakeLists.txt.bak
        echo "  ✅ Fixed CMakeLists.txt LANGUAGES issue"
    fi
fi

# Install dependencies again (in case there were changes)
echo "📚 Installing project dependencies..."
"$VCPKG_PATH/vcpkg" install

# Initial build
echo "🔨 Building project..."
mkdir -p build
cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE="$TOOLCHAIN_FILE"

# Use appropriate build command
if command -v make &> /dev/null; then
    make
elif command -v ninja &> /dev/null; then
    ninja
else
    cmake --build . --config Release
fi

echo ""
echo "🎉 Setup complete!"
echo ""
echo "Quick start commands:"
echo "   ./scripts/run      # Build and run the game"
echo "   ./scripts/build    # Build the project"
echo "   ./scripts/clean    # Clean build files"
echo "   ./scripts/install  # Install/update dependencies"
echo ""
echo "Or use the convenient aliases:"
echo "   make run           # Same as ./scripts/run"
echo "   make build         # Same as ./scripts/build"
echo "   make clean-all     # Same as ./scripts/clean"
echo ""
echo "🎮 To run your game now:"
if [[ "$PROJECT_NAME" == *" "* ]]; then
    echo "   ./build/\"$PROJECT_NAME\""
else
    echo "   ./build/$PROJECT_NAME"
fi