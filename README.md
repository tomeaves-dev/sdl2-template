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

- **macOS, Linux, or Windows**
- **Git** (for cloning and vcpkg)
- **C++20** compatible compiler
- Platform-specific build tools (auto-installed by setup script):
  - **macOS**: Xcode Command Line Tools, Homebrew
  - **Linux**: build-essential, cmake, pkg-config  
  - **Windows**: Visual Studio 2019+ with C++ workload

### Platform Setup (Optional - Handled by Scripts)

<details>
<summary>Manual platform setup (only if auto-setup fails)</summary>

#### macOS Setup

```bash
# Install dependencies via Homebrew
brew install cmake pkg-config git

# Clone and setup vcpkg
git clone https://github.com/Microsoft/vcpkg.git ~/vcpkg
cd ~/vcpkg
./bootstrap-vcpkg.sh

# Add vcpkg to PATH
echo 'export PATH="$HOME/vcpkg:$PATH"' >> ~/.zshrc
source ~/.zshrc
```

#### Linux Setup

```bash
# Ubuntu/Debian
sudo apt update
sudo apt install cmake pkg-config build-essential git curl zip unzip tar

# Clone and setup vcpkg
git clone https://github.com/Microsoft/vcpkg.git ~/vcpkg
cd ~/vcpkg
./bootstrap-vcpkg.sh

# Add to PATH
echo 'export PATH="$HOME/vcpkg:$PATH"' >> ~/.bashrc
source ~/.bashrc
```

#### Windows Setup

```cmd
# Install vcpkg (run in Command Prompt as Administrator)
git clone https://github.com/Microsoft/vcpkg.git C:\vcpkg
cd C:\vcpkg
.\bootstrap-vcpkg.bat

# Add to PATH or use full path in cmake commands
```

</details>

## 🔨 Build Instructions

### Quick Start (Recommended)

```bash
# Clone this template with your own project name
# Replace 'your-game-name' with whatever you want to call your project
git clone https://github.com/tomeaves-dev/sdl2-template.git your-game-name
cd your-game-name

# One-command setup (auto-detects your platform)
make setup

# Run your game
make run
```

**That's it!** The setup command will:
- Install vcpkg and all dependencies automatically
- Optionally disconnect from template repo and create your own
- Let you rename the project from "SDL2Template" to whatever you want
- Build the project
- Get you ready to start developing

### Alternative Setup Methods

#### Option 1: Using Make Commands (Cross-Platform)
```bash
make setup     # First-time setup
make build     # Build the project  
make run       # Build and run
make clean     # Clean build files
make debug     # Build in debug mode
make release   # Build in release mode
```

#### Option 2: Using Platform-Agnostic Scripts
```bash
./scripts/setup    # Auto-detects platform
./scripts/build
./scripts/run
./scripts/clean
./scripts/install  # Update dependencies
```

#### Option 3: Platform-Specific Scripts

**Unix/Linux/macOS:**
```bash
./scripts/unix/setup.sh
./scripts/unix/build.sh
./scripts/unix/run.sh
```

**Windows (Command Prompt):**
```cmd
scripts\windows\setup.bat
scripts\windows\build.bat
scripts\windows\run.bat
```

**Windows (PowerShell):**
```powershell
.\scripts\powershell\setup.ps1
.\scripts\powershell\build.ps1
.\scripts\powershell\run.ps1
```

### Manual Installation (If Auto-Setup Fails)

<details>
<summary>Click to expand manual installation instructions</summary>

#### Prerequisites
- **macOS**: Xcode Command Line Tools, Homebrew
- **Linux**: build-essential, cmake, pkg-config, git
- **Windows**: Visual Studio 2019+ with C++ workload

#### Install vcpkg
```bash
# macOS/Linux
git clone https://github.com/Microsoft/vcpkg.git ~/vcpkg
cd ~/vcpkg && ./bootstrap-vcpkg.sh

# Windows
git clone https://github.com/Microsoft/vcpkg.git C:\vcpkg
cd C:\vcpkg && .\bootstrap-vcpkg.bat
```

#### Build Project
```bash
# Create build directory
mkdir build && cd build

# Configure (adjust vcpkg path for your system)
cmake .. -DCMAKE_TOOLCHAIN_FILE=$HOME/vcpkg/scripts/buildsystems/vcpkg.cmake

# Build
make  # or cmake --build . --config Release
```

</details>

## 📁 Project Structure

```
sdl2-template/
├── src/                     # Source code
│   ├── main.cpp            # Entry point
│   ├── public/             # Public headers (interfaces)
│   │   ├── core/           # Core game systems
│   │   ├── rendering/      # Graphics and rendering
│   │   ├── physics/        # Physics simulation
│   │   ├── input/          # Input handling
│   │   └── utils/          # Utility functions
│   └── private/            # Implementation files
│       ├── core/           # Core system implementations  
│       ├── rendering/      # Rendering implementations
│       ├── physics/        # Physics implementations
│       ├── input/          # Input implementations
│       └── utils/          # Utility implementations
├── assets/                 # Game assets
│   ├── textures/           # Images, sprites, tilesets
│   ├── audio/              # Sound effects and music
│   └── maps/               # Tiled map files (.tmx)
├── scripts/                # Build and utility scripts
│   ├── setup, build, run   # Platform-agnostic entry points
│   ├── unix/               # macOS/Linux scripts
│   ├── windows/            # Windows batch scripts
│   └── powershell/         # Windows PowerShell scripts
├── ide-configs/            # IDE configuration templates
│   ├── vscode/             # VS Code settings
│   ├── clion/              # CLion settings
│   └── visual-studio/      # Visual Studio settings
├── docs/                   # Documentation
├── build/                  # Build output (gitignored)
├── vcpkg_installed/        # Installed packages (gitignored)
├── CMakeLists.txt          # Build configuration
├── CMakePresets.json       # CMake preset configurations
├── Makefile                # Convenient build aliases
├── vcpkg.json              # Dependency manifest
├── .gitignore              # Git ignore rules
├── LICENSE                 # MIT license
└── README.md               # This file
```

## 🎯 Usage

This template provides a complete setup for 2D game development with a modern C++ architecture:

### Getting Started
1. **Clone and Setup**: Use `make setup` for automatic configuration and project naming
2. **Start Coding**: Develop your game using the provided class structure
3. **Add Assets**: Place textures, audio, and maps in `assets/`
4. **Build and Run**: Use `make run` for quick iteration

### Code Architecture

The template follows a **public/private header pattern** for clean separation:

- **`src/public/`** - Interface definitions (what other systems can use)
- **`src/private/`** - Implementation details (how systems work internally)

**Core Systems Included:**
- **Game** - Main game loop and system coordination
- **Window** - SDL2 window management  
- **Renderer** - OpenGL rendering pipeline
- **Physics** - Box2D physics integration (gravity optional)
- **Input** - Keyboard and mouse handling
- **Logger** - Structured logging with spdlog

### Example Usage

```cpp
// Example: Adding gravity to your game
auto game = std::make_unique<core::Game>();
game->Initialize();

// Get physics system and enable gravity
auto* physics = game->GetPhysics();
physics->SetEarthGravity();  // or SetLowGravity() for platformers

game->Run();
```

### Development Workflow

```bash
# Daily development cycle
make run          # Quick build and test
make debug        # Debug build for troubleshooting  
make clean        # Clean slate rebuild
make install      # Update dependencies (when adding new libs)
```

## 🛠️ Development Tips

- **Build System**: Use `make setup` for first-time setup, then `make run` for daily development
- **Cross-Platform**: All scripts work on Windows, macOS, and Linux automatically
- **IDE Support**: Copy configs from `ide-configs/` for your preferred IDE (VS Code, CLion, Visual Studio)
- **Physics**: Box2D v3 uses a new API with world IDs instead of direct objects - gravity is disabled by default
- **Logging**: Use spdlog for all logging needs - it's fast and configurable
- **Math**: GLM provides all the vector/matrix math you'll need
- **Assets**: The build system automatically copies the `assets/` folder to your build directory
- **Maps**: Use Tiled Map Editor to create levels, then load them with tmxlite
- **Debug UI**: ImGui is included for runtime debugging and parameter tweaking
- **Git Workflow**: Setup script can disconnect from template repo and create your own automatically

## 🔧 Customization

### Renaming Your Project

The setup script will automatically offer to rename your project from "SDL2Template" to your preferred name. This updates:

1. **CMakeLists.txt**: Changes the executable name
2. **vcpkg.json**: Updates the package name  
3. **README.md**: Updates project title and references

You can also rename manually later by editing these files, or run the rename process again:

```bash
# Re-run just the setup to rename
make setup
```

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