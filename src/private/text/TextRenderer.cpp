#include "public/text/TextRenderer.h"
#include "public/rendering/Texture.h"
#include "public/rendering/Renderer.h"
#include "public/rendering/Sprite.h"
#include "public/core/Transform.h"
#include "public/utils/Config.h"
#include <spdlog/spdlog.h>
#include <SDL.h>
#include <algorithm>
#include <sstream>
#include <iomanip>

namespace text {

TextRenderer::TextRenderer() = default;

TextRenderer::~TextRenderer() {
    Shutdown();
}

bool TextRenderer::Initialize(FontManager* fontManager, rendering::Renderer* renderer) {
    if (!fontManager) {
        spdlog::error("FontManager is required for TextRenderer initialization");
        return false;
    }
    
    if (!renderer) {
        spdlog::error("Renderer is required for TextRenderer initialization");
        return false;
    }
    
    if (!fontManager->IsInitialized()) {
        spdlog::error("FontManager must be initialized before TextRenderer");
        return false;
    }
    
    spdlog::info("Initializing text renderer...");
    
    m_fontManager = fontManager;
    m_renderer = renderer;
    
    // Load configuration
    LoadConfigFromSettings();
    
    m_initialized = true;
    spdlog::info("Text renderer initialized successfully");
    
    return true;
}

void TextRenderer::Shutdown() {
    if (m_initialized) {
        spdlog::info("Shutting down text renderer...");
        
        // Clear cache
        ClearCache();
        
        m_fontManager = nullptr;
        m_renderer = nullptr;
        m_initialized = false;
        
        spdlog::info("Text renderer shutdown complete");
    }
}

void TextRenderer::Update() {
    if (!m_initialized) return;
    
    m_currentFrame++;
    
    // Periodically clean up old cache entries (every 60 frames)
    if (m_currentFrame % 60 == 0) {
        EvictOldCacheEntries();
    }
}

void TextRenderer::DrawText(const Text& text, const glm::vec2& position) {
    DrawText(text, position, 0.0f, glm::vec2(1.0f));
}

void TextRenderer::DrawText(const Text& text, const core::Transform& transform) {
    DrawText(text, transform.GetPosition(), transform.GetRotation(), transform.GetScale());
}

void TextRenderer::DrawText(const Text& text, const glm::vec2& position, float rotation, const glm::vec2& scale) {
    if (!m_initialized || text.IsEmpty()) {
        return;
    }
    
    // Choose rendering method based on text render mode and caching settings
    if (text.GetRenderMode() == TextRenderMode::Cached && m_enableCache) {
        RenderCached(text, position, rotation, scale);
    } else {
        RenderImmediate(text, position, rotation, scale);
    }
}

glm::ivec2 TextRenderer::MeasureText(const Text& text) {
    if (!m_initialized || text.IsEmpty()) {
        return {0, 0};
    }
    
    return CalculateTextSize(text);
}

glm::ivec2 TextRenderer::MeasureText(const std::string& content, const std::string& fontName) {
    if (!m_initialized || content.empty()) {
        return {0, 0};
    }
    
    int width, height;
    m_fontManager->GetTextSize(content, fontName, width, height);
    return {width, height};
}

void TextRenderer::ClearCache() {
    m_textCache.clear();
    m_currentCacheSize = 0;
    spdlog::debug("Text cache cleared");
}

void TextRenderer::ClearCacheForFont(const std::string& fontName) {
    auto it = m_textCache.begin();
    while (it != m_textCache.end()) {
        // Check if this cache entry uses the specified font
        // This is a simplified check - in a more robust system you'd store font info
        if (it->first.find(fontName) != std::string::npos) {
            m_currentCacheSize -= it->second.memorySize;
            it = m_textCache.erase(it);
        } else {
            ++it;
        }
    }
    spdlog::debug("Cleared cache entries for font '{}'", fontName);
}

void TextRenderer::SetMaxCacheSize(size_t maxSizeMB) {
    m_maxCacheSize = maxSizeMB * 1024 * 1024;
    EvictOldCacheEntries();
}

size_t TextRenderer::GetCacheMemoryUsage() const {
    return m_currentCacheSize;
}

void TextRenderer::LoadConfigFromSettings() {
    auto config = utils::Config::Instance();
    
    m_maxCacheSize = config->GetInt("text.cache_size_mb", 64) * 1024 * 1024;
    m_enableKerning = config->GetBool("text.kerning.enabled", true);
    m_kerningAdjustment = config->GetFloat("text.kerning.adjustment", 0.0f);
    
    // Map hinting string to TTF constant
    std::string hinting = config->GetString("text.hinting", "normal");
    if (hinting == "none") m_hinting = TTF_HINTING_NONE;
    else if (hinting == "light") m_hinting = TTF_HINTING_LIGHT;
    else if (hinting == "mono") m_hinting = TTF_HINTING_MONO;
    else m_hinting = TTF_HINTING_NORMAL;
    
    spdlog::debug("Text renderer config loaded - Cache: {}MB, Kerning: {}, Hinting: {}", 
                 m_maxCacheSize / (1024 * 1024), m_enableKerning, hinting);
}

void TextRenderer::RenderImmediate(const Text& text, const glm::vec2& position, float rotation, const glm::vec2& scale) {
    // Create texture and render it immediately
    auto texture = RenderToTexture(text);
    if (!texture || !m_renderer) return;
    
    // Calculate final position with alignment offset
    glm::ivec2 textSize = {texture->GetWidth(), texture->GetHeight()};
    glm::vec2 alignmentOffset = CalculateAlignmentOffset(text, textSize);
    glm::vec2 finalPosition = position + alignmentOffset;
    
    // Create a sprite from the texture and render it
    rendering::Sprite textSprite(texture);
    core::Transform transform(finalPosition, rotation, scale);
    
    m_renderer->DrawSprite(&textSprite, transform);
}

std::shared_ptr<rendering::Texture> TextRenderer::RenderToTexture(const Text& text) {
    // Create surface with text
    SDL_Surface* textSurface = CreateTextSurface(text);
    if (!textSurface) return nullptr;
    
    // Convert surface to RGBA format for OpenGL
    SDL_Surface* rgbaSurface = SDL_ConvertSurfaceFormat(textSurface, SDL_PIXELFORMAT_RGBA32, 0);
    SDL_FreeSurface(textSurface);
    
    if (!rgbaSurface) {
        spdlog::error("Failed to convert text surface to RGBA format");
        return nullptr;
    }
    
    // Apply effects if needed (placeholder for now)
    SDL_Surface* finalSurface = ApplyTextEffects(rgbaSurface, text);
    
    // Create texture from surface
    auto texture = std::make_shared<rendering::Texture>();
    bool success = texture->LoadFromMemory(
        (unsigned char*)finalSurface->pixels,
        finalSurface->w,
        finalSurface->h,
        4 // RGBA
    );
    
    // Clean up surfaces
    if (finalSurface != rgbaSurface) {
        SDL_FreeSurface(finalSurface);
    }
    SDL_FreeSurface(rgbaSurface);
    
    if (!success) {
        spdlog::error("Failed to create texture from text surface");
        return nullptr;
    }
    
    return texture;
}

void TextRenderer::RenderCached(const Text& text, const glm::vec2& position, float rotation, const glm::vec2& scale) {
    // Generate cache key
    std::string cacheKey = GenerateTextHash(text);
    
    std::shared_ptr<rendering::Texture> texture;
    
    // Check if we have this text cached
    auto it = m_textCache.find(cacheKey);
    if (it != m_textCache.end()) {
        // Update last used frame for LRU
        it->second.lastUsedFrame = m_currentFrame;
        texture = it->second.texture;
    } else {
        // Not cached - render to texture and cache it
        texture = RenderToTexture(text);
        if (!texture) return;
        
        // Calculate memory size (rough estimate)
        glm::ivec2 size = {texture->GetWidth(), texture->GetHeight()};
        size_t memorySize = size.x * size.y * 4; // 4 bytes per pixel (RGBA)
        
        // Check if we have room in cache
        while (m_currentCacheSize + memorySize > m_maxCacheSize && !m_textCache.empty()) {
            EvictOldCacheEntries();
        }
        
        // Add to cache
        CachedTextData cacheData;
        cacheData.texture = texture;
        cacheData.size = size;
        cacheData.memorySize = memorySize;
        cacheData.lastUsedFrame = m_currentFrame;
        cacheData.textHash = cacheKey;
        
        m_textCache[cacheKey] = cacheData;
        m_currentCacheSize += memorySize;
    }
    
    if (!texture || !m_renderer) return;
    
    // Calculate final position with alignment offset
    glm::ivec2 textSize = {texture->GetWidth(), texture->GetHeight()};
    glm::vec2 alignmentOffset = CalculateAlignmentOffset(text, textSize);
    glm::vec2 finalPosition = position + alignmentOffset;
    
    // Create a sprite from the texture and render it
    rendering::Sprite textSprite(texture);
    core::Transform transform(finalPosition, rotation, scale);
    
    m_renderer->DrawSprite(&textSprite, transform);
}

std::vector<std::string> TextRenderer::WrapText(const std::string& content, const std::string& fontName, float maxWidth) {
    std::vector<std::string> lines;
    
    if (maxWidth <= 0) {
        lines.push_back(content);
        return lines;
    }
    
    std::istringstream words(content);
    std::string word;
    std::string currentLine;
    
    while (words >> word) {
        std::string testLine = currentLine.empty() ? word : currentLine + " " + word;
        
        int width, height;
        m_fontManager->GetTextSize(testLine, fontName, width, height);
        
        if (width <= maxWidth) {
            currentLine = testLine;
        } else {
            if (!currentLine.empty()) {
                lines.push_back(currentLine);
                currentLine = word;
            } else {
                // Single word is too long - add it anyway
                lines.push_back(word);
            }
        }
    }
    
    if (!currentLine.empty()) {
        lines.push_back(currentLine);
    }
    
    return lines;
}

std::string TextRenderer::GenerateTextHash(const Text& text) {
    // Create a hash from all text properties that affect rendering
    std::ostringstream hash;
    hash << text.GetContent() << "|"
         << text.GetFont() << "|"
         << std::fixed << std::setprecision(3)
         << text.GetColor().r << "," << text.GetColor().g << "," 
         << text.GetColor().b << "," << text.GetColor().a << "|"
         << static_cast<int>(text.GetAlignment()) << "|"
         << text.IsWordWrapEnabled() << "|" << text.GetMaxWidth() << "|"
         << text.GetLineSpacing() << "|"
         << text.IsOutlineEnabled() << "|" << text.GetOutlineThickness() << "|"
         << text.GetOutlineColor().r << "," << text.GetOutlineColor().g << ","
         << text.GetOutlineColor().b << "," << text.GetOutlineColor().a << "|"
         << text.IsShadowEnabled() << "|" 
         << text.GetShadowOffset().x << "," << text.GetShadowOffset().y << "|"
         << text.GetShadowColor().r << "," << text.GetShadowColor().g << ","
         << text.GetShadowColor().b << "," << text.GetShadowColor().a;
    
    return hash.str();
}

glm::ivec2 TextRenderer::CalculateTextSize(const Text& text) {
    if (text.IsWordWrapEnabled()) {
        auto lines = WrapText(text.GetContent(), text.GetFont(), text.GetMaxWidth());
        int maxWidth = 0;
        int totalHeight = 0;
        
        for (const auto& line : lines) {
            int width, height;
            m_fontManager->GetTextSize(line, text.GetFont(), width, height);
            maxWidth = std::max(maxWidth, width);
            totalHeight += static_cast<int>(height * text.GetLineSpacing());
        }
        
        return {maxWidth, totalHeight};
    } else {
        int width, height;
        m_fontManager->GetTextSize(text.GetContent(), text.GetFont(), width, height);
        return {width, height};
    }
}

void TextRenderer::EvictOldCacheEntries() {
    if (m_textCache.empty()) return;
    
    // If we're over the cache limit, remove oldest entries
    while (m_currentCacheSize > m_maxCacheSize && !m_textCache.empty()) {
        // Find the oldest entry (lowest lastUsedFrame)
        auto oldestIt = std::min_element(m_textCache.begin(), m_textCache.end(),
            [](const auto& a, const auto& b) {
                return a.second.lastUsedFrame < b.second.lastUsedFrame;
            });
        
        m_currentCacheSize -= oldestIt->second.memorySize;
        m_textCache.erase(oldestIt);
    }
}

void TextRenderer::RemoveCacheEntry(const std::string& key) {
    auto it = m_textCache.find(key);
    if (it != m_textCache.end()) {
        m_currentCacheSize -= it->second.memorySize;
        m_textCache.erase(it);
    }
}

SDL_Surface* TextRenderer::CreateTextSurface(const Text& text) {
    TTF_Font* font = m_fontManager->GetFont(text.GetFont());
    if (!font) {
        spdlog::warn("Font '{}' not available for text rendering", text.GetFont());
        return nullptr;
    }
    
    // Convert color to SDL format
    SDL_Color color = {
        static_cast<Uint8>(text.GetColor().r * 255),
        static_cast<Uint8>(text.GetColor().g * 255),
        static_cast<Uint8>(text.GetColor().b * 255),
        static_cast<Uint8>(text.GetColor().a * 255)
    };
    
    SDL_Surface* surface = nullptr;
    
    if (text.IsWordWrapEnabled()) {
        // Handle word wrapping
        auto lines = WrapText(text.GetContent(), text.GetFont(), text.GetMaxWidth());
        
        // Calculate total size
        int maxWidth = 0;
        int totalHeight = 0;
        int lineHeight = m_fontManager->GetFontLineSkip(text.GetFont());
        
        std::vector<SDL_Surface*> lineSurfaces;
        
        for (const auto& line : lines) {
            SDL_Surface* lineSurface = TTF_RenderUTF8_Blended(font, line.c_str(), color);
            if (lineSurface) {
                lineSurfaces.push_back(lineSurface);
                maxWidth = std::max(maxWidth, lineSurface->w);
                totalHeight += static_cast<int>(lineHeight * text.GetLineSpacing());
            }
        }
        
        if (!lineSurfaces.empty()) {
            // Create combined surface
            surface = SDL_CreateRGBSurface(0, maxWidth, totalHeight, 32,
                0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
            
            if (surface) {
                // Fill with transparent
                SDL_FillRect(surface, nullptr, SDL_MapRGBA(surface->format, 0, 0, 0, 0));
                
                // Blit lines
                int yOffset = 0;
                for (SDL_Surface* lineSurface : lineSurfaces) {
                    SDL_Rect destRect = {0, yOffset, lineSurface->w, lineSurface->h};
                    
                    // Apply alignment
                    if (text.GetAlignment() == TextAlignment::Center) {
                        destRect.x = (maxWidth - lineSurface->w) / 2;
                    } else if (text.GetAlignment() == TextAlignment::Right) {
                        destRect.x = maxWidth - lineSurface->w;
                    }
                    
                    SDL_BlitSurface(lineSurface, nullptr, surface, &destRect);
                    yOffset += static_cast<int>(lineHeight * text.GetLineSpacing());
                }
            }
        }
        
        // Clean up line surfaces
        for (SDL_Surface* lineSurface : lineSurfaces) {
            SDL_FreeSurface(lineSurface);
        }
    } else {
        // Simple single-line rendering
        surface = TTF_RenderUTF8_Blended(font, text.GetContent().c_str(), color);
    }
    
    return surface;
}

SDL_Surface* TextRenderer::CreateOutlineTextSurface(const Text& text) {
    // This is a simplified outline implementation
    // A more sophisticated version would render the text multiple times with offsets
    return CreateTextSurface(text);
}

SDL_Surface* TextRenderer::ApplyTextEffects(SDL_Surface* textSurface, const Text& text) {
    if (!text.IsOutlineEnabled() && !text.IsShadowEnabled()) {
        return textSurface;
    }
    
    // For now, return the original surface
    // Full effect implementation would involve multiple rendering passes
    return textSurface;
}

glm::vec2 TextRenderer::CalculateAlignmentOffset(const Text& text, const glm::ivec2& textSize) {
    glm::vec2 offset(0.0f);
    
    switch (text.GetAlignment()) {
        case TextAlignment::Center:
            offset.x = -textSize.x * 0.5f;
            break;
        case TextAlignment::Right:
            offset.x = -static_cast<float>(textSize.x);
            break;
        case TextAlignment::Left:
        case TextAlignment::Justify:
        default:
            // No offset needed
            break;
    }
    
    return offset;
}

} // namespace text