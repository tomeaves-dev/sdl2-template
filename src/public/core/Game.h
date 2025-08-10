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

namespace core {
    
    class Game {
    public:
        Game();
        ~Game();
        
        bool Initialize();
        void Run();
        void Shutdown();
        
        bool IsRunning() const { return m_isRunning; }
        
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
    };
}