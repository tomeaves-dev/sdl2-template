#include "engine/public/physics/RigidBody.h"
#include "engine/public/core/Engine.h"
#include "engine/public/physics/Physics.h"
#include <spdlog/spdlog.h>
#include <glm/gtc/constants.hpp>

namespace physics {

RigidBody::RigidBody(const core::Transform& initialTransform, Type type) : m_type(type) {
    // Create body definition
    b2BodyDef bodyDef = b2DefaultBodyDef();
    bodyDef.position = {initialTransform.position.x, initialTransform.position.y};
    bodyDef.rotation = b2MakeRot(initialTransform.rotation * glm::pi<float>() / 180.0f);
    
    switch (type) {
        case Type::Static:
            bodyDef.type = b2_staticBody;
            break;
        case Type::Kinematic:
            bodyDef.type = b2_kinematicBody;
            break;
        case Type::Dynamic:
            bodyDef.type = b2_dynamicBody;
            break;
    }
    
    // Create the body
    b2WorldId worldId = core::Engine::Physics().GetWorldId();
    m_bodyId = b2CreateBody(worldId, &bodyDef);
    
    if (m_bodyId.index1 != 0) {
        m_valid = true;
    } else {
        spdlog::error("Failed to create physics body");
    }
}

RigidBody::~RigidBody() {
    if (m_valid && m_bodyId.index1 != 0) {
        b2DestroyBody(m_bodyId);
    }
}

void RigidBody::SetPosition(const glm::vec2& position) {
    if (!m_valid) return;
    b2Body_SetTransform(m_bodyId, {position.x, position.y}, b2Body_GetRotation(m_bodyId));
}

void RigidBody::SetRotation(float degrees) {
    if (!m_valid) return;
    float radians = degrees * glm::pi<float>() / 180.0f;
    b2Body_SetTransform(m_bodyId, b2Body_GetPosition(m_bodyId), b2MakeRot(radians));
}

glm::vec2 RigidBody::GetPosition() const {
    if (!m_valid) return {0, 0};
    b2Vec2 pos = b2Body_GetPosition(m_bodyId);
    return {pos.x, pos.y};
}

float RigidBody::GetRotation() const {
    if (!m_valid) return 0.0f;
    b2Rot rotation = b2Body_GetRotation(m_bodyId);
    return b2Rot_GetAngle(rotation) * 180.0f / glm::pi<float>();
}

void RigidBody::ApplyForce(const glm::vec2& force) {
    if (!m_valid) return;
    b2Body_ApplyForceToCenter(m_bodyId, {force.x, force.y}, true);
}

void RigidBody::ApplyImpulse(const glm::vec2& impulse) {
    if (!m_valid) return;
    b2Body_ApplyLinearImpulseToCenter(m_bodyId, {impulse.x, impulse.y}, true);
}

void RigidBody::SetVelocity(const glm::vec2& velocity) {
    if (!m_valid) return;
    b2Body_SetLinearVelocity(m_bodyId, {velocity.x, velocity.y});
}

glm::vec2 RigidBody::GetVelocity() const {
    if (!m_valid) return {0, 0};
    b2Vec2 vel = b2Body_GetLinearVelocity(m_bodyId);
    return {vel.x, vel.y};
}

void RigidBody::SetDensity(float density) {
    // Note: This would require iterating through all shapes on the body
    // For simplicity, store density for when shapes are created
    spdlog::warn("SetDensity not implemented - set density when creating shapes");
}

void RigidBody::SetFriction(float friction) {
    // Note: This would require iterating through all shapes on the body
    spdlog::warn("SetFriction not implemented - set friction when creating shapes");
}

void RigidBody::SetRestitution(float restitution) {
    // Note: This would require iterating through all shapes on the body
    spdlog::warn("SetRestitution not implemented - set restitution when creating shapes");
}

void RigidBody::AddBoxCollider(float width, float height, const glm::vec2& offset) {
    if (!m_valid) return;
    
    b2ShapeDef shapeDef = b2DefaultShapeDef();
    shapeDef.density = 1.0f;
    
    b2Polygon box = b2MakeOffsetBox(width / 2.0f, height / 2.0f, {offset.x, offset.y}, b2Rot_identity);
    b2CreatePolygonShape(m_bodyId, &shapeDef, &box);
}

void RigidBody::AddCircleCollider(float radius, const glm::vec2& offset) {
    if (!m_valid) return;
    
    b2ShapeDef shapeDef = b2DefaultShapeDef();
    shapeDef.density = 1.0f;
    
    b2Circle circle = {{offset.x, offset.y}, radius};
    b2CreateCircleShape(m_bodyId, &shapeDef, &circle);
}

bool RigidBody::IsValid() const {
    return m_valid && m_bodyId.index1 != 0;
}

} // namespace physics