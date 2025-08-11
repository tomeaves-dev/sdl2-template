#pragma once

#include <memory>

namespace audio { class AudioManager; }
namespace input { class Input; }
namespace physics { class Physics; }
namespace rendering { class Renderer; class Window; }
namespace save { class SaveManager; }
namespace text { class FontManager; class TextRenderer; }
namespace utils { class Config; class ResourceManager; class Logger; }

namespace core {
    class IGameApplication;
    
    class Engine {
    public:
        // Lifecycle
        static bool Initialize();
        static void Run();
        static void Shutdown();
        static bool IsInitialized();
        
        // System access - Clean static API
        static audio::AudioManager& Audio();
        static input::Input& Input();
        static utils::Logger& Logger();
        static physics::Physics& Physics();
        static rendering::Renderer& Renderer();
        static rendering::Window& Window();
        static save::SaveManager& SaveManager();
        static text::FontManager& Fonts();
        static text::TextRenderer& TextRenderer();
        static utils::Config& Config();
        static utils::ResourceManager& Resources();
        
        // Internal access (for engine code only)
        static Engine& Instance();
        
    private:
        // Game loop methods
        void Update(float deltaTime);
        void Render();
        void HandleEvents();
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
        std::unique_ptr<rendering::Window> m_window;
        std::unique_ptr<save::SaveManager> m_saveManager;
        std::unique_ptr<text::FontManager> m_fontManager;
        std::unique_ptr<text::TextRenderer> m_textRenderer;
        std::unique_ptr<utils::ResourceManager> m_resourceManager;
        
        bool m_initialized = false;
        
        // Game loop state
        bool m_isRunning = false;
        float m_fixedTimestep = 1.0f / 60.0f;  // Default 60Hz
        float m_physicsAccumulator = 0.0f;
        int m_maxFixedSteps = 5;  // Prevent spiral of death
        std::unique_ptr<IGameApplication> m_gameApp;
    };
}