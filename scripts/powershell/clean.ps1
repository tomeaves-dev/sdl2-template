# SDL2 Template Clean Script for Windows (PowerShell)
# Removes build artifacts and temporary files

param(
    [switch]$Force
)

# Enable strict mode for better error handling
Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

Write-Host "🧹 Cleaning SDL2 Template..." -ForegroundColor Green

# Get project root directory
$ProjectRoot = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
Set-Location $ProjectRoot

# Remove build directory
if (Test-Path "build") {
    Write-Host "🗑️  Removing build directory..." -ForegroundColor Yellow
    try {
        Remove-Item -Recurse -Force "build"
        Write-Host "✅ Build directory removed" -ForegroundColor Green
    }
    catch {
        Write-Host "❌ Failed to remove build directory: $_" -ForegroundColor Red
    }
}
else {
    Write-Host "ℹ️  Build directory doesn't exist" -ForegroundColor Blue
}

# Remove vcpkg installed packages (ask user unless -Force)
if (Test-Path "vcpkg_installed") {
    $removeVcpkg = $Force
    
    if (-not $Force) {
        Write-Host ""
        $response = Read-Host "🤔 Remove vcpkg_installed directory? (y/N)"
        $removeVcpkg = ($response -match "^[Yy]")
    }
    
    if ($removeVcpkg) {
        Write-Host "🗑️  Removing vcpkg_installed directory..." -ForegroundColor Yellow
        try {
            Remove-Item -Recurse -Force "vcpkg_installed"
            Write-Host "✅ vcpkg_installed directory removed" -ForegroundColor Green
        }
        catch {
            Write-Host "❌ Failed to remove vcpkg_installed directory: $_" -ForegroundColor Red
        }
    }
    else {
        Write-Host "ℹ️  Keeping vcpkg_installed directory" -ForegroundColor Blue
    }
}

# Clean other common temporary files
Write-Host "🧽 Cleaning temporary files..." -ForegroundColor Blue

$tempPatterns = @(
    "*.tmp",
    "*.log", 
    "Thumbs.db",
    "*.user",
    "*.suo"
)

foreach ($pattern in $tempPatterns) {
    try {
        Get-ChildItem -Path . -Name $pattern -ErrorAction SilentlyContinue | 
        ForEach-Object { 
            Remove-Item $_ -Force
            Write-Host "  Removed: $_" -ForegroundColor DarkGray
        }
    }
    catch {
        # Ignore errors for individual file removal
    }
}

# Remove IDE temporary directories
$tempDirs = @(
    ".vs",
    "x64",
    "Win32",
    "cmake-build-debug",
    "cmake-build-release"
)

foreach ($dir in $tempDirs) {
    if (Test-Path $dir) {
        try {
            Remove-Item -Recurse -Force $dir
            Write-Host "  Removed directory: $dir" -ForegroundColor DarkGray
        }
        catch {
            Write-Host "  Failed to remove: $dir" -ForegroundColor DarkRed
        }
    }
}

Write-Host ""
Write-Host "✨ Cleaning complete!" -ForegroundColor Green
Write-Host "🔄 To rebuild: .\scripts\build.ps1" -ForegroundColor Cyan
Write-Host ""

if (-not $Force) {
    Read-Host "Press Enter to continue"
}