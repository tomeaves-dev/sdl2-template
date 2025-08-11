#pragma once

#include <glm/glm.hpp>

namespace rendering {
    class Window;
    class Sprite;
}

namespace core {
    struct Transform;
}

namespace rendering {
    
    class Renderer {
    public:
        Renderer();
        ~Renderer();
        
        bool Initialize(Window* window);
        void Shutdown();
        
        void BeginFrame();
        void EndFrame();
        void Clear(float r = 0.0f, float g = 0.0f, float b = 0.0f, float a = 1.0f);
        
        // Sprite rendering
        void DrawSprite(const Sprite* sprite, const core::Transform& transform);
        void DrawSprite(const Sprite* sprite, const glm::vec2& position);
        void DrawSprite(const Sprite* sprite, const glm::vec2& position, float rotation);
        void DrawSprite(const Sprite* sprite, const glm::vec2& position, float rotation, const glm::vec2& scale);
        
        // Primitive rendering
        void DrawRectangle(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color);
        void DrawLine(const glm::vec2& start, const glm::vec2& end, const glm::vec4& color, float thickness = 1.0f);
        
        // Camera/View management
        void SetViewMatrix(const glm::mat4& view);
        void SetProjectionMatrix(const glm::mat4& projection);
        void SetOrthographicProjection(float left, float right, float bottom, float top);
        
    private:
        void SetupSpriteShader();
        void SetupQuadMesh();
        
        void* m_context; // SDL_GLContext (using void* to avoid including SDL_opengl.h in header)
        
        // Shader program for sprite rendering
        unsigned int m_spriteShader = 0;
        unsigned int m_quadVBO = 0;
        
        // Matrices
        glm::mat4 m_viewMatrix = glm::mat4(1.0f);
        glm::mat4 m_projectionMatrix = glm::mat4(1.0f);
        
        bool m_initialized = false;
    };
    
} // namespace rendering