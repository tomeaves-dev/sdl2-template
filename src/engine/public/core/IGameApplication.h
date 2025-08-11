#pragma once

#include <memory>

namespace core {
    class IGameApplication {
    public:
        virtual ~IGameApplication() = default;
        
        // User lifecycle hooks
        virtual bool Initialize() = 0;
        virtual void Update(float deltaTime) = 0;        // Variable timestep (every frame)
        virtual void FixedUpdate(float fixedDeltaTime) = 0;  // Fixed timestep (consistent interval)
        virtual void Render() = 0;
        virtual void Shutdown() = 0;
    };
    
    // User must implement this factory function
    extern std::unique_ptr<IGameApplication> CreateGameApplication();
}