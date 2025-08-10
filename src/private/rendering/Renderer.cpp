#include "public/rendering/Renderer.h"
#include "public/rendering/Window.h"
#include "public/rendering/Sprite.h"
#include "public/rendering/Texture.h"
#include "public/core/Transform.h"
#include <spdlog/spdlog.h>
#include <SDL.h>
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace rendering {

// Vertex shader source for sprite rendering (OpenGL 2.1 compatible)
const char* spriteVertexShader = R"(
#version 120
attribute vec2 aPos;
attribute vec2 aTexCoord;

uniform mat4 u_MVP;
uniform vec2 u_spriteSize;
uniform vec2 u_origin;

varying vec2 TexCoord;

void main()
{
    // Apply origin offset to vertex position
    vec2 offsetPos = aPos - u_origin;
    
    // Scale by sprite size
    vec2 scaledPos = offsetPos * u_spriteSize;
    
    gl_Position = u_MVP * vec4(scaledPos, 0.0, 1.0);
    TexCoord = aTexCoord;
}
)";

// Fragment shader source for sprite rendering (OpenGL 2.1 compatible)
const char* spriteFragmentShader = R"(
#version 120
varying vec2 TexCoord;

uniform sampler2D u_texture;
uniform vec4 u_tint;
uniform vec4 u_sourceRect; // x, y, width, height in texture coordinates

void main()
{
    // Calculate texture coordinates based on source rectangle
    vec2 adjustedTexCoord = u_sourceRect.xy + TexCoord * u_sourceRect.zw;
    
    vec4 texColor = texture2D(u_texture, adjustedTexCoord);
    gl_FragColor = texColor * u_tint;
    
    // Discard fully transparent pixels
    if (gl_FragColor.a < 0.01)
        discard;
}
)";

Renderer::Renderer() : m_context(nullptr) {
}

Renderer::~Renderer() {
    Shutdown();
}

bool Renderer::Initialize(Window* window) {
    spdlog::info("Initializing renderer...");
    
    if (!window || !window->GetSDLWindow()) {
        spdlog::error("Invalid window provided to renderer");
        return false;
    }
    
    // Set OpenGL attributes before creating context
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    
    // Create OpenGL context
    m_context = SDL_GL_CreateContext(window->GetSDLWindow());
    if (!m_context) {
        spdlog::error("Failed to create OpenGL context: {}", SDL_GetError());
        return false;
    }
    
    // Initialize GLAD
    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
        spdlog::error("Failed to initialize GLAD");
        return false;
    }
    
    // Set OpenGL settings
    glViewport(0, 0, window->GetWidth(), window->GetHeight());
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // Setup default orthographic projection
    SetOrthographicProjection(0.0f, (float)window->GetWidth(), (float)window->GetHeight(), 0.0f);
    
    // Setup sprite rendering
    SetupSpriteShader();
    SetupQuadMesh();
    
    m_initialized = true;
    spdlog::info("Renderer initialized successfully");
    spdlog::info("OpenGL Version: {}", (char*)glGetString(GL_VERSION));
    
    return true;
}

void Renderer::Shutdown() {
    if (m_initialized) {
        // Cleanup OpenGL resources
        if (m_quadVBO) {
            glDeleteBuffers(1, &m_quadVBO);
            m_quadVBO = 0;
        }
        if (m_spriteShader) {
            glDeleteProgram(m_spriteShader);
            m_spriteShader = 0;
        }
        
        m_initialized = false;
    }
    
    if (m_context) {
        SDL_GL_DeleteContext((SDL_GLContext)m_context);
        m_context = nullptr;
        spdlog::info("Renderer shutdown");
    }
}

void Renderer::BeginFrame() {
    // Set viewport and clear any state if needed
}

void Renderer::EndFrame() {
    // Present frame operations if needed
}

void Renderer::Clear(float r, float g, float b, float a) {
    glClearColor(r, g, b, a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::DrawSprite(const Sprite* sprite, const core::Transform& transform) {
    if (!sprite || !sprite->IsValid() || !m_initialized) {
        return;
    }
    
    // Create model matrix from transform
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(transform.GetPosition(), 0.0f));
    model = glm::rotate(model, glm::radians(transform.GetRotation()), glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::scale(model, glm::vec3(transform.GetScale(), 1.0f));
    
    glm::mat4 mvp = m_projectionMatrix * m_viewMatrix * model;
    
    // Use sprite shader
    glUseProgram(m_spriteShader);
    
    // Set uniforms
    glUniformMatrix4fv(glGetUniformLocation(m_spriteShader, "u_MVP"), 1, GL_FALSE, glm::value_ptr(mvp));
    
    // Sprite size
    glm::vec2 spriteSize = glm::vec2(sprite->GetSize());
    glUniform2fv(glGetUniformLocation(m_spriteShader, "u_spriteSize"), 1, glm::value_ptr(spriteSize));
    
    // Origin
    glm::vec2 origin = sprite->GetOrigin();
    glUniform2fv(glGetUniformLocation(m_spriteShader, "u_origin"), 1, glm::value_ptr(origin));
    
    // Tint
    glUniform4fv(glGetUniformLocation(m_spriteShader, "u_tint"), 1, glm::value_ptr(sprite->GetTint()));
    
    // Source rectangle (normalized to texture coordinates)
    auto texture = sprite->GetTexture();
    auto sourceRect = sprite->GetSourceRect();
    glm::vec4 normalizedSourceRect = {
        (float)sourceRect.x / texture->GetWidth(),
        (float)sourceRect.y / texture->GetHeight(),
        (float)sourceRect.z / texture->GetWidth(),
        (float)sourceRect.w / texture->GetHeight()
    };
    glUniform4fv(glGetUniformLocation(m_spriteShader, "u_sourceRect"), 1, glm::value_ptr(normalizedSourceRect));
    
    // Bind texture
    texture->Bind(0);
    glUniform1i(glGetUniformLocation(m_spriteShader, "u_texture"), 0);
    
    // Setup vertex data (OpenGL 2.1 style - no VAO)
    glBindBuffer(GL_ARRAY_BUFFER, m_quadVBO);
    
    // Get attribute locations
    GLint posAttrib = glGetAttribLocation(m_spriteShader, "aPos");
    GLint texAttrib = glGetAttribLocation(m_spriteShader, "aTexCoord");
    
    // Enable and set vertex attributes
    if (posAttrib >= 0) {
        glEnableVertexAttribArray(posAttrib);
        glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    }
    
    if (texAttrib >= 0) {
        glEnableVertexAttribArray(texAttrib);
        glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    }
    
    // Draw quad
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    // Cleanup
    if (posAttrib >= 0) glDisableVertexAttribArray(posAttrib);
    if (texAttrib >= 0) glDisableVertexAttribArray(texAttrib);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    texture->Unbind();
    glUseProgram(0);
}

void Renderer::DrawSprite(const Sprite* sprite, const glm::vec2& position) {
    core::Transform transform(position);
    DrawSprite(sprite, transform);
}

void Renderer::DrawSprite(const Sprite* sprite, const glm::vec2& position, float rotation) {
    core::Transform transform(position, rotation);
    DrawSprite(sprite, transform);
}

void Renderer::DrawSprite(const Sprite* sprite, const glm::vec2& position, float rotation, const glm::vec2& scale) {
    core::Transform transform(position, rotation, scale);
    DrawSprite(sprite, transform);
}

void Renderer::DrawRectangle(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color) {
    // TODO: Implement primitive rectangle rendering
    // For now, this is a placeholder for future primitive rendering
}

void Renderer::DrawLine(const glm::vec2& start, const glm::vec2& end, const glm::vec4& color, float thickness) {
    // TODO: Implement line rendering
    // For now, this is a placeholder for future primitive rendering
}

void Renderer::SetViewMatrix(const glm::mat4& view) {
    m_viewMatrix = view;
}

void Renderer::SetProjectionMatrix(const glm::mat4& projection) {
    m_projectionMatrix = projection;
}

void Renderer::SetOrthographicProjection(float left, float right, float bottom, float top) {
    m_projectionMatrix = glm::ortho(left, right, bottom, top, -1.0f, 1.0f);
}

void Renderer::SetupSpriteShader() {
    // Compile vertex shader
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &spriteVertexShader, nullptr);
    glCompileShader(vertexShader);
    
    // Check vertex shader compilation
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
        spdlog::error("Vertex shader compilation failed: {}", infoLog);
        glDeleteShader(vertexShader);
        return;
    }
    
    // Compile fragment shader
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &spriteFragmentShader, nullptr);
    glCompileShader(fragmentShader);
    
    // Check fragment shader compilation
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
        spdlog::error("Fragment shader compilation failed: {}", infoLog);
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        return;
    }
    
    // Create shader program
    m_spriteShader = glCreateProgram();
    glAttachShader(m_spriteShader, vertexShader);
    glAttachShader(m_spriteShader, fragmentShader);
    glLinkProgram(m_spriteShader);
    
    // Check program linking
    glGetProgramiv(m_spriteShader, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(m_spriteShader, 512, nullptr, infoLog);
        spdlog::error("Shader program linking failed: {}", infoLog);
        glDeleteProgram(m_spriteShader);
        m_spriteShader = 0;
    }
    
    // Cleanup shaders (they're linked into the program now)
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    
    if (m_spriteShader) {
        spdlog::info("Sprite shader compiled and linked successfully");
    }
}

void Renderer::SetupQuadMesh() {
    // Quad vertices (position + texture coordinates)
    // Using unit quad that will be scaled by sprite size
    float quadVertices[] = {
        // positions   // texture coords
        0.0f, 1.0f,    0.0f, 1.0f,   // top left
        1.0f, 1.0f,    1.0f, 1.0f,   // top right
        0.0f, 0.0f,    0.0f, 0.0f,   // bottom left
        
        1.0f, 1.0f,    1.0f, 1.0f,   // top right
        1.0f, 0.0f,    1.0f, 0.0f,   // bottom right
        0.0f, 0.0f,    0.0f, 0.0f    // bottom left
    };
    
    glGenBuffers(1, &m_quadVBO);
    
    glBindBuffer(GL_ARRAY_BUFFER, m_quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
    
    spdlog::info("Sprite quad mesh created");
}

} // namespace rendering