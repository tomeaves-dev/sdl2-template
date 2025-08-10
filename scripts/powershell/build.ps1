# SDL2 Template Build Script for Windows (PowerShell)
# Builds the project using CMake and Visual Studio

param(
    [string]$BuildType = "Release"
)

# Enable strict mode for better error handling
Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

Write-Host "🔨 Building SDL2 Template..." -ForegroundColor Green

# Get project root directory
$ProjectRoot = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
Set-Location $ProjectRoot

# Check if vcpkg exists
$VcpkgPath = "C:\vcpkg"
if (-not (Test-Path $VcpkgPath)) {
    Write-Host "❌ vcpkg not found at $VcpkgPath" -ForegroundColor Red
    Write-Host "Please run setup first: .\scripts\setup.ps1" -ForegroundColor White
    Read-Host "Press Enter to exit"
    exit 1
}

$ToolchainFile = "$VcpkgPath\scripts\buildsystems\vcpkg.cmake"

# Create build directory if it doesn't exist
if (-not (Test-Path "build")) {
    New-Item -ItemType Directory -Name "build" | Out-Null
}

Write-Host "📋 Build type: $BuildType" -ForegroundColor Cyan

try {
    Push-Location "build"
    
    # Configure project
    Write-Host "⚙️  Configuring project..." -ForegroundColor Blue
    cmake .. -DCMAKE_TOOLCHAIN_FILE="$ToolchainFile" -A x64
    
    # Build project
    Write-Host "🔧 Compiling..." -ForegroundColor Blue
    $cores = (Get-CimInstance -ClassName Win32_ComputerSystem).NumberOfLogicalProcessors
    cmake --build . --config $BuildType --parallel $cores
    
    Pop-Location
}
catch {
    Write-Host "❌ Build failed: $_" -ForegroundColor Red
    Pop-Location
    Read-Host "Press Enter to exit"
    exit 1
}

Write-Host ""
Write-Host "✅ Build complete!" -ForegroundColor Green
Write-Host "🎮 Run with: .\scripts\run.ps1" -ForegroundColor Cyan