#pragma once

#include "engine/public/core/Transform.h"
#include <box2d/box2d.h>
#include <glm/glm.hpp>

namespace physics {
    
    class RigidBody {
    public:
        enum class Type { Static, Kinematic, Dynamic };
        
        RigidBody(const core::Transform& initialTransform, Type type);
        ~RigidBody();
        
        // Manual sync methods (user calls these)
        void SetPosition(const glm::vec2& position);
        void SetRotation(float degrees);
        glm::vec2 GetPosition() const;
        float GetRotation() const;
        
        // Physics control
        void ApplyForce(const glm::vec2& force);
        void ApplyImpulse(const glm::vec2& impulse);
        void SetVelocity(const glm::vec2& velocity);
        glm::vec2 GetVelocity() const;
        
        // Body properties
        void SetDensity(float density);
        void SetFriction(float friction);
        void SetRestitution(float restitution);
        
        // Collision detection
        void AddBoxCollider(float width, float height, const glm::vec2& offset = {0, 0});
        void AddCircleCollider(float radius, const glm::vec2& offset = {0, 0});
        
        // Utility
        bool IsValid() const;
        Type GetType() const { return m_type; }
        
    private:
        b2BodyId m_bodyId;
        Type m_type;
        bool m_valid = false;
    };
    
} // namespace physics