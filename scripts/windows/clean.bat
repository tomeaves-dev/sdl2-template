@echo off
setlocal

:: SDL2 Template Clean Script for Windows
:: Removes build artifacts and temporary files

echo 🧹 Cleaning SDL2 Template...

:: Get project root directory
set "PROJECT_ROOT=%~dp0..\.."
pushd "%PROJECT_ROOT%"

:: Remove build directory
if exist "build" (
    echo 🗑️  Removing build directory...
    rmdir /s /q "build"
    echo ✅ Build directory removed
) else (
    echo ℹ️  Build directory doesn't exist
)

:: Remove vcpkg installed packages (ask user)
if exist "vcpkg_installed" (
    echo.
    set /p "REMOVE_VCPKG=🤔 Remove vcpkg_installed directory? (y/N): "
    if /i "!REMOVE_VCPKG!"=="y" (
        echo 🗑️  Removing vcpkg_installed directory...
        rmdir /s /q "vcpkg_installed"
        echo ✅ vcpkg_installed directory removed
    ) else (
        echo ℹ️  Keeping vcpkg_installed directory
    )
)

:: Clean other common temporary files
echo 🧽 Cleaning temporary files...

:: Remove common temporary files
if exist "*.tmp" del /q "*.tmp" 2>nul
if exist "*.log" del /q "*.log" 2>nul
if exist "Thumbs.db" del /q "Thumbs.db" 2>nul

:: Remove IDE temporary files
if exist ".vs" rmdir /s /q ".vs" 2>nul
if exist "cmake-build-debug" rmdir /s /q "cmake-build-debug" 2>nul
if exist "cmake-build-release" rmdir /s /q "cmake-build-release" 2>nul

:: Remove Visual Studio temporary files
for /d %%d in (x64 Win32) do (
    if exist "%%d" rmdir /s /q "%%d" 2>nul
)

popd

echo.
echo ✨ Cleaning complete!
echo 🔄 To rebuild: scripts\build.bat
echo.
pause