#pragma once

#include <memory>

// Forward declarations
namespace rendering {
    class Window;
    class Renderer;
}

namespace physics {
    class Physics;
}

namespace input {
    class Input;
}

namespace audio {
    class AudioManager;
}

namespace core {
    
    class Game {
    public:
        Game();
        ~Game();
        
        bool Initialize();
        void Run();
        void Shutdown();
        
        bool IsRunning() const { return m_isRunning; }
        
        // System access (for advanced usage)
        audio::AudioManager* GetAudioManager() const { return m_audioManager.get(); }
        physics::Physics* GetPhysics() const { return m_physics.get(); }
        
    private:
        void Update(float deltaTime);
        void Render();
        void HandleEvents();
        
        bool m_isRunning;
        
        // Core systems
        std::unique_ptr<rendering::Window> m_window;
        std::unique_ptr<rendering::Renderer> m_renderer;
        std::unique_ptr<physics::Physics> m_physics;
        std::unique_ptr<input::Input> m_input;
        std::unique_ptr<audio::AudioManager> m_audioManager;
    };
}