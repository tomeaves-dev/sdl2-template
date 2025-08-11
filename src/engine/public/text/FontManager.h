#pragma once

#include <SDL_ttf.h>
#include <string>
#include <unordered_map>
#include <memory>

namespace text {

    /**
     * Font Manager
     * 
     * Manages loading, caching, and cleanup of TTF fonts.
     * Supports multiple sizes of the same font file.
     * 
     * Usage:
     *   fontManager->LoadFont("arial.ttf", "arial", 24);
     *   fontManager->LoadFont("arial.ttf", "arial_large", 48);
     *   auto* font = fontManager->GetFont("arial");
     */
    class FontManager {
    public:
        FontManager();
        ~FontManager();

        // System lifecycle
        bool Initialize();
        void Shutdown();
        bool IsInitialized() const { return m_initialized; }

        // Font loading/management
        bool LoadFont(const std::string& filepath, const std::string& name, int size);
        void UnloadFont(const std::string& name);
        void UnloadAll();

        // Font access
        TTF_Font* GetFont(const std::string& name) const;
        bool HasFont(const std::string& name) const;

        // Font information
        int GetFontSize(const std::string& name) const;
        std::string GetFontFilepath(const std::string& name) const;
        
        // Utility functions
        void GetTextSize(const std::string& text, const std::string& fontName, int& width, int& height) const;
        int GetFontHeight(const std::string& fontName) const;
        int GetFontAscent(const std::string& fontName) const;
        int GetFontDescent(const std::string& fontName) const;
        int GetFontLineSkip(const std::string& fontName) const;

        // Default font management
        void SetDefaultFont(const std::string& fontName);
        const std::string& GetDefaultFont() const { return m_defaultFont; }
        TTF_Font* GetDefaultFontHandle() const;

    private:
        struct FontData {
            TTF_Font* font = nullptr;
            std::string filepath;
            int size = 0;
        };

        bool m_initialized = false;
        std::unordered_map<std::string, FontData> m_fonts;
        std::string m_defaultFont;
    };

} // namespace text