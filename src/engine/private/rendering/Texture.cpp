#include "engine/public/rendering/Texture.h"
#include <SDL_image.h>
#include <spdlog/spdlog.h>

namespace rendering {

Texture::Texture() = default;

Texture::~Texture() {
    Cleanup();
}

Texture::Texture(Texture&& other) noexcept 
    : m_textureID(other.m_textureID)
    , m_width(other.m_width)
    , m_height(other.m_height)
    , m_channels(other.m_channels)
    , m_filepath(std::move(other.m_filepath)) {
    
    other.m_textureID = 0;
    other.m_width = 0;
    other.m_height = 0;
    other.m_channels = 0;
}

Texture& Texture::operator=(Texture&& other) noexcept {
    if (this != &other) {
        Cleanup();
        
        m_textureID = other.m_textureID;
        m_width = other.m_width;
        m_height = other.m_height;
        m_channels = other.m_channels;
        m_filepath = std::move(other.m_filepath);
        
        other.m_textureID = 0;
        other.m_width = 0;
        other.m_height = 0;
        other.m_channels = 0;
    }
    return *this;
}

bool Texture::LoadFromFile(const std::string& filepath) {
    m_filepath = filepath;
    
    // Load image using SDL_image
    SDL_Surface* surface = IMG_Load(filepath.c_str());
    if (!surface) {
        spdlog::error("Failed to load texture '{}': {}", filepath, IMG_GetError());
        return false;
    }
    
    // Convert to RGBA format for consistency
    SDL_Surface* rgbaSurface = SDL_ConvertSurfaceFormat(surface, SDL_PIXELFORMAT_RGBA32, 0);
    SDL_FreeSurface(surface);
    
    if (!rgbaSurface) {
        spdlog::error("Failed to convert texture '{}' to RGBA format: {}", filepath, SDL_GetError());
        return false;
    }
    
    m_width = rgbaSurface->w;
    m_height = rgbaSurface->h;
    m_channels = 4; // RGBA
    
    // Create OpenGL texture
    glGenTextures(1, &m_textureID);
    glBindTexture(GL_TEXTURE_2D, m_textureID);
    
    // Upload texture data
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, rgbaSurface->pixels);
    
    // Set default texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    
    glBindTexture(GL_TEXTURE_2D, 0);
    
    SDL_FreeSurface(rgbaSurface);
    
    spdlog::info("Loaded texture '{}' ({}x{})", filepath, m_width, m_height);
    return true;
}

bool Texture::LoadFromMemory(const unsigned char* data, int width, int height, int channels) {
    if (!data || width <= 0 || height <= 0) {
        spdlog::error("Invalid texture data parameters");
        return false;
    }
    
    m_width = width;
    m_height = height;
    m_channels = channels;
    
    // Determine OpenGL format
    GLenum format = GL_RGBA;
    if (channels == 1) format = GL_RED;
    else if (channels == 3) format = GL_RGB;
    else if (channels == 4) format = GL_RGBA;
    
    // Create OpenGL texture
    glGenTextures(1, &m_textureID);
    glBindTexture(GL_TEXTURE_2D, m_textureID);
    
    // Upload texture data
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    
    // Set default texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    
    glBindTexture(GL_TEXTURE_2D, 0);
    
    spdlog::info("Created texture from memory ({}x{}, {} channels)", width, height, channels);
    return true;
}

void Texture::Bind(unsigned int slot) const {
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, m_textureID);
}

void Texture::Unbind() const {
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::SetFilterMode(GLenum minFilter, GLenum magFilter) {
    glBindTexture(GL_TEXTURE_2D, m_textureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::SetWrapMode(GLenum wrapS, GLenum wrapT) {
    glBindTexture(GL_TEXTURE_2D, m_textureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapS);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapT);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::Cleanup() {
    if (m_textureID != 0) {
        glDeleteTextures(1, &m_textureID);
        m_textureID = 0;
    }
}

} // namespace rendering