# IDE Configuration Templates

This directory contains configuration templates for popular C++ IDEs. Choose the IDE you're using and follow the setup instructions.

## 🎯 Quick Setup

### VS Code (Recommended)
1. Copy the `vscode/` folder contents to your project root as `.vscode/`
2. Install recommended extensions:
   - C/C++ Extension Pack
   - CMake Tools
3. Open project and press `Ctrl+Shift+P` → "CMake: Configure"

### CLion
1. Open project in CLion
2. Copy settings from `clion/CMakeSettings.json` to CLion's CMake settings
3. Follow instructions in `clion/.idea-settings.md`

### Visual Studio (Windows)
1. Open project as CMake project
2. Copy settings from `visual-studio/CMakeSettings.json` to VS CMake settings  
3. Follow instructions in `visual-studio/.vs-settings.md`

## 📁 Directory Structure

```
ide-configs/
├── README.md              # This file
├── vscode/                # VS Code configuration
│   ├── settings.json      # Editor settings
│   ├── tasks.json         # Build tasks
│   ├── launch.json        # Debug configuration
│   └── c_cpp_properties.json # IntelliSense settings
├── clion/                 # CLion configuration
│   ├── CMakeSettings.json # CMake profiles
│   └── .idea-settings.md  # Setup instructions
└── visual-studio/         # Visual Studio configuration
    ├── CMakeSettings.json # CMake configurations
    └── .vs-settings.md    # Setup instructions
```

## 🔧 Features Included

### All IDEs
- ✅ CMake integration with vcpkg
- ✅ Debug and Release configurations
- ✅ Proper include paths for IntelliSense
- ✅ C++20 standard support

### VS Code Specific
- ✅ Build tasks (Ctrl+Shift+P → Tasks)
- ✅ Debug configurations (F5)
- ✅ Cross-platform debugging (macOS/Linux/Windows)
- ✅ Integrated terminal support

### CLion Specific  
- ✅ Multiple CMake profiles
- ✅ Built-in debugging
- ✅ Code analysis and refactoring tools

### Visual Studio Specific
- ✅ x64 and x86 configurations
- ✅ Ninja generator for fast builds
- ✅ Integrated debugger
- ✅ IntelliSense support

## 🚀 Getting Started

1. **Choose your IDE** from the options above
2. **Copy the configuration files** to your project
3. **Install required extensions/tools** (if any)
4. **Open the project** in your IDE
5. **Build and run** using `./scripts/build` or IDE build commands

## 🎮 Building and Running

All IDEs are configured to work with our build scripts:

```bash
# Build the project
./scripts/build

# Run the game  
./scripts/run

# Or use IDE-specific build/run commands
```

## 🔍 Troubleshooting

### Common Issues

**Problem**: CMake can't find vcpkg
**Solution**: Ensure vcpkg is installed and paths in config files are correct

**Problem**: IntelliSense not working
**Solution**: Check include paths in configuration files match your system

**Problem**: Debugger not working
**Solution**: Build in Debug mode and check debugger paths in launch configurations

### Platform-Specific Notes

**macOS**: Uses `lldb` debugger
**Linux**: Uses `gdb` debugger  
**Windows**: Uses Visual Studio debugger

## 📝 Customization

Feel free to modify these configurations for your needs:
- Adjust compiler flags
- Add custom build tasks
- Modify debugger settings
- Add additional include paths

## 🤝 Contributing

If you improve these configurations or add support for other IDEs, consider contributing back to the template!