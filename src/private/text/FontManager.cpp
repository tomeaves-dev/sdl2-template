#include "public/text/FontManager.h"
#include <spdlog/spdlog.h>
#include <filesystem>

namespace text {

FontManager::FontManager() = default;

FontManager::~FontManager() {
    Shutdown();
}

bool FontManager::Initialize() {
    spdlog::info("Initializing font manager...");
    
    // Initialize SDL_ttf
    if (TTF_Init() == -1) {
        spdlog::error("Failed to initialize SDL_ttf: {}", TTF_GetError());
        return false;
    }
    
    m_initialized = true;
    spdlog::info("Font manager initialized successfully");
    
    return true;
}

void FontManager::Shutdown() {
    if (m_initialized) {
        spdlog::info("Shutting down font manager...");
        
        // Unload all fonts
        UnloadAll();
        
        // Shutdown SDL_ttf
        TTF_Quit();
        
        m_initialized = false;
        spdlog::info("Font manager shutdown complete");
    }
}

bool FontManager::LoadFont(const std::string& filepath, const std::string& name, int size) {
    if (!m_initialized) {
        spdlog::warn("Font manager not initialized, cannot load font: {}", name);
        return false;
    }
    
    // Check if font already loaded
    if (HasFont(name)) {
        spdlog::warn("Font '{}' already loaded", name);
        return true;
    }
    
    // Validate size
    if (size <= 0) {
        spdlog::error("Invalid font size {} for font '{}'", size, name);
        return false;
    }
    
    // Try to load font from multiple locations
    TTF_Font* font = nullptr;
    std::string actualPath;
    
    // First try: assets/fonts/ directory
    std::string assetsPath = "assets/fonts/" + filepath;
    if (std::filesystem::exists(assetsPath)) {
        font = TTF_OpenFont(assetsPath.c_str(), size);
        actualPath = assetsPath;
    }
    
    // Second try: system font directories (macOS/Linux/Windows)
    if (!font) {
        std::vector<std::string> systemPaths;
        
        #ifdef __APPLE__
        systemPaths = {
            "/System/Library/Fonts/" + filepath,
            "/Library/Fonts/" + filepath,
            "~/Library/Fonts/" + filepath
        };
        #elif defined(__linux__)
        systemPaths = {
            "/usr/share/fonts/truetype/dejavu/" + filepath,
            "/usr/share/fonts/TTF/" + filepath,
            "/usr/share/fonts/" + filepath,
            "~/.fonts/" + filepath
        };
        #elif defined(_WIN32)
        systemPaths = {
            "C:/Windows/Fonts/" + filepath
        };
        #endif
        
        for (const auto& path : systemPaths) {
            if (std::filesystem::exists(path)) {
                font = TTF_OpenFont(path.c_str(), size);
                if (font) {
                    actualPath = path;
                    break;
                }
            }
        }
    }
    
    // Third try: common system font names as fallbacks
    if (!font) {
        std::vector<std::string> fallbackFonts;
        
        #ifdef __APPLE__
        fallbackFonts = {
            "/System/Library/Fonts/Arial.ttf",
            "/System/Library/Fonts/Helvetica.ttc",
            "/System/Library/Fonts/Times.ttc"
        };
        #elif defined(__linux__)
        fallbackFonts = {
            "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
            "/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf",
            "/usr/share/fonts/TTF/arial.ttf"
        };
        #elif defined(_WIN32)
        fallbackFonts = {
            "C:/Windows/Fonts/arial.ttf",
            "C:/Windows/Fonts/calibri.ttf",
            "C:/Windows/Fonts/tahoma.ttf"
        };
        #endif
        
        for (const auto& fallback : fallbackFonts) {
            if (std::filesystem::exists(fallback)) {
                font = TTF_OpenFont(fallback.c_str(), size);
                if (font) {
                    actualPath = fallback;
                    spdlog::debug("Using fallback font for '{}'", name);
                    break;
                }
            }
        }
    }
    
    // Final fallback: Let SDL_ttf try to find any system font
    if (!font) {
        // Try some generic names that SDL_ttf might resolve
        std::vector<std::string> genericNames = {"Arial", "Helvetica", "Times", "sans-serif"};
        for (const auto& genericName : genericNames) {
            font = TTF_OpenFont(genericName.c_str(), size);
            if (font) {
                actualPath = genericName + " (system resolved)";
                spdlog::debug("Using system-resolved font for '{}'", name);
                break;
            }
        }
    }
    
    if (!font) {
        spdlog::error("Failed to load font '{}' from '{}' or any fallback: {}", name, filepath, TTF_GetError());
        return false;
    }
    
    // Store font data
    FontData fontData;
    fontData.font = font;
    fontData.filepath = actualPath;
    fontData.size = size;
    
    m_fonts[name] = fontData;
    
    spdlog::info("Loaded font '{}' at size {}", name, size);
    
    // Set as default if no default is set
    if (m_defaultFont.empty()) {
        SetDefaultFont(name);
    }
    
    return true;
}

void FontManager::UnloadFont(const std::string& name) {
    auto it = m_fonts.find(name);
    if (it != m_fonts.end()) {
        if (it->second.font) {
            TTF_CloseFont(it->second.font);
        }
        
        // Clear default font if this was it
        if (m_defaultFont == name) {
            m_defaultFont.clear();
        }
        
        m_fonts.erase(it);
        spdlog::info("Unloaded font '{}'", name);
    }
}

void FontManager::UnloadAll() {
    for (auto& [name, fontData] : m_fonts) {
        if (fontData.font) {
            TTF_CloseFont(fontData.font);
        }
    }
    
    m_fonts.clear();
    m_defaultFont.clear();
    
    if (!m_fonts.empty()) {
        spdlog::info("Unloaded {} fonts", m_fonts.size());
    }
}

TTF_Font* FontManager::GetFont(const std::string& name) const {
    auto it = m_fonts.find(name);
    if (it != m_fonts.end()) {
        return it->second.font;
    }
    
    // If font not found, try to return default font
    if (!m_defaultFont.empty() && name != m_defaultFont) {
        spdlog::warn("Font '{}' not found, using default font '{}'", name, m_defaultFont);
        return GetFont(m_defaultFont);
    }
    
    spdlog::warn("Font '{}' not found and no default font available", name);
    return nullptr;
}

bool FontManager::HasFont(const std::string& name) const {
    return m_fonts.find(name) != m_fonts.end();
}

int FontManager::GetFontSize(const std::string& name) const {
    auto it = m_fonts.find(name);
    if (it != m_fonts.end()) {
        return it->second.size;
    }
    return 0;
}

std::string FontManager::GetFontFilepath(const std::string& name) const {
    auto it = m_fonts.find(name);
    if (it != m_fonts.end()) {
        return it->second.filepath;
    }
    return "";
}

void FontManager::GetTextSize(const std::string& text, const std::string& fontName, int& width, int& height) const {
    TTF_Font* font = GetFont(fontName);
    if (!font) {
        width = height = 0;
        return;
    }
    
    if (TTF_SizeUTF8(font, text.c_str(), &width, &height) != 0) {
        spdlog::warn("Failed to get text size for '{}': {}", text, TTF_GetError());
        width = height = 0;
    }
}

int FontManager::GetFontHeight(const std::string& fontName) const {
    TTF_Font* font = GetFont(fontName);
    if (!font) return 0;
    
    return TTF_FontHeight(font);
}

int FontManager::GetFontAscent(const std::string& fontName) const {
    TTF_Font* font = GetFont(fontName);
    if (!font) return 0;
    
    return TTF_FontAscent(font);
}

int FontManager::GetFontDescent(const std::string& fontName) const {
    TTF_Font* font = GetFont(fontName);
    if (!font) return 0;
    
    return TTF_FontDescent(font);
}

int FontManager::GetFontLineSkip(const std::string& fontName) const {
    TTF_Font* font = GetFont(fontName);
    if (!font) return 0;
    
    return TTF_FontLineSkip(font);
}

void FontManager::SetDefaultFont(const std::string& fontName) {
    if (HasFont(fontName)) {
        m_defaultFont = fontName;
        spdlog::info("Set default font to '{}'", fontName);
    } else {
        spdlog::warn("Cannot set default font to '{}' - font not loaded", fontName);
    }
}

TTF_Font* FontManager::GetDefaultFontHandle() const {
    if (m_defaultFont.empty()) {
        spdlog::warn("No default font set");
        return nullptr;
    }
    
    return GetFont(m_defaultFont);
}

} // namespace text