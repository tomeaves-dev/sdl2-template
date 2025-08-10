#include "public/core/Game.h"
#include "public/rendering/Window.h"
#include "public/rendering/Renderer.h"
#include "public/physics/Physics.h"
#include "public/input/Input.h"
#include "public/utils/Logger.h"

#include <SDL.h>

namespace core {

Game::Game() : m_isRunning(false) {
    utils::Logger::Initialize();
    spdlog::info("Game created");
}

Game::~Game() {
    Shutdown();
    utils::Logger::Shutdown();
}

bool Game::Initialize() {
    spdlog::info("Initializing game systems...");
    
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0) {
        spdlog::error("Failed to initialize SDL: {}", SDL_GetError());
        return false;
    }
    
    // Create and initialize systems in order
    m_window = std::make_unique<rendering::Window>();
    if (!m_window->Initialize("SDL2 Game Template", 1280, 720)) {
        spdlog::error("Failed to initialize window");
        return false;
    }
    
    m_renderer = std::make_unique<rendering::Renderer>();
    if (!m_renderer->Initialize(m_window.get())) {
        spdlog::error("Failed to initialize renderer");
        return false;
    }
    
    m_physics = std::make_unique<physics::Physics>();
    if (!m_physics->Initialize()) {
        spdlog::error("Failed to initialize physics");
        return false;
    }
    
    m_input = std::make_unique<input::Input>();
    
    m_isRunning = true;
    spdlog::info("Game initialized successfully!");
    return true;
}

void Game::Run() {
    spdlog::info("Starting game loop...");
    
    Uint64 lastTime = SDL_GetPerformanceCounter();
    const float targetFPS = 60.0f;
    const float targetFrameTime = 1.0f / targetFPS;
    
    while (m_isRunning) {
        // Calculate delta time
        Uint64 currentTime = SDL_GetPerformanceCounter();
        float deltaTime = (float)(currentTime - lastTime) / SDL_GetPerformanceFrequency();
        lastTime = currentTime;
        
        // Cap delta time to prevent spiral of death
        if (deltaTime > 0.05f) {
            deltaTime = 0.05f;
        }
        
        HandleEvents();
        Update(deltaTime);
        Render();
        
        // Simple frame rate limiting
        float frameTime = (float)(SDL_GetPerformanceCounter() - currentTime) / SDL_GetPerformanceFrequency();
        if (frameTime < targetFrameTime) {
            SDL_Delay((Uint32)((targetFrameTime - frameTime) * 1000.0f));
        }
    }
    
    spdlog::info("Game loop ended");
}

void Game::Shutdown() {
    if (m_isRunning) {
        spdlog::info("Shutting down game systems...");
        m_isRunning = false;
        
        // Cleanup systems in reverse order of initialization
        m_input.reset();
        m_physics.reset();
        m_renderer.reset();
        m_window.reset();
        
        SDL_Quit();
        spdlog::info("Game shutdown complete");
    }
}

void Game::Update(float deltaTime) {
    // Update input system
    if (m_input) {
        m_input->Update();
    }
    
    // Step physics simulation
    if (m_physics) {
        m_physics->Step(deltaTime);
    }
    
    // TODO: Add your game logic here
    // Example: Update game objects, handle collisions, etc.
}

void Game::Render() {
    if (!m_renderer) return;
    
    // Clear the screen with a nice blue color
    m_renderer->Clear(0.2f, 0.3f, 0.4f, 1.0f);
    m_renderer->BeginFrame();
    
    // TODO: Add your rendering code here
    // Example: Render sprites, UI, particles, etc.
    
    m_renderer->EndFrame();
    
    // Present the frame
    if (m_window) {
        SDL_GL_SwapWindow(m_window->GetSDLWindow());
    }
}

void Game::HandleEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        // Handle window close
        if (event.type == SDL_QUIT) {
            m_isRunning = false;
        }
        
        // Handle escape key to quit
        if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) {
            m_isRunning = false;
        }
        
        // Pass events to input system
        if (m_input) {
            m_input->HandleEvent(event);
        }
        
        // TODO: Handle other game-specific events here
    }
}

} // namespace core