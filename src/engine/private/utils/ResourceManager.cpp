#include "engine/public/utils/ResourceManager.h"
#include "engine/public/rendering/Texture.h"
#include "engine/public/utils/Logger.h"
#include <spdlog/spdlog.h>

namespace utils {

ResourceManager::ResourceManager() = default;

ResourceManager::~ResourceManager() {
    Shutdown();
}

bool ResourceManager::Initialize() {
    if (m_initialized) {
        return true;
    }
    
    spdlog::info("Initializing ResourceManager...");
    m_initialized = true;
    return true;
}

void ResourceManager::Shutdown() {
    if (!m_initialized) {
        return;
    }
    
    spdlog::info("Shutting down ResourceManager...");
    UnloadAllTextures();
    m_initialized = false;
}

std::shared_ptr<rendering::Texture> ResourceManager::LoadTexture(const std::string& path) {
    if (!m_initialized) {
        spdlog::error("ResourceManager not initialized");
        return nullptr;
    }
    
    // Check if texture is already loaded
    auto it = m_textures.find(path);
    if (it != m_textures.end()) {
        if (auto texture = it->second.lock()) {
            return texture;
        } else {
            // Weak pointer expired, remove from cache
            m_textures.erase(it);
        }
    }
    
    // Load new texture
    auto texture = std::make_shared<rendering::Texture>();
    if (texture->LoadFromFile(path)) {
        m_textures[path] = texture;
        spdlog::debug("Loaded texture: {}", path);
        return texture;
    } else {
        spdlog::error("Failed to load texture: {}", path);
        return nullptr;
    }
}

void ResourceManager::UnloadTexture(const std::string& path) {
    auto it = m_textures.find(path);
    if (it != m_textures.end()) {
        m_textures.erase(it);
        spdlog::debug("Unloaded texture: {}", path);
    }
}

void ResourceManager::UnloadAllTextures() {
    size_t count = m_textures.size();
    m_textures.clear();
    if (count > 0) {
        spdlog::info("Unloaded {} textures", count);
    }
}

size_t ResourceManager::GetTextureCount() const {
    return m_textures.size();
}

size_t ResourceManager::GetMemoryUsage() const {
    // Simple estimation - would need texture size info for accurate count
    return m_textures.size() * sizeof(rendering::Texture);
}

} // namespace utils