#pragma once

#include <box2d/box2d.h>

namespace physics {
    class Physics {
    public:
        Physics();
        ~Physics();
        
        bool Initialize();
        void Shutdown();
        
        void Step(float deltaTime);
        
        void SetGravity(float x, float y);
        void GetGravity(float& x, float& y) const;
        void DisableGravity() { SetGravity(0.0f, 0.0f); }
        
        // World access (for creating bodies, joints, etc.)
        b2WorldId GetWorldId() const { return m_worldId; }
        
    private:
        b2WorldId m_worldId;
        bool m_initialized;
    };
}