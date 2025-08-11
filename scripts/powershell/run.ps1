# SDL2 Template Run Script for Windows (PowerShell)
# Builds (if needed) and runs the game

param(
    [string[]]$GameArgs = @()
)

# Enable strict mode for better error handling
Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

Write-Host "🎮 Running SDL2 Template..." -ForegroundColor Green

# Get project root directory
$ProjectRoot = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
Set-Location $ProjectRoot

# Function to extract project name from CMakeLists.txt
function Get-ProjectName {
    if (-not (Test-Path "CMakeLists.txt")) {
        return "SDL2Template"
    }
    
    try {
        $cmakeContent = Get-Content "CMakeLists.txt"
        $projectLine = $cmakeContent | Where-Object { $_ -match "^project\(" } | Select-Object -First 1
        
        if (-not $projectLine) {
            return "SDL2Template"
        }
        
        # Extract project name - handle quotes and spaces
        if ($projectLine -match "^project\(\s*[`"']?([^`"',)\s]+)") {
            $projectName = $matches[1].Trim()
            if ($projectName -and $projectName -ne "project") {
                return $projectName
            }
        }
    }
    catch {
        # If parsing fails, use default
    }
    
    return "SDL2Template"
}

# Get the actual project name
$ExecutableName = Get-ProjectName

Write-Host "🎯 Looking for executable: $ExecutableName" -ForegroundColor Cyan

# Check if executable exists (try both Debug and Release)
$ExecutableRelease = "build\Release\$ExecutableName.exe"
$ExecutableDebug = "build\Debug\$ExecutableName.exe"
$Executable = $null
$BuildConfig = $null

if (Test-Path $ExecutableRelease) {
    $Executable = $ExecutableRelease
    $BuildConfig = "Release"
}
elseif (Test-Path $ExecutableDebug) {
    $Executable = $ExecutableDebug
    $BuildConfig = "Debug"
}

if (-not $Executable) {
    Write-Host "📦 Executable not found. Building first..." -ForegroundColor Yellow
    try {
        & ".\scripts\powershell\build.ps1"
        
        # Check again after build
        if (Test-Path $ExecutableRelease) {
            $Executable = $ExecutableRelease
            $BuildConfig = "Release"
        }
        elseif (Test-Path $ExecutableDebug) {
            $Executable = $ExecutableDebug
            $BuildConfig = "Debug"
        }
        else {
            Write-Host "❌ Failed to build executable" -ForegroundColor Red
            Read-Host "Press Enter to exit"
            exit 1
        }
    }
    catch {
        Write-Host "❌ Build failed: $_" -ForegroundColor Red
        Read-Host "Press Enter to exit"
        exit 1
    }
}

# Simple dependency check - if source files are newer, rebuild
if (Test-Path "src") {
    try {
        $newestSource = Get-ChildItem -Path "src" -Recurse -Include "*.cpp", "*.h", "*.hpp" | 
                       Sort-Object LastWriteTime -Descending | 
                       Select-Object -First 1
        
        $executableTime = (Get-Item $Executable).LastWriteTime
        
        if ($newestSource -and $newestSource.LastWriteTime -gt $executableTime) {
            Write-Host "🔄 Source files updated. Rebuilding..." -ForegroundColor Yellow
            & ".\scripts\powershell\build.ps1"
        }
    }
    catch {
        # Ignore errors in dependency checking - not critical
    }
}

Write-Host "🚀 Starting game ($BuildConfig build)..." -ForegroundColor Blue
Write-Host ""

# Run the game - handle names with spaces properly
try {
    Push-Location "build"
    
    if ($BuildConfig -eq "Release") {
        $gameExe = "Release\$ExecutableName.exe"
    }
    else {
        $gameExe = "Debug\$ExecutableName.exe"
    }
    
    # Run the executable
    $gameProcess = Start-Process -FilePath $gameExe -ArgumentList $GameArgs -Wait -PassThru -NoNewWindow
    
    $exitCode = $gameProcess.ExitCode
    Pop-Location
}
catch {
    Write-Host "❌ Failed to run game: $_" -ForegroundColor Red
    Pop-Location
    Read-Host "Press Enter to exit"
    exit 1
}

Write-Host ""
if ($exitCode -eq 0) {
    Write-Host "🏁 Game finished successfully" -ForegroundColor Green
}
else {
    Write-Host "🏁 Game finished with exit code: $exitCode" -ForegroundColor Yellow
    Read-Host "Press Enter to continue"
}