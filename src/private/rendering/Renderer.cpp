#include "public/rendering/Renderer.h"
#include "public/rendering/Window.h"
#include <spdlog/spdlog.h>
#include <SDL.h>
#include <glad/glad.h>

namespace rendering {

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
    
    spdlog::info("Renderer initialized successfully");
    spdlog::info("OpenGL Version: {}", (char*)glGetString(GL_VERSION));
    
    return true;
}

void Renderer::Shutdown() {
    if (m_context) {
        SDL_GL_DeleteContext((SDL_GLContext)m_context);
        m_context = nullptr;
        spdlog::info("Renderer shutdown");
    }
}

void Renderer::BeginFrame() {
    // TODO: Begin frame operations
}

void Renderer::EndFrame() {
    // TODO: End frame operations (swap buffers handled by window)
}

void Renderer::Clear(float r, float g, float b, float a) {
    glClearColor(r, g, b, a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

} // namespace rendering