#include "public/physics/Physics.h"
#include <spdlog/spdlog.h>

namespace physics {

Physics::Physics() : m_initialized(false) {
    m_worldId = b2_nullWorldId;
}

Physics::~Physics() {
    Shutdown();
}

bool Physics::Initialize() {
    spdlog::info("Initializing physics...");
    
    // Create Box2D world with no gravity by default
    b2WorldDef worldDef = b2DefaultWorldDef();
    worldDef.gravity = {0.0f, 0.0f}; // No gravity by default - set it if needed
    
    m_worldId = b2CreateWorld(&worldDef);
    if (m_worldId.index1 == 0) { // Check if world creation failed
        spdlog::error("Failed to create Box2D world");
        return false;
    }
    
    m_initialized = true;
    spdlog::info("Physics initialized successfully (no gravity - use SetGravity() to enable)");
    return true;
}

void Physics::Shutdown() {
    if (m_initialized && m_worldId.index1 != 0) {
        b2DestroyWorld(m_worldId);
        m_worldId = b2_nullWorldId;
        m_initialized = false;
        spdlog::info("Physics shutdown");
    }
}

void Physics::Step(float deltaTime) {
    if (!m_initialized) return;
    
    // Step the physics simulation
    // Box2D recommends fixed timestep, but this is a simple example
    int32_t subStepCount = 4;
    b2World_Step(m_worldId, deltaTime, subStepCount);
}

void Physics::SetGravity(float x, float y) {
    if (!m_initialized) return;
    
    b2World_SetGravity(m_worldId, {x, y});
    spdlog::info("Physics gravity set to: ({}, {})", x, y);
}

void Physics::GetGravity(float& x, float& y) const {
    if (!m_initialized) {
        x = y = 0.0f;
        return;
    }
    
    b2Vec2 gravity = b2World_GetGravity(m_worldId);
    x = gravity.x;
    y = gravity.y;
}

} // namespace physics