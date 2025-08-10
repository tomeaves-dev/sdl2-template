@echo off
setlocal

:: SDL2 Template Run Script for Windows
:: Builds (if needed) and runs the game

echo 🎮 Running SDL2 Template...

:: Get project root directory
set "PROJECT_ROOT=%~dp0..\.."
pushd "%PROJECT_ROOT%"

:: Detect the actual executable name from CMakeLists.txt
set "EXECUTABLE_NAME=SDL2Template"
if exist "CMakeLists.txt" (
    for /f "tokens=1,2 delims=()" %%a in ('findstr /r "^project(" CMakeLists.txt') do (
        for /f "tokens=1" %%c in ("%%b") do set "EXECUTABLE_NAME=%%c"
    )
)

echo 🎯 Looking for executable: %EXECUTABLE_NAME%

:: Check if executable exists (try both Debug and Release)
set "EXECUTABLE_RELEASE=build\Release\%EXECUTABLE_NAME%.exe"
set "EXECUTABLE_DEBUG=build\Debug\%EXECUTABLE_NAME%.exe"
set "EXECUTABLE="

if exist "%EXECUTABLE_RELEASE%" (
    set "EXECUTABLE=%EXECUTABLE_RELEASE%"
    set "BUILD_CONFIG=Release"
) else if exist "%EXECUTABLE_DEBUG%" (
    set "EXECUTABLE=%EXECUTABLE_DEBUG%"
    set "BUILD_CONFIG=Debug"
)

if "%EXECUTABLE%"=="" (
    echo 📦 Executable not found. Building first...
    call scripts\windows\build.bat
    
    :: Check again after build
    if exist "%EXECUTABLE_RELEASE%" (
        set "EXECUTABLE=%EXECUTABLE_RELEASE%"
        set "BUILD_CONFIG=Release"
    ) else if exist "%EXECUTABLE_DEBUG%" (
        set "EXECUTABLE=%EXECUTABLE_DEBUG%"
        set "BUILD_CONFIG=Debug"
    ) else (
        echo ❌ Failed to build executable
        popd
        pause
        exit /b 1
    )
)

:: Simple dependency check - if source files are newer, rebuild
if exist "src" (
    for /f %%i in ('forfiles /p src /m *.cpp /c "cmd /c echo @fdate @ftime" 2^>nul ^| sort /r ^| head -1') do set "NEWEST_SOURCE=%%i"
    for %%i in ("%EXECUTABLE%") do set "EXECUTABLE_DATE=%%~ti"
    
    :: Note: This is a simplified check. For more robust checking, consider using PowerShell
)

echo 🚀 Starting game (%BUILD_CONFIG% build)...
echo.

:: Run the game
pushd build
if "%BUILD_CONFIG%"=="Release" (
    Release\%EXECUTABLE_NAME%.exe %*
) else (
    Debug\%EXECUTABLE_NAME%.exe %*
)

set "GAME_EXIT_CODE=%ERRORLEVEL%"
popd
popd

echo.
echo 🏁 Game finished (exit code: %GAME_EXIT_CODE%)

if not "%GAME_EXIT_CODE%"=="0" (
    echo ⚠️  Game exited with error code %GAME_EXIT_CODE%
    pause
)