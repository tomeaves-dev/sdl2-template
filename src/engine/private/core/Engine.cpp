#include "engine/public/core/Engine.h"
#include "engine/public/audio/AudioManager.h"
#include "engine/public/input/Input.h"
#include "engine/public/physics/Physics.h"
#include "engine/public/rendering/Renderer.h"
#include "engine/public/rendering/Window.h"
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
    
    // Initialize renderer (requires window)
    s_instance->m_renderer = std::make_unique<rendering::Renderer>();
    
    // Initialize text systems
    s_instance->m_fontManager = std::make_unique<text::FontManager>();
    if (!s_instance->m_fontManager->Initialize()) {
        spdlog::warn("Failed to initialize font manager - continuing without text rendering");
    }
    
    s_instance->m_textRenderer = std::make_unique<text::TextRenderer>();
    
    s_instance->m_initialized = true;
    spdlog::info("Engine initialized successfully!");
    
    return true;
}

void Engine::Shutdown() {
    if (!s_instance) {
        return;
    }
    
    spdlog::info("Shutting down Engine...");
    
    // Cleanup systems in reverse order of initialization
    s_instance->m_textRenderer.reset();
    s_instance->m_fontManager.reset();
    s_instance->m_renderer.reset();
    s_instance->m_physics.reset();
    s_instance->m_input.reset();
    s_instance->m_audioManager.reset();
    s_instance->m_resourceManager.reset();
    
    // Shutdown config system
    auto configPtr = utils::Config::Instance();
    configPtr->Shutdown();
    
    SDL_Quit();
    utils::Logger::Shutdown();
    
    delete s_instance;
    s_instance = nullptr;
    
    spdlog::info("Engine shutdown complete");
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

} // namespace core