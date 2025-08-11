#include "public/core/Game.h"
#include "public/rendering/Window.h"
#include "public/rendering/Renderer.h"
#include "public/physics/Physics.h"
#include "public/input/Input.h"
#include "public/audio/AudioManager.h"
#include "public/utils/Logger.h"
#include "public/utils/Config.h"

#include <SDL.h>

namespace core {

Game::Game() : m_isRunning(false) {
    utils::Logger::Initialize();
    spdlog::info("Game created");
}

Game::~Game() {
    Shutdown();
    // Logger shutdown moved to main to avoid order issues
}

bool Game::Initialize() {
    spdlog::info("Initializing game systems...");
    
    // Load configuration first
    auto config = utils::Config::Instance();
    if (!config->Load("assets/settings.json")) {
        spdlog::warn("Failed to load config, using defaults");
    }
    
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0) {
        spdlog::error("Failed to initialize SDL: {}", SDL_GetError());
        return false;
    }
    
    // Create and initialize systems using config values
    m_window = std::make_unique<rendering::Window>();
    
    // Get window settings from config
    int width = config->GetInt("window.width", 1280);
    int height = config->GetInt("window.height", 720);
    std::string title = config->GetString("window.title", "SDL2 Game");
    
    spdlog::info("Creating window: {}x{} titled '{}'", width, height, title);
    
    if (!m_window->Initialize(title, width, height)) {
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
    
    // Initialize audio manager
    m_audioManager = std::make_unique<audio::AudioManager>();
    if (!m_audioManager->Initialize()) {
        spdlog::warn("Failed to initialize audio manager - continuing without audio");
    } else {
        // Load audio settings from config
        float masterVolume = config->GetFloat("audio.master_volume", 1.0f);
        float musicVolume = config->GetFloat("audio.music_volume", 0.8f);
        float sfxVolume = config->GetFloat("audio.sfx_volume", 1.0f);
        float ambientVolume = config->GetFloat("audio.ambient_volume", 0.8f);
        bool muted = config->GetBool("audio.muted", false);
        
        m_audioManager->SetMasterVolume(masterVolume);
        m_audioManager->SetCategoryVolume(audio::AudioCategory::Music, musicVolume);
        m_audioManager->SetCategoryVolume(audio::AudioCategory::SFX, sfxVolume);
        m_audioManager->SetCategoryVolume(audio::AudioCategory::Ambient, ambientVolume);
        m_audioManager->SetMuted(muted);
        
        spdlog::info("Audio initialized - Master: {:.1f}, Music: {:.1f}, SFX: {:.1f}, Ambient: {:.1f}, Muted: {}", 
                    masterVolume, musicVolume, sfxVolume, ambientVolume, muted);
    }
    
    m_isRunning = true;
    spdlog::info("Game initialized successfully!");
    spdlog::info("Configuration loaded from: {}", config->GetConfigPath());
    
    // Log some config values to verify they're loaded
    spdlog::info("Window config - Width: {}, Height: {}, Fullscreen: {}", 
                width, height, config->GetBool("window.fullscreen", false));
    spdlog::info("Graphics config - VSync: {}, AA: {}", 
                config->GetBool("window.vsync", true), 
                config->GetBool("graphics.antialiasing", true));
    
    return true;
}

void Game::Run() {
    spdlog::info("Starting game loop...");
    
    auto config = utils::Config::Instance();
    bool vsync = config->GetBool("window.vsync", true);
    bool showFPS = config->GetBool("game.show_fps", false);
    
    Uint64 lastTime = SDL_GetPerformanceCounter();
    const float targetFPS = 60.0f;
    const float targetFrameTime = 1.0f / targetFPS;
    
    // FPS tracking
    Uint64 fpsTimer = 0;
    int frameCount = 0;
    float currentFPS = 0.0f;
    
    while (m_isRunning) {
        // Calculate delta time
        Uint64 currentTime = SDL_GetPerformanceCounter();
        float deltaTime = (float)(currentTime - lastTime) / SDL_GetPerformanceFrequency();
        lastTime = currentTime;
        
        // Cap delta time to prevent spiral of death
        if (deltaTime > 0.05f) {
            deltaTime = 0.05f;
        }
        
        // FPS calculation
        if (showFPS) {
            frameCount++;
            fpsTimer += (Uint64)(deltaTime * SDL_GetPerformanceFrequency());
            
            if (fpsTimer >= SDL_GetPerformanceFrequency()) { // 1 second
                currentFPS = frameCount;
                frameCount = 0;
                fpsTimer = 0;
                spdlog::info("FPS: {:.1f}", currentFPS);
            }
        }
        
        HandleEvents();
        Update(deltaTime);
        Render();
        
        // Frame rate limiting (only if vsync is disabled)
        if (!vsync) {
            float frameTime = (float)(SDL_GetPerformanceCounter() - currentTime) / SDL_GetPerformanceFrequency();
            if (frameTime < targetFrameTime) {
                SDL_Delay((Uint32)((targetFrameTime - frameTime) * 1000.0f));
            }
        }
    }
    
    spdlog::info("Game loop ended");
}

void Game::Shutdown() {
    if (m_isRunning) {
        spdlog::info("Shutting down game systems...");
        m_isRunning = false;
        
        // Save configuration first (while everything is still valid)
        auto config = utils::Config::Instance();
        
        // Save current audio settings to config
        if (m_audioManager && m_audioManager->IsInitialized()) {
            config->SetFloat("audio.master_volume", m_audioManager->GetMasterVolume());
            config->SetFloat("audio.music_volume", m_audioManager->GetCategoryVolume(audio::AudioCategory::Music));
            config->SetFloat("audio.sfx_volume", m_audioManager->GetCategoryVolume(audio::AudioCategory::SFX));
            config->SetFloat("audio.ambient_volume", m_audioManager->GetCategoryVolume(audio::AudioCategory::Ambient));
            config->SetBool("audio.muted", m_audioManager->IsMuted());
        }
        
        config->Save("assets/settings.json");
        
        // Cleanup systems in reverse order of initialization
        m_audioManager.reset();
        m_input.reset();
        m_physics.reset();
        m_renderer.reset();
        m_window.reset();
        
        // Shutdown config system
        config->Shutdown();
        
        SDL_Quit();
        
        spdlog::info("Game shutdown complete");
    }
}

void Game::Update(float deltaTime) {
    // Update input system
    if (m_input) {
        m_input->Update();
    }
    
    // Update audio system (cleanup finished sounds)
    if (m_audioManager) {
        m_audioManager->Update();
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
    auto config = utils::Config::Instance();
    
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
        
        // Example: Toggle FPS display with F1
        if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_F1) {
            bool showFPS = config->GetBool("game.show_fps", false);
            config->SetBool("game.show_fps", !showFPS);
            spdlog::info("FPS display {}", !showFPS ? "enabled" : "disabled");
        }
        
        // Example: Audio controls for testing
        if (m_audioManager && m_audioManager->IsInitialized()) {
            if (event.type == SDL_KEYDOWN) {
                switch (event.key.keysym.sym) {
                    case SDLK_m:
                        // Toggle mute
                        m_audioManager->SetMuted(!m_audioManager->IsMuted());
                        spdlog::info("Audio {}", m_audioManager->IsMuted() ? "muted" : "unmuted");
                        break;
                    case SDLK_MINUS:
                        // Decrease master volume
                        {
                            float volume = std::max(0.0f, m_audioManager->GetMasterVolume() - 0.1f);
                            m_audioManager->SetMasterVolume(volume);
                            spdlog::info("Master volume: {:.1f}", volume);
                        }
                        break;
                    case SDLK_EQUALS:
                        // Increase master volume
                        {
                            float volume = std::min(1.0f, m_audioManager->GetMasterVolume() + 0.1f);
                            m_audioManager->SetMasterVolume(volume);
                            spdlog::info("Master volume: {:.1f}", volume);
                        }
                        break;
                }
            }
        }
        
        // Pass events to input system
        if (m_input) {
            m_input->HandleEvent(event);
        }
        
        // TODO: Handle other game-specific events here
    }
}

} // namespace core