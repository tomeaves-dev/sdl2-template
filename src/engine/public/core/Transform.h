#pragma once

#include <glm/glm.hpp>

namespace core {

    /**
     * Transform Component
     * 
     * Represents position, rotation, and scale in 2D space.
     * Separate from rendering - can be used by physics, sprites, UI, etc.
     */
    struct Transform {
        glm::vec2 position = {0.0f, 0.0f};
        float rotation = 0.0f;      // Rotation in degrees
        glm::vec2 scale = {1.0f, 1.0f};
        
        // Constructors
        Transform() = default;
        Transform(float x, float y) : position(x, y) {}
        Transform(const glm::vec2& pos) : position(pos) {}
        Transform(const glm::vec2& pos, float rot) : position(pos), rotation(rot) {}
        Transform(const glm::vec2& pos, float rot, const glm::vec2& scl) 
            : position(pos), rotation(rot), scale(scl) {}
        
        // Utility methods
        void SetPosition(float x, float y) { position = {x, y}; }
        void SetPosition(const glm::vec2& pos) { position = pos; }
        void SetRotation(float degrees) { rotation = degrees; }
        void SetScale(float uniformScale) { scale = {uniformScale, uniformScale}; }
        void SetScale(float x, float y) { scale = {x, y}; }
        void SetScale(const glm::vec2& scl) { scale = scl; }
        
        // Movement helpers
        void Translate(float deltaX, float deltaY) { position.x += deltaX; position.y += deltaY; }
        void Translate(const glm::vec2& delta) { position += delta; }
        void Rotate(float deltaDegrees) { rotation += deltaDegrees; }
        
        // Getters
        const glm::vec2& GetPosition() const { return position; }
        float GetRotation() const { return rotation; }
        const glm::vec2& GetScale() const { return scale; }
        
        // Get rotation in radians for calculations
        float GetRotationRadians() const { return glm::radians(rotation); }
    };

} // namespace core