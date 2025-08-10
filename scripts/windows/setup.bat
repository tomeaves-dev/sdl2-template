@echo off
setlocal enabledelayedexpansion

:: SDL2 Template Setup Script for Windows
:: This script handles first-time setup including vcpkg installation and initial build

echo 🚀 Setting up SDL2 Template...
echo.

:: Get project root directory (parent of scripts directory)
set "PROJECT_ROOT=%~dp0..\.."
pushd "%PROJECT_ROOT%"

echo 📁 Project root: %CD%
echo.

:: Check if vcpkg exists
set "VCPKG_PATH=C:\vcpkg"
set "TOOLCHAIN_FILE=C:\vcpkg\scripts\buildsystems\vcpkg.cmake"

if not exist "%VCPKG_PATH%" (
    echo 📦 vcpkg not found. Installing vcpkg...
    echo.
    
    :: Check if Git is available
    git --version >nul 2>&1
    if errorlevel 1 (
        echo ❌ Git not found. Please install Git first:
        echo    https://git-scm.com/downloads
        pause
        exit /b 1
    )
    
    :: Check if Visual Studio or Build Tools are available
    where cl.exe >nul 2>&1
    if errorlevel 1 (
        echo ❌ Visual Studio Build Tools not found.
        echo Please install one of the following:
        echo   - Visual Studio 2019/2022 with C++ workload
        echo   - Build Tools for Visual Studio 2019/2022
        echo   - Visual Studio Community ^(free^)
        echo.
        echo Download from: https://visualstudio.microsoft.com/downloads/
        pause
        exit /b 1
    )
    
    echo ⬇️  Cloning vcpkg to C:\vcpkg...
    git clone https://github.com/Microsoft/vcpkg.git "%VCPKG_PATH%"
    
    if errorlevel 1 (
        echo ❌ Failed to clone vcpkg
        pause
        exit /b 1
    )
    
    echo 🔧 Bootstrapping vcpkg...
    pushd "%VCPKG_PATH%"
    call bootstrap-vcpkg.bat
    
    if errorlevel 1 (
        echo ❌ Failed to bootstrap vcpkg
        popd
        pause
        exit /b 1
    )
    popd
    
    echo.
    echo 💡 Consider adding vcpkg to your PATH:
    echo    Add C:\vcpkg to your system PATH environment variable
    echo.
) else (
    echo ✅ vcpkg found at %VCPKG_PATH%
)

:: Git Repository Setup
echo.
echo 🔗 Git Repository Setup
echo.
echo Currently this project is connected to the SDL2 template repository.
echo You have two options:
echo.
echo   1. Keep connection to template ^(you can pull future template updates^)
echo   2. Create your own independent repository ^(recommended for new projects^)
echo.
set /p "REPO_CHOICE=Would you like to disconnect from the template and create your own repository? (Y/n): "

if /i "!REPO_CHOICE!"=="y" goto create_repo
if /i "!REPO_CHOICE!"=="" goto create_repo
goto keep_repo

:create_repo
echo 🔄 Creating independent repository...

:: Remove the template's git history
if exist ".git" rmdir /s /q ".git"

:: Initialize a new git repository
git init

if errorlevel 1 (
    echo ❌ Failed to initialize git repository
    goto build_project
)

:: Add all files to the new repository
git add .

:: Create initial commit
git commit -m "Initial commit: SDL2 game template setup"

echo.
echo ✅ Independent repository created!
echo.
echo To connect to your own remote repository:
echo   1. Create a new repository on GitHub/GitLab/etc.
echo   2. Run: git remote add origin ^<your-repo-url^>
echo   3. Run: git push -u origin main
echo.
goto build_project

:keep_repo
echo ℹ️  Keeping connection to template repository.
echo    You can pull template updates with: git pull
echo.

:build_project
:: Install dependencies
echo 📚 Installing project dependencies...
"%VCPKG_PATH%\vcpkg.exe" install

if errorlevel 1 (
    echo ❌ Failed to install dependencies
    pause
    exit /b 1
)

:: Initial build
echo 🔨 Building project...
if not exist "build" mkdir build
pushd build

:: Configure project
cmake .. -DCMAKE_TOOLCHAIN_FILE="%TOOLCHAIN_FILE%" -A x64

if errorlevel 1 (
    echo ❌ Failed to configure project
    popd
    pause
    exit /b 1
)

:: Build project
cmake --build . --config Release

if errorlevel 1 (
    echo ❌ Failed to build project
    popd
    pause
    exit /b 1
)

popd

echo.
echo 🎉 Setup complete!
echo.
echo Quick start commands:
echo    scripts\run.bat      # Build and run the game
echo    scripts\build.bat    # Build the project
echo    scripts\clean.bat    # Clean build files
echo    scripts\install.bat  # Install/update dependencies
echo.
echo 🎮 To run your game now:
echo    build\Release\SDL2Template.exe
echo.
pause