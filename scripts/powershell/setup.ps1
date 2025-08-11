# SDL2 Template Setup Script for Windows (PowerShell)
# This script handles first-time setup including vcpkg installation and initial build

param(
    [switch]$NoGitSetup
)

# Enable strict mode for better error handling
Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

Write-Host "🚀 Setting up SDL2 Template..." -ForegroundColor Green
Write-Host ""

# Get project root directory
$ProjectRoot = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
Set-Location $ProjectRoot

Write-Host "📁 Project root: $PWD" -ForegroundColor Cyan
Write-Host ""

# Check if vcpkg exists
$VcpkgPath = "C:\vcpkg"
$ToolchainFile = "$VcpkgPath\scripts\buildsystems\vcpkg.cmake"

if (-not (Test-Path $VcpkgPath)) {
    Write-Host "📦 vcpkg not found. Installing vcpkg..." -ForegroundColor Yellow
    Write-Host ""
    
    # Check if Git is available
    try {
        git --version | Out-Null
    }
    catch {
        Write-Host "❌ Git not found. Please install Git first:" -ForegroundColor Red
        Write-Host "   https://git-scm.com/downloads" -ForegroundColor White
        Read-Host "Press Enter to exit"
        exit 1
    }
    
    # Check if Visual Studio or Build Tools are available
    $vcVars = Get-Command "vcvars*.bat" -ErrorAction SilentlyContinue
    $clExe = Get-Command "cl.exe" -ErrorAction SilentlyContinue
    
    if (-not $vcVars -and -not $clExe) {
        Write-Host "❌ Visual Studio Build Tools not found." -ForegroundColor Red
        Write-Host "Please install one of the following:" -ForegroundColor White
        Write-Host "  - Visual Studio 2019/2022 with C++ workload" -ForegroundColor White
        Write-Host "  - Build Tools for Visual Studio 2019/2022" -ForegroundColor White
        Write-Host "  - Visual Studio Community (free)" -ForegroundColor White
        Write-Host ""
        Write-Host "Download from: https://visualstudio.microsoft.com/downloads/" -ForegroundColor Cyan
        Read-Host "Press Enter to exit"
        exit 1
    }
    
    Write-Host "⬇️  Cloning vcpkg to C:\vcpkg..." -ForegroundColor Blue
    try {
        git clone https://github.com/Microsoft/vcpkg.git $VcpkgPath
    }
    catch {
        Write-Host "❌ Failed to clone vcpkg: $_" -ForegroundColor Red
        Read-Host "Press Enter to exit"
        exit 1
    }
    
    Write-Host "🔧 Bootstrapping vcpkg..." -ForegroundColor Blue
    try {
        Push-Location $VcpkgPath
        .\bootstrap-vcpkg.bat
        Pop-Location
    }
    catch {
        Write-Host "❌ Failed to bootstrap vcpkg: $_" -ForegroundColor Red
        Pop-Location
        Read-Host "Press Enter to exit"
        exit 1
    }
    
    Write-Host ""
    Write-Host "💡 Consider adding vcpkg to your PATH:" -ForegroundColor Yellow
    Write-Host "   Add C:\vcpkg to your system PATH environment variable" -ForegroundColor White
    Write-Host ""
}
else {
    Write-Host "✅ vcpkg found at $VcpkgPath" -ForegroundColor Green
}

# Git Repository Setup
if (-not $NoGitSetup) {
    Write-Host ""
    Write-Host "🔗 Git Repository Setup" -ForegroundColor Cyan
    Write-Host ""
    Write-Host "Currently this project is connected to the SDL2 template repository." -ForegroundColor White
    Write-Host "You have two options:" -ForegroundColor White
    Write-Host ""
    Write-Host "  1. Keep connection to template (you can pull future template updates)" -ForegroundColor White
    Write-Host "  2. Create your own independent repository (recommended for new projects)" -ForegroundColor White
    Write-Host ""
    
    $repoChoice = Read-Host "Would you like to disconnect from the template and create your own repository? (Y/n)"
    
    if ($repoChoice -eq "" -or $repoChoice -match "^[Yy]") {
        Write-Host "🔄 Creating independent repository..." -ForegroundColor Blue
        
        # Remove the template's git history
        if (Test-Path ".git") {
            Remove-Item -Recurse -Force ".git"
        }
        
        try {
            # Initialize a new git repository
            git init
            
            # Add all files to the new repository
            git add .
            
            # Create initial commit
            git commit -m "Initial commit: SDL2 game template setup"
            
            Write-Host ""
            Write-Host "✅ Independent repository created!" -ForegroundColor Green
            Write-Host ""
            Write-Host "To connect to your own remote repository:" -ForegroundColor Yellow
            Write-Host "  1. Create a new repository on GitHub/GitLab/etc." -ForegroundColor White
            Write-Host "  2. Run: git remote add origin <your-repo-url>" -ForegroundColor White
            Write-Host "  3. Run: git push -u origin main" -ForegroundColor White
            Write-Host ""
        }
        catch {
            Write-Host "❌ Failed to create git repository: $_" -ForegroundColor Red
        }
    }
    else {
        Write-Host "ℹ️  Keeping connection to template repository." -ForegroundColor Blue
        Write-Host "   You can pull template updates with: git pull" -ForegroundColor White
        Write-Host ""
    }
}

# Function to validate project name
function Test-ProjectName {
    param([string]$Name)
    
    # Check if name is empty
    if ([string]::IsNullOrWhiteSpace($Name)) {
        Write-Host "❌ Project name cannot be empty" -ForegroundColor Red
        return $false
    }
    
    # Check if name starts with a letter
    if ($Name -notmatch "^[a-zA-Z]") {
        Write-Host "❌ Project name must start with a letter" -ForegroundColor Red
        return $false
    }
    
    # Check if name contains only valid characters
    if ($Name -notmatch "^[a-zA-Z][a-zA-Z0-9_-]*$") {
        Write-Host "❌ Project name can only contain letters, numbers, hyphens, and underscores" -ForegroundColor Red
        return $false
    }
    
    # Check for reserved names
    $reservedNames = @("make", "cmake", "build", "run", "clean", "install", "test", "package")
    if ($reservedNames -contains $Name.ToLower()) {
        Write-Host "❌ '$Name' is a reserved name and cannot be used" -ForegroundColor Red
        return $false
    }
    
    return $true
}

function Show-ValidationHelp {
    Write-Host ""
    Write-Host "Please enter a valid project name:" -ForegroundColor Yellow
    Write-Host "  - Must start with a letter" -ForegroundColor White
    Write-Host "  - Can contain letters, numbers, hyphens, and underscores" -ForegroundColor White
    Write-Host "  - Cannot be a reserved name (make, cmake, build, run, etc.)" -ForegroundColor White
    Write-Host ""
}

# Project Configuration with validation
Write-Host ""
Write-Host "🏷️  Project Configuration" -ForegroundColor Cyan
Write-Host ""
Write-Host "Current project name: SDL2Template" -ForegroundColor White

do {
    $projectName = Read-Host "Enter your project name (or press Enter to keep 'SDL2Template')"
    
    # Trim whitespace
    $projectName = $projectName.Trim()
    
    # If empty, keep default
    if ([string]::IsNullOrEmpty($projectName)) {
        $projectName = "SDL2Template"
        $validName = $true
    }
    else {
        $validName = Test-ProjectName $projectName
        if (-not $validName) {
            Show-ValidationHelp
        }
    }
} while (-not $validName)

if ($projectName -ne "SDL2Template") {
    Write-Host "🔧 Renaming project to: $projectName" -ForegroundColor Blue
    
    try {
        # Update CMakeLists.txt with LANGUAGES fix
        if (Test-Path "CMakeLists.txt") {
            $content = Get-Content "CMakeLists.txt" -Raw
            $content = $content -replace "project\(SDL2Template", "project(`"$projectName`""
            $content = $content -replace "project\(`"$projectName`" VERSION 1\.0\.0\)", "project(`"$projectName`" VERSION 1.0.0 LANGUAGES CXX)"
            Set-Content "CMakeLists.txt" $content
            Write-Host "  ✅ Updated CMakeLists.txt" -ForegroundColor Green
        }
        
        # Update vcpkg.json
        if (Test-Path "vcpkg.json") {
            # Convert to lowercase and replace spaces/special chars with hyphens for package name
            $packageName = $projectName.ToLower() -replace "[^a-z0-9]", "-" -replace "-+", "-" -replace "^-|-$", ""
            $content = Get-Content "vcpkg.json" -Raw
            $content = $content -replace '"name": "sdl2-template"', "`"name`": `"$packageName`""
            Set-Content "vcpkg.json" $content
            Write-Host "  ✅ Updated vcpkg.json (package name: $packageName)" -ForegroundColor Green
        }
        
        # Update README.md
        if (Test-Path "README.md") {
            $content = Get-Content "README.md" -Raw
            $content = $content -replace "# SDL2 Game Development Template", "# $projectName"
            $content = $content -replace "SDL2 Template", $projectName
            Set-Content "README.md" $content
            Write-Host "  ✅ Updated README.md" -ForegroundColor Green
        }
        
        # Update window title in settings.json
        if (Test-Path "assets\settings.json") {
            $content = Get-Content "assets\settings.json" -Raw
            $content = $content -replace '"title": "SDL2 Game"', "`"title`": `"$projectName`""
            Set-Content "assets\settings.json" $content
            Write-Host "  ✅ Updated window title in assets\settings.json" -ForegroundColor Green
        }
        
        Write-Host ""
        Write-Host "✅ Project renamed successfully!" -ForegroundColor Green
        Write-Host "   Executable will be named: $projectName.exe" -ForegroundColor White
        Write-Host ""
    }
    catch {
        Write-Host "❌ Failed to rename project: $_" -ForegroundColor Red
        Write-Host "   Continuing with original name..." -ForegroundColor White
        Write-Host ""
        $projectName = "SDL2Template"
    }
}
else {
    Write-Host "ℹ️  Keeping project name as 'SDL2Template'" -ForegroundColor Blue
    Write-Host ""
    
    # Still fix the CMakeLists.txt LANGUAGES issue
    try {
        if (Test-Path "CMakeLists.txt") {
            $content = Get-Content "CMakeLists.txt" -Raw
            $content = $content -replace "project\(SDL2Template VERSION 1\.0\.0\)", "project(SDL2Template VERSION 1.0.0 LANGUAGES CXX)"
            Set-Content "CMakeLists.txt" $content
            Write-Host "  ✅ Fixed CMakeLists.txt LANGUAGES issue" -ForegroundColor Green
        }
    }
    catch {
        Write-Host "⚠️  Could not fix CMakeLists.txt: $_" -ForegroundColor Yellow
    }
}

# Install dependencies
Write-Host "📚 Installing project dependencies..." -ForegroundColor Blue
try {
    & "$VcpkgPath\vcpkg.exe" install
}
catch {
    Write-Host "❌ Failed to install dependencies: $_" -ForegroundColor Red
    Read-Host "Press Enter to exit"
    exit 1
}

# Initial build
Write-Host "🔨 Building project..." -ForegroundColor Blue
if (-not (Test-Path "build")) {
    New-Item -ItemType Directory -Name "build" | Out-Null
}

try {
    Push-Location "build"
    
    # Configure project
    cmake .. -DCMAKE_TOOLCHAIN_FILE="$ToolchainFile" -A x64
    
    # Build project
    $cores = (Get-CimInstance -ClassName Win32_ComputerSystem).NumberOfLogicalProcessors
    cmake --build . --config Release --parallel $cores
    
    Pop-Location
}
catch {
    Write-Host "❌ Build failed: $_" -ForegroundColor Red
    Pop-Location
    Read-Host "Press Enter to exit"
    exit 1
}

Write-Host ""
Write-Host "🎉 Setup complete!" -ForegroundColor Green
Write-Host ""
Write-Host "Quick start commands:" -ForegroundColor Cyan
Write-Host "   .\scripts\run.ps1       # Build and run the game" -ForegroundColor White
Write-Host "   .\scripts\build.ps1     # Build the project" -ForegroundColor White
Write-Host "   .\scripts\clean.ps1     # Clean build files" -ForegroundColor White
Write-Host "   .\scripts\install.ps1   # Install/update dependencies" -ForegroundColor White
Write-Host ""
Write-Host "🎮 To run your game now:" -ForegroundColor Cyan
if ($projectName -match " ") {
    Write-Host "   `".\build\Release\$projectName.exe`"" -ForegroundColor White
}
else {
    Write-Host "   .\build\Release\$projectName.exe" -ForegroundColor White
}
Write-Host ""

if (-not $NoGitSetup) {
    Read-Host "Press Enter to continue"
}