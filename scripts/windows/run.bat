@echo off
setlocal enabledelayedexpansion

:: SDL2 Template Run Script for Windows
:: Builds (if needed) and runs the game

echo 🎮 Running SDL2 Template...

:: Get project root directory
set "PROJECT_ROOT=%~dp0..\.."
pushd "%PROJECT_ROOT%"

:: Function to extract project name from CMakeLists.txt
call :get_project_name EXECUTABLE_NAME

echo 🎯 Looking for executable: !EXECUTABLE_NAME!

:: Check if executable exists (try both Debug and Release)
set "EXECUTABLE_RELEASE=build\Release\!EXECUTABLE_NAME!.exe"
set "EXECUTABLE_DEBUG=build\Debug\!EXECUTABLE_NAME!.exe"
set "EXECUTABLE="

if exist "!EXECUTABLE_RELEASE!" (
    set "EXECUTABLE=!EXECUTABLE_RELEASE!"
    set "BUILD_CONFIG=Release"
) else if exist "!EXECUTABLE_DEBUG!" (
    set "EXECUTABLE=!EXECUTABLE_DEBUG!"
    set "BUILD_CONFIG=Debug"
)

if "!EXECUTABLE!"=="" (
    echo 📦 Executable not found. Building first...
    call scripts\windows\build.bat
    
    :: Check again after build
    if exist "!EXECUTABLE_RELEASE!" (
        set "EXECUTABLE=!EXECUTABLE_RELEASE!"
        set "BUILD_CONFIG=Release"
    ) else if exist "!EXECUTABLE_DEBUG!" (
        set "EXECUTABLE=!EXECUTABLE_DEBUG!"
        set "BUILD_CONFIG=Debug"
    ) else (
        echo ❌ Failed to build executable
        popd
        pause
        exit /b 1
    )
)

echo 🚀 Starting game (!BUILD_CONFIG! build)...
echo.

:: Run the game - handle names with spaces properly
pushd build
if "!BUILD_CONFIG!"=="Release" (
    if "!EXECUTABLE_NAME!" NEQ "!EXECUTABLE_NAME: =!" (
        :: Name has spaces, use quotes
        "Release\!EXECUTABLE_NAME!.exe" %*
    ) else (
        :: Name has no spaces, no quotes needed
        Release\!EXECUTABLE_NAME!.exe %*
    )
) else (
    if "!EXECUTABLE_NAME!" NEQ "!EXECUTABLE_NAME: =!" (
        :: Name has spaces, use quotes
        "Debug\!EXECUTABLE_NAME!.exe" %*
    ) else (
        :: Name has no spaces, no quotes needed
        Debug\!EXECUTABLE_NAME!.exe %*
    )
)

set "GAME_EXIT_CODE=%ERRORLEVEL%"
popd
popd

echo.
if "%GAME_EXIT_CODE%"=="0" (
    echo 🏁 Game finished successfully
) else (
    echo 🏁 Game finished with exit code: %GAME_EXIT_CODE%
    pause
)
goto :eof

:: Function to get project name from CMakeLists.txt
:get_project_name
set "project_name=SDL2Template"

if not exist "CMakeLists.txt" (
    set "%~1=!project_name!"
    goto :eof
)

:: Look for project line and extract name
for /f "usebackq tokens=*" %%a in (`findstr /r "^project(" CMakeLists.txt 2^>nul`) do (
    set "project_line=%%a"
    :: Remove 'project(' prefix and extract first token
    set "project_line=!project_line:project(=!"
    
    :: Extract project name (handle quotes and spaces)
    for /f "tokens=1 delims=, )" %%b in ("!project_line!") do (
        set "temp_name=%%b"
        :: Remove quotes if present
        set "temp_name=!temp_name:"=!"
        :: Trim leading/trailing spaces
        for /f "tokens=* delims= " %%c in ("!temp_name!") do (
            set "temp_name=%%c"
        )
        :: Validate we got a real name
        if not "!temp_name!"=="" if not "!temp_name!"=="project" (
            set "project_name=!temp_name!"
        )
    )
    goto :found_project
)

:found_project
set "%~1=!project_name!"
goto :eof