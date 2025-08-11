#pragma once

#include <memory>

namespace audio { class AudioManager; }
namespace input { class Input; }
namespace physics { class Physics; }
namespace rendering { class Renderer; }
namespace text { class FontManager; class TextRenderer; }
namespace utils { class Config; class ResourceManager; }

namespace core {
    class Engine {
    public:
        // Lifecycle
        static bool Initialize();
        static void Shutdown();
        static bool IsInitialized();
        
        // System access - Clean static API
        static audio::AudioManager& Audio();
        static input::Input& Input();
        static physics::Physics& Physics();
        static rendering::Renderer& Renderer();
        static text::FontManager& Fonts();
        static text::TextRenderer& TextRenderer();
        static utils::Config& Config();
        static utils::ResourceManager& Resources();
        
        // Internal access (for engine code only)
        static Engine& Instance();
        
    private:
        Engine() = default;
        ~Engine() = default;
        Engine(const Engine&) = delete;
        Engine& operator=(const Engine&) = delete;
        
        static Engine* s_instance;
        
        // System instances (unique_ptr for proper cleanup)
        std::unique_ptr<audio::AudioManager> m_audioManager;
        std::unique_ptr<input::Input> m_input;
        std::unique_ptr<physics::Physics> m_physics;
        std::unique_ptr<rendering::Renderer> m_renderer;
        std::unique_ptr<text::FontManager> m_fontManager;
        std::unique_ptr<text::TextRenderer> m_textRenderer;
        std::unique_ptr<utils::ResourceManager> m_resourceManager;
        
        bool m_initialized = false;
    };
}