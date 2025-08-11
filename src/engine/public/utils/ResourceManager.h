#pragma once

#include <memory>
#include <unordered_map>
#include <string>

namespace rendering {
    class Texture;
}

namespace text {
    class Font;
}

namespace utils {
    
    class ResourceManager {
    public:
        ResourceManager();
        ~ResourceManager();
        
        bool Initialize();
        void Shutdown();
        
        // Automatic loading with caching
        std::shared_ptr<rendering::Texture> LoadTexture(const std::string& path);
        
        // Manual resource management
        void UnloadTexture(const std::string& path);
        void UnloadAllTextures();
        
        // Resource info
        size_t GetTextureCount() const;
        size_t GetMemoryUsage() const;
        
    private:
        std::unordered_map<std::string, std::weak_ptr<rendering::Texture>> m_textures;
        bool m_initialized = false;
    };
}