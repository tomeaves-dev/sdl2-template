# SDL2 Game Development Template

A modern C++ game development template using SDL2 and popular game development libraries, pre-configured with vcpkg for easy dependency management.

## 🎮 Libraries Included

- **SDL2** - Core framework for window, input, and rendering
- **SDL2_image** - Image loading (PNG, JPG, etc.)
- **SDL2_mixer** - Audio playback and mixing
- **Box2D v3** - 2D physics simulation and collision detection
- **OpenGL + GLAD** - Hardware-accelerated rendering
- **Dear ImGui** - Debug UI and development tools
- **GLM** - Mathematics library for vectors, matrices, and transforms
- **spdlog** - Fast, asynchronous logging
- **fmt** - Modern string formatting
- **nlohmann/json** - JSON parsing for config files and save data
- **tmxlite** - Tiled map editor file loader

## 🚀 Quick Start

### Prerequisites

- **macOS**, Linux, or Windows
- **CMake 3.20+**
- **vcpkg** package manager
- **C++20** compatible compiler
- **pkg-config** (macOS/Linux)

### macOS Setup

```bash
# Install dependencies via Homebrew
brew install cmake pkg-config

# Clone and setup vcpkg
git clone https://github.com/Microsoft/vcpkg.git ~/vcpkg
cd ~/vcpkg
./bootstrap-vcpkg.sh

# Add vcpkg to PATH
echo 'export PATH="$HOME/vcpkg:$PATH"' >> ~/.zshrc
source ~/.zshrc
```

### Linux Setup

```bash
# Ubuntu/Debian
sudo apt update
sudo apt install cmake pkg-config build-essential

# Clone and setup vcpkg
git clone https://github.com/Microsoft/vcpkg.git ~/vcpkg
cd ~/vcpkg
./bootstrap-vcpkg.sh

# Add to PATH
echo 'export PATH="$HOME/vcpkg:$PATH"' >> ~/.bashrc
source ~/.bashrc
```

### Windows Setup

```bash
# Install vcpkg (run in PowerShell as Administrator)
git clone https://github.com/Microsoft/vcpkg.git C:\vcpkg
cd C:\vcpkg
.\bootstrap-vcpkg.bat

# Add to PATH or use full path in cmake commands
```

## 🔨 Build Instructions

### Option 1: Using CMake Presets (Recommended)

```bash
# Clone this template
git clone https://github.com/tomeaves-dev/sdl2-template.git your-project-name
cd sdl2-template

# Install all dependencies (first time only)
vcpkg install

# Configure and build using presets
cmake --preset default
cmake --build build

# Run the test program
./build/SDL2Template      # macOS/Linux
# OR
.\build\SDL2Template.exe  # Windows
```

### Option 2: Manual CMake Configuration

```bash
# Clone this template
git clone https://github.com/tomeaves-dev/sdl2-template.git your-project-name
cd sdl2-template

# Install all dependencies (first time only)
vcpkg install

# Create build directory
mkdir build && cd build

# Configure with vcpkg toolchain
# macOS/Linux:
cmake .. -DCMAKE_TOOLCHAIN_FILE=$HOME/vcpkg/scripts/buildsystems/vcpkg.cmake

# Windows:
cmake .. -DCMAKE_TOOLCHAIN_FILE=C:\vcpkg\scripts\buildsystems\vcpkg.cmake

# Build the project
make          # macOS/Linux
# OR
cmake --build . --config Release  # Windows/Cross-platform

# Run the test program
./SDL2Template      # macOS/Linux
# OR
.\SDL2Template.exe  # Windows
```

## 📁 Project Structure

```
sdl2-template/
├── src/                    # Source code
│   └── main.cpp           # Entry point with library tests
├── assets/                # Game assets
│   ├── textures/          # Images, sprites, tilesets
│   ├── audio/             # Sound effects and music
│   └── maps/              # Tiled map files (.tmx)
├── .vscode/               # VSCode configuration
│   └── settings.json      # Shared IDE settings
├── docs/                  # Documentation
├── build/                 # Build output (gitignored)
├── vcpkg_installed/       # Installed packages (gitignored)
├── CMakeLists.txt         # Build configuration
├── CMakePresets.json      # CMake preset configurations
├── vcpkg.json            # Dependency manifest
├── .gitignore            # Git ignore rules
├── LICENSE               # MIT license
└── README.md             # This file
```

## 🎯 Usage

This template provides a complete setup for 2D game development:

1. **Clone and Build**: Follow the build instructions above
2. **Start Coding**: Develop your game in the `src/` directory
3. **Add Assets**: Place textures, audio, and maps in `assets/`
4. **Configure**: Modify `CMakeLists.txt` as needed for your project

### Example Code Structure

```cpp
#include <SDL.h>
#include <box2d/box2d.h>
#include <glm/glm.hpp>
#include <spdlog/spdlog.h>

int main(int argc, char* argv[]) {
    // Initialize SDL
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    
    // Your game code here...
    
    SDL_Quit();
    return 0;
}
```

## 🛠️ Development Tips

- **Build System**: Use `cmake --preset default` for the simplest build experience
- **VSCode**: The included settings provide optimal C++ development experience
- **Physics**: Box2D v3 uses a new API with world IDs instead of direct objects
- **Logging**: Use spdlog for all logging needs - it's fast and configurable
- **Math**: GLM provides all the vector/matrix math you'll need
- **Assets**: The build system automatically copies the `assets/` folder to your build directory
- **Maps**: Use Tiled Map Editor to create levels, then load them with tmxlite
- **Debug UI**: ImGui is perfect for runtime debugging and parameter tweaking

## 🔧 Customization

### Renaming Your Project

To rename the project from "sdl2-template" to your game name:

1. **Update CMakeLists.txt**: Change the project name on line 4:
   ```cmake
   project(YourGameName VERSION 1.0.0)
   ```

2. **Update vcpkg.json**: Change the name field:
   ```json
   {
     "name": "yourgamename",
     "version": "1.0.0",
     ...
   }
   ```

3. **Rebuild**: The executable will now be named `YourGameName` instead of `SDL2Template`

### Adding New Dependencies

1. Add the package name to `vcpkg.json`
2. Add `find_package()` and `target_link_libraries()` calls to `CMakeLists.txt`
3. Run `vcpkg install` to fetch the new dependency

### Platform-Specific Notes

- **macOS**: Ensure Xcode Command Line Tools are installed
- **Linux**: May need additional development packages for specific distributions
- **Windows**: Use Visual Studio 2019+ or recent MinGW

## 📚 Resources

- [SDL2 Documentation](https://wiki.libsdl.org/)
- [Box2D v3 Documentation](https://box2d.org/documentation/)
- [Dear ImGui Examples](https://github.com/ocornut/imgui/tree/master/examples)
- [Tiled Map Editor](https://www.mapeditor.org/)
- [vcpkg Documentation](https://vcpkg.io/)

## 🤝 Contributing

1. Fork the repository
2. Create a feature branch
3. Make your improvements
4. Submit a pull request

## 📄 License

MIT License - Use this template for any project, commercial or personal!

---

**Happy Game Development! 🎮**