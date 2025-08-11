#include "engine/public/text/Text.h"

namespace text {

// Color constants
const glm::vec4 Text::White = {1.0f, 1.0f, 1.0f, 1.0f};
const glm::vec4 Text::Black = {0.0f, 0.0f, 0.0f, 1.0f};
const glm::vec4 Text::Red = {1.0f, 0.0f, 0.0f, 1.0f};
const glm::vec4 Text::Green = {0.0f, 1.0f, 0.0f, 1.0f};
const glm::vec4 Text::Blue = {0.0f, 0.0f, 1.0f, 1.0f};
const glm::vec4 Text::Yellow = {1.0f, 1.0f, 0.0f, 1.0f};
const glm::vec4 Text::Transparent = {0.0f, 0.0f, 0.0f, 0.0f};

Text::Text() = default;

Text::Text(const std::string& content) : m_content(content) {
    MarkAsChanged();
}

Text::Text(const std::string& content, const std::string& fontName) 
    : m_content(content), m_fontName(fontName) {
    MarkAsChanged();
}

Text::Text(const std::string& content, const std::string& fontName, const glm::vec4& color) 
    : m_content(content), m_fontName(fontName), m_color(color) {
    MarkAsChanged();
}

void Text::SetContent(const std::string& content) {
    if (m_content != content) {
        m_content = content;
        MarkAsChanged();
    }
}

void Text::SetFont(const std::string& fontName) {
    if (m_fontName != fontName) {
        m_fontName = fontName;
        MarkAsChanged();
    }
}

void Text::SetColor(const glm::vec4& color) {
    if (m_color != color) {
        m_color = color;
        MarkAsChanged();
    }
}

void Text::SetColor(float r, float g, float b, float a) {
    SetColor(glm::vec4(r, g, b, a));
}

void Text::SetAlignment(TextAlignment alignment) {
    if (m_alignment != alignment) {
        m_alignment = alignment;
        MarkAsChanged();
    }
}

void Text::SetWordWrap(bool enabled, float maxWidth) {
    if (m_wordWrap != enabled || m_maxWidth != maxWidth) {
        m_wordWrap = enabled;
        m_maxWidth = maxWidth;
        MarkAsChanged();
    }
}

void Text::SetLineSpacing(float spacing) {
    if (m_lineSpacing != spacing) {
        m_lineSpacing = spacing;
        MarkAsChanged();
    }
}

void Text::SetRenderMode(TextRenderMode mode) {
    if (m_renderMode != mode) {
        m_renderMode = mode;
        MarkAsChanged();
    }
}

void Text::SetOutline(bool enabled, float thickness, const glm::vec4& color) {
    if (m_outlineEnabled != enabled || m_outlineThickness != thickness || m_outlineColor != color) {
        m_outlineEnabled = enabled;
        m_outlineThickness = thickness;
        m_outlineColor = color;
        MarkAsChanged();
    }
}

void Text::SetShadow(bool enabled, const glm::vec2& offset, const glm::vec4& color) {
    if (m_shadowEnabled != enabled || m_shadowOffset != offset || m_shadowColor != color) {
        m_shadowEnabled = enabled;
        m_shadowOffset = offset;
        m_shadowColor = color;
        MarkAsChanged();
    }
}

} // namespace text