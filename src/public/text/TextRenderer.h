#pragma once

#include "FontManager.h"
#include "Text.h"
#include <glm/glm.hpp>
#include <memory>
#include <unordered_map>
#include <string>
#include <vector>

// Forward declarations
namespace rendering {
    class Texture;
    class Renderer;
}

namespace core {
    struct Transform;
}

namespace text {

    /**
     * Text Renderer
     * 
     * Handles rendering of text objects with support for both cached and immediate modes.
     * Integrates with FontManager for font access and supports all text effects.
     * 
     * Features:
     * - Cached rendering (text to texture, then draw texture)
     * - Immediate rendering (render text directly each frame)
     * - Text effects (outline, shadow)
     * - Alignment and word wrapping
     * - Memory management for cached textures
     */
    class TextRenderer {
    public:
        TextRenderer();
        ~TextRenderer();

        // System lifecycle
        bool Initialize(FontManager* fontManager, rendering::Renderer* renderer);
        void Shutdown();
        void Update(); // Call each frame to manage cache
        bool IsInitialized() const { return m_initialized; }

        // Text rendering
        void DrawText(const Text& text, const glm::vec2& position);
        void DrawText(const Text& text, const core::Transform& transform);

        // Advanced rendering with custom properties
        void DrawText(const Text& text, const glm::vec2& position, float rotation, const glm::vec2& scale = glm::vec2(1.0f));

        // Text measurement (useful for layout)
        glm::ivec2 MeasureText(const Text& text);
        glm::ivec2 MeasureText(const std::string& content, const std::string& fontName);
        
        // Cache management
        void ClearCache();
        void ClearCacheForFont(const std::string& fontName);
        void SetMaxCacheSize(size_t maxSizeMB);
        size_t GetCacheMemoryUsage() const;

        // Configuration
        void LoadConfigFromSettings();
        
    private:
        struct CachedTextData {
            std::shared_ptr<rendering::Texture> texture;
            glm::ivec2 size;
            size_t memorySize; // In bytes
            mutable size_t lastUsedFrame; // For LRU eviction
            
            // Text properties hash for cache key validation
            std::string textHash;
        };

        // Internal rendering methods
        void RenderImmediate(const Text& text, const glm::vec2& position, float rotation, const glm::vec2& scale);
        std::shared_ptr<rendering::Texture> RenderToTexture(const Text& text);
        void RenderCached(const Text& text, const glm::vec2& position, float rotation, const glm::vec2& scale);

        // Text processing
        std::vector<std::string> WrapText(const std::string& content, const std::string& fontName, float maxWidth);
        std::string GenerateTextHash(const Text& text);
        glm::ivec2 CalculateTextSize(const Text& text);

        // Cache management
        void EvictOldCacheEntries();
        void RemoveCacheEntry(const std::string& key);

        // Effect rendering helpers
        void RenderTextWithEffects(const Text& text, const glm::vec2& position, float rotation, const glm::vec2& scale);
        SDL_Surface* CreateTextSurface(const Text& text);
        SDL_Surface* CreateOutlineTextSurface(const Text& text);
        SDL_Surface* ApplyTextEffects(SDL_Surface* textSurface, const Text& text);

        // Alignment helpers
        glm::vec2 CalculateAlignmentOffset(const Text& text, const glm::ivec2& textSize);

        FontManager* m_fontManager = nullptr;
        rendering::Renderer* m_renderer = nullptr;
        bool m_initialized = false;
        
        // Cache system
        std::unordered_map<std::string, CachedTextData> m_textCache;
        size_t m_maxCacheSize = 64 * 1024 * 1024; // 64MB default
        size_t m_currentCacheSize = 0;
        mutable size_t m_currentFrame = 0;
        
        // Configuration (loaded from settings)
        bool m_enableCache = true;
        bool m_enableKerning = true;
        float m_kerningAdjustment = 0.0f;
        int m_hinting = 0; // TTF hinting mode
    };

} // namespace text