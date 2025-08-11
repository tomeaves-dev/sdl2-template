#include "engine/public/rendering/Sprite.h"
#include "engine/public/core/Engine.h"
#include "engine/public/utils/ResourceManager.h"
#include <spdlog/spdlog.h>

namespace rendering {

Sprite::Sprite() = default;

Sprite::Sprite(const std::string& texturePath) {
    m_texture = core::Engine::Resources().LoadTexture(texturePath);
    if (m_texture && m_texture->IsValid()) {
        UpdateSourceRect();
    }
}

Sprite::Sprite(std::shared_ptr<Texture> texture) : m_texture(texture) {
    if (m_texture) {
        UpdateSourceRect();
    }
}

Sprite::Sprite(std::shared_ptr<Texture> texture, const glm::ivec4& sourceRect) 
    : m_texture(texture), m_sourceRect(sourceRect) {
}

void Sprite::SetTexture(std::shared_ptr<Texture> texture) {
    m_texture = texture;
    if (m_texture) {
        UpdateSourceRect();
    }
}

void Sprite::SetSourceRect(const glm::ivec4& rect) {
    m_sourceRect = rect;
}

void Sprite::SetSourceRect(int x, int y, int width, int height) {
    m_sourceRect = {x, y, width, height};
}

void Sprite::UseFullTexture() {
    if (m_texture) {
        UpdateSourceRect();
    }
}

void Sprite::SetOrigin(const glm::vec2& origin) {
    m_origin = origin;
}

void Sprite::SetOrigin(float x, float y) {
    m_origin = {x, y};
}

void Sprite::SetOriginToCenter() {
    m_origin = {0.5f, 0.5f};
}

void Sprite::SetTint(const glm::vec4& color) {
    m_tint = color;
}

void Sprite::SetTint(float r, float g, float b, float a) {
    m_tint = {r, g, b, a};
}

void Sprite::SetAlpha(float alpha) {
    m_tint.a = alpha;
}

glm::ivec2 Sprite::GetSize() const {
    return {m_sourceRect.z, m_sourceRect.w}; // width, height
}

int Sprite::GetWidth() const {
    return m_sourceRect.z;
}

int Sprite::GetHeight() const {
    return m_sourceRect.w;
}

void Sprite::UpdateSourceRect() {
    if (m_texture && m_texture->IsValid()) {
        m_sourceRect = {0, 0, m_texture->GetWidth(), m_texture->GetHeight()};
    } else {
        m_sourceRect = {0, 0, 0, 0};
    }
}

} // namespace rendering