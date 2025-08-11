#pragma once

#include <string>
#include <glm/glm.hpp>

namespace text {

    enum class TextAlignment {
        Left,
        Center,
        Right,
        Justify
    };

    enum class TextRenderMode {
        Cached,     // Pre-render to texture (default, good for static text)
        Immediate   // Render each frame (good for dynamic text)
    };

    /**
     * Text Class
     * 
     * Represents a text object with visual properties.
     * Does NOT contain position - that's handled by Transform or passed to renderer.
     * 
     * Supports:
     * - Font selection and styling
     * - Color and effects (outline, shadow)
     * - Alignment and word wrapping
     * - Render mode selection (cached vs immediate)
     */
    class Text {
    public:
        Text();
        explicit Text(const std::string& content);
        Text(const std::string& content, const std::string& fontName);
        Text(const std::string& content, const std::string& fontName, const glm::vec4& color);
        
        // Content
        void SetContent(const std::string& content);
        const std::string& GetContent() const { return m_content; }
        
        // Font
        void SetFont(const std::string& fontName);
        const std::string& GetFont() const { return m_fontName; }
        
        // Color
        void SetColor(const glm::vec4& color);
        void SetColor(float r, float g, float b, float a = 1.0f);
        const glm::vec4& GetColor() const { return m_color; }
        
        // Alignment and layout
        void SetAlignment(TextAlignment alignment);
        TextAlignment GetAlignment() const { return m_alignment; }
        
        void SetWordWrap(bool enabled, float maxWidth = 0.0f);
        bool IsWordWrapEnabled() const { return m_wordWrap; }
        float GetMaxWidth() const { return m_maxWidth; }
        
        void SetLineSpacing(float spacing);
        float GetLineSpacing() const { return m_lineSpacing; }
        
        // Render mode
        void SetRenderMode(TextRenderMode mode);
        TextRenderMode GetRenderMode() const { return m_renderMode; }
        
        // Effects - Outline
        void SetOutline(bool enabled, float thickness = 1.0f, const glm::vec4& color = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
        bool IsOutlineEnabled() const { return m_outlineEnabled; }
        float GetOutlineThickness() const { return m_outlineThickness; }
        const glm::vec4& GetOutlineColor() const { return m_outlineColor; }
        
        // Effects - Shadow
        void SetShadow(bool enabled, const glm::vec2& offset = glm::vec2(2.0f, 2.0f), const glm::vec4& color = glm::vec4(0.0f, 0.0f, 0.0f, 0.5f));
        bool IsShadowEnabled() const { return m_shadowEnabled; }
        const glm::vec2& GetShadowOffset() const { return m_shadowOffset; }
        const glm::vec4& GetShadowColor() const { return m_shadowColor; }
        
        // Utility
        bool IsEmpty() const { return m_content.empty(); }
        bool HasChanged() const { return m_hasChanged; }
        void MarkAsClean() { m_hasChanged = false; }
        
        // Convenience color constants
        static const glm::vec4 White;
        static const glm::vec4 Black;
        static const glm::vec4 Red;
        static const glm::vec4 Green;
        static const glm::vec4 Blue;
        static const glm::vec4 Yellow;
        static const glm::vec4 Transparent;
        
    private:
        void MarkAsChanged() { m_hasChanged = true; }
        
        // Core properties
        std::string m_content;
        std::string m_fontName;
        glm::vec4 m_color = {1.0f, 1.0f, 1.0f, 1.0f}; // White
        
        // Layout properties
        TextAlignment m_alignment = TextAlignment::Left;
        bool m_wordWrap = false;
        float m_maxWidth = 0.0f;
        float m_lineSpacing = 1.0f;
        
        // Render properties
        TextRenderMode m_renderMode = TextRenderMode::Cached;
        
        // Effects
        bool m_outlineEnabled = false;
        float m_outlineThickness = 1.0f;
        glm::vec4 m_outlineColor = {0.0f, 0.0f, 0.0f, 1.0f}; // Black
        
        bool m_shadowEnabled = false;
        glm::vec2 m_shadowOffset = {2.0f, 2.0f};
        glm::vec4 m_shadowColor = {0.0f, 0.0f, 0.0f, 0.5f}; // Semi-transparent black
        
        // State tracking
        bool m_hasChanged = true;
    };

} // namespace text