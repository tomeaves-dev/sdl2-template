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
    goto project_config
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
goto project_config

:keep_repo
echo ℹ️  Keeping connection to template repository.
echo    You can pull template updates with: git pull
echo.

:project_config
:: Project Configuration with validation
echo.
echo 🏷️  Project Configuration
echo.
echo Current project name: SDL2Template

:get_project_name
set /p "PROJECT_NAME=Enter your project name (or press Enter to keep 'SDL2Template'): "

:: Trim whitespace
for /f "tokens=* delims= " %%a in ("%PROJECT_NAME%") do set PROJECT_NAME=%%a
for /l %%a in (1,1,100) do if "!PROJECT_NAME:~-1!"==" " set PROJECT_NAME=!PROJECT_NAME:~0,-1!

:: If empty, keep default
if "%PROJECT_NAME%"=="" (
    set "PROJECT_NAME=SDL2Template"
    goto name_validated
)

:: Validate project name
call :validate_project_name "%PROJECT_NAME%"
if errorlevel 1 goto get_project_name

:name_validated
if not "%PROJECT_NAME%"=="SDL2Template" (
    echo 🔧 Renaming project to: !PROJECT_NAME!
    
    :: Update CMakeLists.txt with LANGUAGES fix
    if exist "CMakeLists.txt" (
        powershell -Command "(Get-Content CMakeLists.txt) -replace 'project\(SDL2Template', 'project(\"!PROJECT_NAME!\"' | Set-Content CMakeLists.txt"
        powershell -Command "(Get-Content CMakeLists.txt) -replace 'project\(\"!PROJECT_NAME!\" VERSION 1.0.0\)', 'project(\"!PROJECT_NAME!\" VERSION 1.0.0 LANGUAGES CXX)' | Set-Content CMakeLists.txt"
        echo   ✅ Updated CMakeLists.txt
    )
    
    :: Update vcpkg.json (create lowercase package name)
    if exist "vcpkg.json" (
        :: Convert to lowercase and replace special chars
        set "PACKAGE_NAME=!PROJECT_NAME!"
        for %%i in (A B C D E F G H I J K L M N O P Q R S T U V W X Y Z) do (
            call set "PACKAGE_NAME=%%PACKAGE_NAME:%%i=%%i%%"
        )
        set "PACKAGE_NAME=!PACKAGE_NAME: =-!"
        set "PACKAGE_NAME=!PACKAGE_NAME:_=-!"
        powershell -Command "(Get-Content vcpkg.json) -replace '\"name\": \"sdl2-template\"', '\"name\": \"!PACKAGE_NAME!\"' | Set-Content vcpkg.json"
        echo   ✅ Updated vcpkg.json
    )
    
    :: Update README.md
    if exist "README.md" (
        powershell -Command "(Get-Content README.md) -replace '# SDL2 Game Development Template', '# !PROJECT_NAME!' | Set-Content README.md"
        powershell -Command "(Get-Content README.md) -replace 'SDL2 Template', '!PROJECT_NAME!' | Set-Content README.md"
        echo   ✅ Updated README.md
    )
    
    :: Update window title in settings.json
    if exist "assets\settings.json" (
        powershell -Command "(Get-Content assets\settings.json) -replace '\"title\": \"SDL2 Game\"', '\"title\": \"!PROJECT_NAME!\"' | Set-Content assets\settings.json"
        echo   ✅ Updated window title in assets\settings.json
    )
    
    echo.
    echo ✅ Project renamed successfully!
    echo    Executable will be named: !PROJECT_NAME!.exe
    echo.
) else (
    echo ℹ️  Keeping project name as 'SDL2Template'
    echo.
    
    :: Still fix the CMakeLists.txt LANGUAGES issue
    if exist "CMakeLists.txt" (
        powershell -Command "(Get-Content CMakeLists.txt) -replace 'project\(SDL2Template VERSION 1.0.0\)', 'project(SDL2Template VERSION 1.0.0 LANGUAGES CXX)' | Set-Content CMakeLists.txt"
        echo   ✅ Fixed CMakeLists.txt LANGUAGES issue
    )
)

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
if "%PROJECT_NAME%" NEQ "%PROJECT_NAME: =%" (
    echo    "build\Release\!PROJECT_NAME!.exe"
) else (
    echo    build\Release\!PROJECT_NAME!.exe
)
echo.
pause
goto :eof

:: Function to validate project name
:validate_project_name
set "name=%~1"

:: Check if name is empty
if "%name%"=="" (
    echo ❌ Project name cannot be empty
    exit /b 1
)

:: Check if name starts with a letter
set "first_char=%name:~0,1%"
echo %first_char%|findstr /r "^[A-Za-z]$" >nul
if errorlevel 1 (
    echo ❌ Project name must start with a letter
    goto validation_help
)

:: Check for invalid characters (only allow letters, numbers, hyphens, underscores)
echo %name%|findstr /r "^[A-Za-z][A-Za-z0-9_-]*$" >nul
if errorlevel 1 (
    echo ❌ Project name can only contain letters, numbers, hyphens, and underscores
    goto validation_help
)

:: Check for reserved names
set "reserved_names=make cmake build run clean install test package"
for %%r in (%reserved_names%) do (
    if /i "%name%"=="%%r" (
        echo ❌ '%name%' is a reserved name and cannot be used
        goto validation_help
    )
)

:: Name is valid
exit /b 0

:validation_help
echo.
echo Please enter a valid project name:
echo   - Must start with a letter
echo   - Can contain letters, numbers, hyphens, and underscores
echo   - Cannot be a reserved name ^(make, cmake, build, run, etc.^)
echo.
exit /b 1