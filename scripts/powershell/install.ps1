# SDL2 Template Install Script for Windows (PowerShell)
# Installs or updates project dependencies via vcpkg

# Enable strict mode for better error handling
Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

Write-Host "📦 Installing/updating dependencies..." -ForegroundColor Green

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

# Check if vcpkg.json exists
if (-not (Test-Path "vcpkg.json")) {
    Write-Host "❌ vcpkg.json not found in project root" -ForegroundColor Red
    Read-Host "Press Enter to exit"
    exit 1
}

Write-Host "📋 Installing dependencies from vcpkg.json..." -ForegroundColor Blue

# Install/update dependencies
try {
    $vcpkgExe = "$VcpkgPath\vcpkg.exe"
    
    # Show some progress
    Write-Host "Running: $vcpkgExe install" -ForegroundColor DarkGray
    
    $process = Start-Process -FilePath $vcpkgExe -ArgumentList "install" -Wait -PassThru -NoNewWindow
    
    if ($process.ExitCode -ne 0) {
        throw "vcpkg install failed with exit code $($process.ExitCode)"
    }
}
catch {
    Write-Host "❌ Failed to install dependencies: $_" -ForegroundColor Red
    Read-Host "Press Enter to exit"
    exit 1
}

Write-Host ""
Write-Host "✅ Dependencies installed successfully!" -ForegroundColor Green
Write-Host "🔨 Rebuild recommended: .\scripts\build.ps1" -ForegroundColor Cyan
Write-Host ""

Read-Host "Press Enter to continue"