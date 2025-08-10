#pragma once

#include "Texture.h"
#include <glm/glm.hpp>
#include <memory>

namespace rendering {

    /**
     * Sprite Class
     * 
     * Represents visual data for rendering a 2D sprite.
     * Does NOT contain position/rotation - that's handled by Transform.
     * 
     * Supports:
     * - Full texture rendering
     * - Sprite sheet sub-rectangles
     * - Custom origin points (pivot)
     * - Tinting/coloring
     */
    class Sprite {
    public:
        Sprite();
        explicit Sprite(std::shared_ptr<Texture> texture);
        Sprite(std::shared_ptr<Texture> texture, const glm::ivec4& sourceRect);
        
        // Texture management
        void SetTexture(std::shared_ptr<Texture> texture);
        std::shared_ptr<Texture> GetTexture() const { return m_texture; }
        
        // Source rectangle (for sprite sheets)
        void SetSourceRect(const glm::ivec4& rect); // x, y, width, height
        void SetSourceRect(int x, int y, int width, int height);
        void UseFullTexture(); // Reset to use entire texture
        const glm::ivec4& GetSourceRect() const { return m_sourceRect; }
        
        // Origin/pivot point (relative to source rectangle)
        void SetOrigin(const glm::vec2& origin); // 0,0 = top-left, 0.5,0.5 = center
        void SetOrigin(float x, float y);
        void SetOriginToCenter();
        void SetOriginToTopLeft() { SetOrigin(0.0f, 0.0f); }
        void SetOriginToBottomCenter() { SetOrigin(0.5f, 1.0f); }
        const glm::vec2& GetOrigin() const { return m_origin; }
        
        // Visual properties
        void SetTint(const glm::vec4& color); // RGBA tint (1,1,1,1 = no tint)
        void SetTint(float r, float g, float b, float a = 1.0f);
        void SetAlpha(float alpha);
        const glm::vec4& GetTint() const { return m_tint; }
        
        // Size information
        glm::ivec2 GetSize() const;
        int GetWidth() const;
        int GetHeight() const;
        
        // Utility
        bool IsValid() const { return m_texture && m_texture->IsValid(); }
        
    private:
        void UpdateSourceRect();
        
        std::shared_ptr<Texture> m_texture;
        glm::ivec4 m_sourceRect = {0, 0, 0, 0}; // x, y, width, height
        glm::vec2 m_origin = {0.0f, 0.0f};      // Normalized origin point
        glm::vec4 m_tint = {1.0f, 1.0f, 1.0f, 1.0f}; // RGBA tint
    };

} // namespace rendering