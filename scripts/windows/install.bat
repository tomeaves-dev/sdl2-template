@echo off
setlocal

:: SDL2 Template Install Script for Windows
:: Installs or updates project dependencies via vcpkg

echo 📦 Installing/updating dependencies...

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

:: Check if vcpkg.json exists
if not exist "vcpkg.json" (
    echo ❌ vcpkg.json not found in project root
    pause
    exit /b 1
)

echo 📋 Installing dependencies from vcpkg.json...

:: Install/update dependencies
"%VCPKG_PATH%\vcpkg.exe" install

if errorlevel 1 (
    echo ❌ Failed to install dependencies
    popd
    pause
    exit /b 1
)

popd

echo.
echo ✅ Dependencies installed successfully!
echo 🔨 Rebuild recommended: scripts\build.bat
echo.
pause