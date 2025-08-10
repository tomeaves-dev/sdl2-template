#pragma once

#include <glad/glad.h>
#include <string>
#include <glm/glm.hpp>

namespace rendering {

    /**
     * Texture Class
     * 
     * Wraps OpenGL texture functionality.
     * Handles loading, binding, and basic texture operations.
     */
    class Texture {
    public:
        Texture();
        ~Texture();
        
        // Non-copyable but movable
        Texture(const Texture&) = delete;
        Texture& operator=(const Texture&) = delete;
        Texture(Texture&& other) noexcept;
        Texture& operator=(Texture&& other) noexcept;
        
        // Loading
        bool LoadFromFile(const std::string& filepath);
        bool LoadFromMemory(const unsigned char* data, int width, int height, int channels);
        
        // Binding
        void Bind(unsigned int slot = 0) const;
        void Unbind() const;
        
        // Properties
        unsigned int GetID() const { return m_textureID; }
        int GetWidth() const { return m_width; }
        int GetHeight() const { return m_height; }
        glm::ivec2 GetSize() const { return {m_width, m_height}; }
        bool IsValid() const { return m_textureID != 0; }
        
        // Texture settings
        void SetFilterMode(GLenum minFilter, GLenum magFilter);
        void SetWrapMode(GLenum wrapS, GLenum wrapT);
        
    private:
        void Cleanup();
        
        unsigned int m_textureID = 0;
        int m_width = 0;
        int m_height = 0;
        int m_channels = 0;
        std::string m_filepath;
    };

} // namespace rendering