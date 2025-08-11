#pragma once
#include "engine/public/core/IGameApplication.h"

class GameApplication : public core::IGameApplication {
public:
    bool Initialize() override;
    void Update(float deltaTime) override;           // Variable timestep - input, animation, UI
    void FixedUpdate(float fixedDeltaTime) override; // Fixed timestep - physics, movement
    void Render() override;
    void Shutdown() override;
    
private:
    // User's game objects - complete freedom in organization
    // Examples:
    // Player player;
    // std::vector<Enemy> enemies;
    // UI userInterface;
};