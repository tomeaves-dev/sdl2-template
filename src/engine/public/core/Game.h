#pragma once

#include <memory>

namespace rendering {
    class Window;
}

namespace save {
    class SaveManager;
}

namespace core {
    class IGameApplication;
    
    // Internal engine game loop class
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
        
        // Fixed timestep for physics
        static constexpr float FIXED_TIMESTEP = 1.0f / 60.0f;
        float m_physicsAccumulator = 0.0f;
        
        // Window management (renderer handled by Engine singleton)
        std::unique_ptr<rendering::Window> m_window;
        
        // Save system (not part of Engine singleton)
        std::unique_ptr<save::SaveManager> m_saveManager;
        
        // User's game application
        std::unique_ptr<IGameApplication> m_gameApp;
    };
}