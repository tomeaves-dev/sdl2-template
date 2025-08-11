#include "engine/public/core/Game.h"
#include "engine/public/core/Engine.h"
#include "engine/public/core/IGameApplication.h"
#include "engine/public/rendering/Window.h"
#include "engine/public/rendering/Renderer.h"
#include "engine/public/save/SaveManager.h"
#include "engine/public/utils/Logger.h"
#include "engine/public/utils/Config.h"
#include "engine/public/audio/AudioManager.h"
#include "engine/public/input/Input.h"
#include "engine/public/physics/Physics.h"
#include "engine/public/text/FontManager.h"
#include "engine/public/text/TextRenderer.h"

#include <SDL.h>
#include <spdlog/spdlog.h>

namespace core {

Game::Game() : m_isRunning(false) {
    spdlog::info("Game created");
}

Game::~Game() {
    Shutdown();
}

bool Game::Initialize() {
    spdlog::info("Initializing game systems...");
    
    // Initialize the Engine singleton
    if (!Engine::Initialize()) {
        spdlog::error("Failed to initialize Engine");
        return false;
    }
    
    // Create window
    m_window = std::make_unique<rendering::Window>();
    
    // Get window settings from config
    auto& config = Engine::Config();
    int width = config.GetInt("window.width", 1280);
    int height = config.GetInt("window.height", 720);
    std::string title = config.GetString("window.title", "SDL2 Game");
    
    spdlog::info("Creating window: {}x{} titled '{}'", width, height, title);
    
    if (!m_window->Initialize(title, width, height)) {
        spdlog::error("Failed to initialize window");
        return false;
    }
    
    // Initialize renderer with window
    if (!Engine::Renderer().Initialize(m_window.get())) {
        spdlog::error("Failed to initialize renderer");
        return false;
    }
    
    // Initialize text renderer
    if (!Engine::TextRenderer().Initialize(&Engine::Fonts(), &Engine::Renderer())) {
        spdlog::warn("Failed to initialize text renderer");
    }
    
    // Initialize SaveManager (not part of Engine singleton)
    m_saveManager = std::make_unique<save::SaveManager>();
    std::string gameName = config.GetString("window.title", "SDL2Game");
    m_saveManager->SetGameName(gameName);
    
    if (!m_saveManager->Initialize()) {
        spdlog::warn("Failed to initialize save manager - saves will not work");
    } else {
        spdlog::info("Save manager initialized - saves location: {}", m_saveManager->GetSaveDirectory());
    }
    
    // Load audio settings from config
    if (Engine::Audio().IsInitialized()) {
        float masterVolume = config.GetFloat("audio.master_volume", 1.0f);
        float musicVolume = config.GetFloat("audio.music_volume", 0.8f);
        float sfxVolume = config.GetFloat("audio.sfx_volume", 1.0f);
        float ambientVolume = config.GetFloat("audio.ambient_volume", 0.8f);
        bool muted = config.GetBool("audio.muted", false);
        
        Engine::Audio().SetMasterVolume(masterVolume);
        Engine::Audio().SetCategoryVolume(audio::AudioCategory::Music, musicVolume);
        Engine::Audio().SetCategoryVolume(audio::AudioCategory::SFX, sfxVolume);
        Engine::Audio().SetCategoryVolume(audio::AudioCategory::Ambient, ambientVolume);
        Engine::Audio().SetMuted(muted);
        
        spdlog::info("Audio initialized - Master: {:.1f}, Music: {:.1f}, SFX: {:.1f}, Ambient: {:.1f}, Muted: {}", 
                    masterVolume, musicVolume, sfxVolume, ambientVolume, muted);
    }
    
    // Load default font
    bool fontLoaded = false;
    std::string defaultFont = config.GetString("text.default_font", "arial.ttf");
    int defaultSize = config.GetInt("text.default_size", 16);
    
    if (Engine::Fonts().LoadFont(defaultFont, "default", defaultSize)) {
        fontLoaded = true;
    } else {
        // Try common system fonts as fallbacks
        std::vector<std::pair<std::string, std::string>> fallbacks = {
            {"Arial.ttf", "Arial"},
            {"arial.ttf", "Arial"},
            {"Helvetica.ttc", "Helvetica"},
            {"DejaVuSans.ttf", "DejaVu Sans"},
            {"LiberationSans-Regular.ttf", "Liberation Sans"},
            {"calibri.ttf", "Calibri"},
            {"tahoma.ttf", "Tahoma"}
        };
        
        for (const auto& [filename, displayName] : fallbacks) {
            if (Engine::Fonts().LoadFont(filename, "default", defaultSize)) {
                fontLoaded = true;
                break;
            }
        }
    }
    
    if (!fontLoaded) {
        spdlog::warn("Could not load any font - text rendering may not work");
    }
    
    // Create user's game application
    m_gameApp = CreateGameApplication();
    if (!m_gameApp) {
        spdlog::error("Failed to create game application");
        return false;
    }
    
    if (!m_gameApp->Initialize()) {
        spdlog::error("Failed to initialize game application");
        return false;
    }
    
    m_isRunning = true;
    spdlog::info("Game initialized successfully!");
    
    return true;
}

void Game::Run() {
    spdlog::info("Starting game loop...");
    
    auto& config = Engine::Config();
    bool vsync = config.GetBool("window.vsync", true);
    bool showFPS = config.GetBool("game.show_fps", false);
    
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
        
        // Shutdown user application first
        if (m_gameApp) {
            m_gameApp->Shutdown();
            m_gameApp.reset();
        }
        
        // Save configuration
        auto& config = Engine::Config();
        
        // Save current audio settings to config
        if (Engine::Audio().IsInitialized()) {
            config.SetFloat("audio.master_volume", Engine::Audio().GetMasterVolume());
            config.SetFloat("audio.music_volume", Engine::Audio().GetCategoryVolume(audio::AudioCategory::Music));
            config.SetFloat("audio.sfx_volume", Engine::Audio().GetCategoryVolume(audio::AudioCategory::SFX));
            config.SetFloat("audio.ambient_volume", Engine::Audio().GetCategoryVolume(audio::AudioCategory::Ambient));
            config.SetBool("audio.muted", Engine::Audio().IsMuted());
        }
        
        config.Save("assets/settings.json");
        
        // Cleanup local systems
        m_saveManager.reset();
        m_window.reset();
        
        // Shutdown the Engine singleton last
        Engine::Shutdown();
        
        spdlog::info("Game shutdown complete");
    }
}

void Game::Update(float deltaTime) {
    // Update input system
    Engine::Input().Update();
    
    // Update audio system (cleanup finished sounds)
    Engine::Audio().Update();
    
    // Update text renderer (cache management)
    Engine::TextRenderer().Update();
    
    // Fixed timestep accumulator with protection against spiral of death
    m_physicsAccumulator += deltaTime;
    int fixedSteps = 0;
    const int MAX_FIXED_STEPS = 5; // Prevent spiral of death
    
    // Run fixed timestep logic at consistent intervals
    while (m_physicsAccumulator >= FIXED_TIMESTEP && fixedSteps < MAX_FIXED_STEPS) {
        // 1. Engine physics fixed update
        Engine::Physics().FixedUpdate(FIXED_TIMESTEP);
        
        // 2. User's fixed update (physics-dependent logic)
        if (m_gameApp) {
            m_gameApp->FixedUpdate(FIXED_TIMESTEP);
        }
        
        m_physicsAccumulator -= FIXED_TIMESTEP;
        fixedSteps++;
    }
    
    // 3. User's variable update (frame-dependent logic)
    if (m_gameApp) {
        m_gameApp->Update(deltaTime);
    }
}

void Game::Render() {
    // Clear the screen with a nice blue color
    Engine::Renderer().Clear(0.2f, 0.3f, 0.4f, 1.0f);
    Engine::Renderer().BeginFrame();
    
    // User rendering
    if (m_gameApp) {
        m_gameApp->Render();
    }
    
    Engine::Renderer().EndFrame();
    
    // Present the frame
    if (m_window) {
        SDL_GL_SwapWindow(m_window->GetSDLWindow());
    }
}

void Game::HandleEvents() {
    auto& config = Engine::Config();
    
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
            bool showFPS = config.GetBool("game.show_fps", false);
            config.SetBool("game.show_fps", !showFPS);
            spdlog::info("FPS display {}", !showFPS ? "enabled" : "disabled");
        }
        
        // Example: Audio controls for testing
        if (Engine::Audio().IsInitialized()) {
            if (event.type == SDL_KEYDOWN) {
                switch (event.key.keysym.sym) {
                    case SDLK_m:
                        // Toggle mute
                        Engine::Audio().SetMuted(!Engine::Audio().IsMuted());
                        spdlog::info("Audio {}", Engine::Audio().IsMuted() ? "muted" : "unmuted");
                        break;
                    case SDLK_MINUS:
                        // Decrease master volume
                        {
                            float volume = std::max(0.0f, Engine::Audio().GetMasterVolume() - 0.1f);
                            Engine::Audio().SetMasterVolume(volume);
                            spdlog::info("Master volume: {:.1f}", volume);
                        }
                        break;
                    case SDLK_EQUALS:
                        // Increase master volume
                        {
                            float volume = std::min(1.0f, Engine::Audio().GetMasterVolume() + 0.1f);
                            Engine::Audio().SetMasterVolume(volume);
                            spdlog::info("Master volume: {:.1f}", volume);
                        }
                        break;
                }
            }
        }
        
        // Pass events to input system
        Engine::Input().HandleEvent(event);
    }
}

} // namespace core"}}]