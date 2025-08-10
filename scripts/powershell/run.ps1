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

# Check if executable exists (try both Debug and Release)
$ExecutableRelease = "build\Release\SDL2Template.exe"
$ExecutableDebug = "build\Debug\SDL2Template.exe"
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

# Run the game
try {
    Push-Location "build"
    
    if ($BuildConfig -eq "Release") {
        $gameProcess = Start-Process -FilePath "Release\SDL2Template.exe" -ArgumentList $GameArgs -Wait -PassThru -NoNewWindow
    }
    else {
        $gameProcess = Start-Process -FilePath "Debug\SDL2Template.exe" -ArgumentList $GameArgs -Wait -PassThru -NoNewWindow
    }
    
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
Write-Host "🏁 Game finished (exit code: $exitCode)" -ForegroundColor Cyan

if ($exitCode -ne 0) {
    Write-Host "⚠️  Game exited with error code $exitCode" -ForegroundColor Yellow
    Read-Host "Press Enter to continue"
}