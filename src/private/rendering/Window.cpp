#include "public/rendering/Window.h"
#include <spdlog/spdlog.h>

namespace rendering {

Window::Window() : m_window(nullptr), m_width(0), m_height(0) {
}

Window::~Window() {
    Shutdown();
}

bool Window::Initialize(const std::string& title, int width, int height) {
    spdlog::info("Initializing window: {}x{}", width, height);
    
    m_width = width;
    m_height = height;
    
    // Create SDL window
    m_window = SDL_CreateWindow(
        title.c_str(),
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        width, height,
        SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN
    );
    
    if (!m_window) {
        spdlog::error("Failed to create window: {}", SDL_GetError());
        return false;
    }
    
    spdlog::info("Window initialized successfully");
    return true;
}

void Window::Shutdown() {
    if (m_window) {
        SDL_DestroyWindow(m_window);
        m_window = nullptr;
        spdlog::info("Window shutdown");
    }
}

} // namespace rendering