#include "engine/public/core/Engine.h"
#include "engine/public/core/IGameApplication.h"
#include "engine/public/audio/AudioManager.h"
#include "engine/public/input/Input.h"
#include "engine/public/physics/Physics.h"
#include "engine/public/rendering/Renderer.h"
#include "engine/public/rendering/Window.h"
#include "engine/public/save/SaveManager.h"
#include "engine/public/text/FontManager.h"
#include "engine/public/text/TextRenderer.h"
#include "engine/public/utils/Config.h"
#include "engine/public/utils/Logger.h"
#include "engine/public/utils/ResourceManager.h"

#include <SDL.h>
#include <spdlog/spdlog.h>
#include <memory>
#include <cassert>

namespace core {

Engine* Engine::s_instance = nullptr;

bool Engine::Initialize() {
    if (s_instance) {
        spdlog::warn("Engine already initialized");
        return true;
    }
    
    spdlog::info("Initializing Engine...");
    
    s_instance = new Engine();
    
    // Initialize SDL first
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0) {
        spdlog::error("Failed to initialize SDL: {}", SDL_GetError());
        return false;
    }
    
    // Initialize logger
    utils::Logger::Initialize();
    
    // Initialize config system
    auto configPtr = utils::Config::Instance();
    if (!configPtr->Load("assets/settings.json")) {
        spdlog::warn("Failed to load config, using defaults");
    }
    
    // Initialize ResourceManager
    s_instance->m_resourceManager = std::make_unique<utils::ResourceManager>();
    if (!s_instance->m_resourceManager->Initialize()) {
        spdlog::error("Failed to initialize ResourceManager");
        return false;
    }
    
    // Initialize audio manager
    s_instance->m_audioManager = std::make_unique<audio::AudioManager>();
    if (!s_instance->m_audioManager->Initialize()) {
        spdlog::warn("Failed to initialize audio manager - continuing without audio");
    }
    
    // Initialize input system
    s_instance->m_input = std::make_unique<input::Input>();
    
    // Initialize physics system
    s_instance->m_physics = std::make_unique<physics::Physics>();
    if (!s_instance->m_physics->Initialize()) {
        spdlog::error("Failed to initialize physics");
        return false;
    }
    
    // Create window
    s_instance->m_window = std::make_unique<rendering::Window>();
    
    // Get window settings from config
    auto& config = *utils::Config::Instance();
    int width = config.GetInt("window.width", 1280);
    int height = config.GetInt("window.height", 720);
    std::string title = config.GetString("window.title", "SDL2 Game");
    
    spdlog::info("Creating window: {}x{} titled '{}'", width, height, title);
    
    if (!s_instance->m_window->Initialize(title, width, height)) {
        spdlog::error("Failed to initialize window");
        return false;
    }
    
    // Initialize renderer (requires window)
    s_instance->m_renderer = std::make_unique<rendering::Renderer>();
    if (!s_instance->m_renderer->Initialize(s_instance->m_window.get())) {
        spdlog::error("Failed to initialize renderer");
        return false;
    }
    
    // Initialize text systems
    s_instance->m_fontManager = std::make_unique<text::FontManager>();
    if (!s_instance->m_fontManager->Initialize()) {
        spdlog::warn("Failed to initialize font manager - continuing without text rendering");
    }
    
    s_instance->m_textRenderer = std::make_unique<text::TextRenderer>();
    if (!s_instance->m_textRenderer->Initialize(s_instance->m_fontManager.get(), s_instance->m_renderer.get())) {
        spdlog::warn("Failed to initialize text renderer");
    }
    
    // Initialize SaveManager
    s_instance->m_saveManager = std::make_unique<save::SaveManager>();
    std::string gameName = config.GetString("window.title", "SDL2Game");
    s_instance->m_saveManager->SetGameName(gameName);
    
    if (!s_instance->m_saveManager->Initialize()) {
        spdlog::warn("Failed to initialize save manager - saves will not work");
    } else {
        spdlog::info("Save manager initialized - saves location: {}", s_instance->m_saveManager->GetSaveDirectory());
    }
    
    // Load audio settings from config
    if (s_instance->m_audioManager->IsInitialized()) {
        float masterVolume = config.GetFloat("audio.master_volume", 1.0f);
        float musicVolume = config.GetFloat("audio.music_volume", 0.8f);
        float sfxVolume = config.GetFloat("audio.sfx_volume", 1.0f);
        float ambientVolume = config.GetFloat("audio.ambient_volume", 0.8f);
        bool muted = config.GetBool("audio.muted", false);
        
        s_instance->m_audioManager->SetMasterVolume(masterVolume);
        s_instance->m_audioManager->SetCategoryVolume(audio::AudioCategory::Music, musicVolume);
        s_instance->m_audioManager->SetCategoryVolume(audio::AudioCategory::SFX, sfxVolume);
        s_instance->m_audioManager->SetCategoryVolume(audio::AudioCategory::Ambient, ambientVolume);
        s_instance->m_audioManager->SetMuted(muted);
        
        spdlog::info("Audio initialized - Master: {:.1f}, Music: {:.1f}, SFX: {:.1f}, Ambient: {:.1f}, Muted: {}", 
                    masterVolume, musicVolume, sfxVolume, ambientVolume, muted);
    }
    
    // Load default font
    bool fontLoaded = false;
    std::string defaultFont = config.GetString("text.default_font", "arial.ttf");
    int defaultSize = config.GetInt("text.default_size", 16);
    
    if (s_instance->m_fontManager->LoadFont(defaultFont, "default", defaultSize)) {
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
            if (s_instance->m_fontManager->LoadFont(filename, "default", defaultSize)) {
                fontLoaded = true;
                break;
            }
        }
    }
    
    if (!fontLoaded) {
        spdlog::warn("Could not load any font - text rendering may not work");
    }
    
    // Load fixed timestep from configuration
    s_instance->m_fixedTimestep = config.GetFloat("physics.fixed_timestep", 1.0f / 60.0f);
    s_instance->m_maxFixedSteps = config.GetInt("physics.max_fixed_steps", 5);
    
    // Create user's game application
    s_instance->m_gameApp = CreateGameApplication();
    if (!s_instance->m_gameApp) {
        spdlog::error("Failed to create game application");
        return false;
    }
    
    if (!s_instance->m_gameApp->Initialize()) {
        spdlog::error("Failed to initialize game application");
        return false;
    }
    
    s_instance->m_initialized = true;
    spdlog::info("Engine initialized successfully!");
    
    return true;
}

void Engine::Shutdown() {
    if (!s_instance) {
        return;
    }
    
    spdlog::info("Shutting down Engine...");
    
    // Shutdown user application first
    if (s_instance->m_gameApp) {
        s_instance->m_gameApp->Shutdown();
        s_instance->m_gameApp.reset();
    }
    
    // Save configuration
    auto& config = *utils::Config::Instance();
    
    // Save current audio settings to config
    if (s_instance->m_audioManager && s_instance->m_audioManager->IsInitialized()) {
        config.SetFloat("audio.master_volume", s_instance->m_audioManager->GetMasterVolume());
        config.SetFloat("audio.music_volume", s_instance->m_audioManager->GetCategoryVolume(audio::AudioCategory::Music));
        config.SetFloat("audio.sfx_volume", s_instance->m_audioManager->GetCategoryVolume(audio::AudioCategory::SFX));
        config.SetFloat("audio.ambient_volume", s_instance->m_audioManager->GetCategoryVolume(audio::AudioCategory::Ambient));
        config.SetBool("audio.muted", s_instance->m_audioManager->IsMuted());
    }
    
    config.Save("assets/settings.json");
    
    // Cleanup systems in reverse order of initialization
    s_instance->m_textRenderer.reset();
    s_instance->m_fontManager.reset();
    s_instance->m_renderer.reset();
    s_instance->m_saveManager.reset();
    s_instance->m_window.reset();
    s_instance->m_physics.reset();
    s_instance->m_input.reset();
    s_instance->m_audioManager.reset();
    s_instance->m_resourceManager.reset();
    
    spdlog::info("Engine shutdown complete");
    
    // Shutdown config system after final log message
    auto configPtr = utils::Config::Instance();
    configPtr->Shutdown();
    
    SDL_Quit();
    utils::Logger::Shutdown();
    
    delete s_instance;
    s_instance = nullptr;
}

bool Engine::IsInitialized() {
    return s_instance && s_instance->m_initialized;
}

Engine& Engine::Instance() {
    assert(s_instance && "Engine not initialized");
    return *s_instance;
}

// System access methods
audio::AudioManager& Engine::Audio() {
    assert(s_instance && s_instance->m_audioManager && "Engine not initialized or AudioManager not available");
    return *s_instance->m_audioManager;
}

input::Input& Engine::Input() {
    assert(s_instance && s_instance->m_input && "Engine not initialized or Input not available");
    return *s_instance->m_input;
}

utils::Logger& Engine::Logger() {
    static utils::Logger logger;
    return logger;
}

physics::Physics& Engine::Physics() {
    assert(s_instance && s_instance->m_physics && "Engine not initialized or Physics not available");
    return *s_instance->m_physics;
}

rendering::Renderer& Engine::Renderer() {
    assert(s_instance && s_instance->m_renderer && "Engine not initialized or Renderer not available");
    return *s_instance->m_renderer;
}

text::FontManager& Engine::Fonts() {
    assert(s_instance && s_instance->m_fontManager && "Engine not initialized or FontManager not available");
    return *s_instance->m_fontManager;
}

text::TextRenderer& Engine::TextRenderer() {
    assert(s_instance && s_instance->m_textRenderer && "Engine not initialized or TextRenderer not available");
    return *s_instance->m_textRenderer;
}

utils::Config& Engine::Config() {
    auto configPtr = utils::Config::Instance();
    assert(configPtr && "Config system not available");
    return *configPtr;
}

utils::ResourceManager& Engine::Resources() {
    assert(s_instance && s_instance->m_resourceManager && "Engine not initialized or ResourceManager not available");
    return *s_instance->m_resourceManager;
}

rendering::Window& Engine::Window() {
    assert(s_instance && s_instance->m_window && "Engine not initialized or Window not available");
    return *s_instance->m_window;
}

save::SaveManager& Engine::SaveManager() {
    assert(s_instance && s_instance->m_saveManager && "Engine not initialized or SaveManager not available");
    return *s_instance->m_saveManager;
}

void Engine::Run() {
    if (!s_instance || !s_instance->m_initialized) {
        spdlog::error("Engine not initialized");
        return;
    }
    
    spdlog::info("Starting game loop...");
    
    auto& config = *utils::Config::Instance();
    bool vsync = config.GetBool("window.vsync", true);
    bool showFPS = config.GetBool("game.show_fps", false);
    
    Uint64 lastTime = SDL_GetPerformanceCounter();
    const float targetFPS = 60.0f;
    const float targetFrameTime = 1.0f / targetFPS;
    
    // FPS tracking
    Uint64 fpsTimer = 0;
    int frameCount = 0;
    float currentFPS = 0.0f;
    
    s_instance->m_isRunning = true;
    
    while (s_instance->m_isRunning) {
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
        
        s_instance->HandleEvents();
        s_instance->Update(deltaTime);
        s_instance->Render();
        
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

void Engine::Update(float deltaTime) {
    // Update input system
    s_instance->m_input->Update();
    
    // Update audio system (cleanup finished sounds)
    s_instance->m_audioManager->Update();
    
    // Update text renderer (cache management)
    s_instance->m_textRenderer->Update();
    
    // Fixed timestep accumulator with protection against spiral of death
    s_instance->m_physicsAccumulator += deltaTime;
    int fixedSteps = 0;
    
    // Run fixed timestep logic at consistent intervals
    while (s_instance->m_physicsAccumulator >= s_instance->m_fixedTimestep && fixedSteps < s_instance->m_maxFixedSteps) {
        // 1. Engine physics fixed update
        s_instance->m_physics->FixedUpdate(s_instance->m_fixedTimestep);
        
        // 2. User's fixed update (physics-dependent logic)
        if (s_instance->m_gameApp) {
            s_instance->m_gameApp->FixedUpdate(s_instance->m_fixedTimestep);
        }
        
        s_instance->m_physicsAccumulator -= s_instance->m_fixedTimestep;
        fixedSteps++;
    }
    
    // 3. User's variable update (frame-dependent logic)
    if (s_instance->m_gameApp) {
        s_instance->m_gameApp->Update(deltaTime);
    }
}

void Engine::Render() {
    // Clear the screen with a nice blue color
    s_instance->m_renderer->Clear(0.2f, 0.3f, 0.4f, 1.0f);
    s_instance->m_renderer->BeginFrame();
    
    // User rendering
    if (s_instance->m_gameApp) {
        s_instance->m_gameApp->Render();
    }
    
    s_instance->m_renderer->EndFrame();
    
    // Present the frame
    if (s_instance->m_window) {
        SDL_GL_SwapWindow(s_instance->m_window->GetSDLWindow());
    }
}

void Engine::HandleEvents() {
    auto& config = *utils::Config::Instance();
    
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        // Handle window close
        if (event.type == SDL_QUIT) {
            s_instance->m_isRunning = false;
        }
        
        // Handle escape key to quit
        if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) {
            s_instance->m_isRunning = false;
        }
        
        // Example: Toggle FPS display with F1
        if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_F1) {
            bool showFPS = config.GetBool("game.show_fps", false);
            config.SetBool("game.show_fps", !showFPS);
            spdlog::info("FPS display {}", !showFPS ? "enabled" : "disabled");
        }
        
        // Example: Audio controls for testing
        if (s_instance->m_audioManager->IsInitialized()) {
            if (event.type == SDL_KEYDOWN) {
                switch (event.key.keysym.sym) {
                    case SDLK_m:
                        // Toggle mute
                        s_instance->m_audioManager->SetMuted(!s_instance->m_audioManager->IsMuted());
                        spdlog::info("Audio {}", s_instance->m_audioManager->IsMuted() ? "muted" : "unmuted");
                        break;
                    case SDLK_MINUS:
                        // Decrease master volume
                        {
                            float volume = std::max(0.0f, s_instance->m_audioManager->GetMasterVolume() - 0.1f);
                            s_instance->m_audioManager->SetMasterVolume(volume);
                            spdlog::info("Master volume: {:.1f}", volume);
                        }
                        break;
                    case SDLK_EQUALS:
                        // Increase master volume
                        {
                            float volume = std::min(1.0f, s_instance->m_audioManager->GetMasterVolume() + 0.1f);
                            s_instance->m_audioManager->SetMasterVolume(volume);
                            spdlog::info("Master volume: {:.1f}", volume);
                        }
                        break;
                }
            }
        }
        
        // Pass events to input system
        s_instance->m_input->HandleEvent(event);
    }
}

} // namespace core