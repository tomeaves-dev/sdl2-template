@echo off
setlocal

:: SDL2 Template Build Script for Windows
:: Builds the project using CMake and Visual Studio

echo 🔨 Building SDL2 Template...

:: Get project root directory
set "PROJECT_ROOT=%~dp0..\.."
pushd "%PROJECT_ROOT%"

:: Check if vcpkg exists
set "VCPKG_PATH=C:\vcpkg"
if not exist "%VCPKG_PATH%" (
    echo ❌ vcpkg not found at %VCPKG_PATH%
    echo Please run setup first: scripts\setup.bat
    pause
    exit /b 1
)

set "TOOLCHAIN_FILE=C:\vcpkg\scripts\buildsystems\vcpkg.cmake"

:: Create build directory if it doesn't exist
if not exist "build" mkdir build
pushd build

:: Get build type from command line argument (default to Release)
set "BUILD_TYPE=%1"
if "%BUILD_TYPE%"=="" set "BUILD_TYPE=Release"

echo 📋 Build type: %BUILD_TYPE%

:: Configure project
echo ⚙️  Configuring project...
cmake .. -DCMAKE_TOOLCHAIN_FILE="%TOOLCHAIN_FILE%" -A x64

if errorlevel 1 (
    echo ❌ Failed to configure project
    popd
    popd
    pause
    exit /b 1
)

:: Build project
echo 🔧 Compiling...
cmake --build . --config %BUILD_TYPE% --parallel

if errorlevel 1 (
    echo ❌ Build failed
    popd
    popd
    pause
    exit /b 1
)

popd
popd

echo.
echo ✅ Build complete!
echo 🎮 Run with: scripts\run.bat