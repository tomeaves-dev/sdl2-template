#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <box2d/box2d.h>
#include <glm/glm.hpp>
#include <spdlog/spdlog.h>
#include <fmt/format.h>
#include <nlohmann/json.hpp>
#include <tmxlite/Map.hpp>
#include <glad/glad.h>
#include <imgui.h>

int main(int argc, char* argv[]) {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0) {
        spdlog::error("SDL_Init Error: {}", SDL_GetError());
        return 1;
    }

    // Test our libraries by creating simple objects
    spdlog::info("SDL2 Template initialized successfully!");
    spdlog::info("Testing libraries:");
    
    // Test GLM
    glm::vec3 testVec(1.0f, 2.0f, 3.0f);
    spdlog::info("GLM vector: ({}, {}, {})", testVec.x, testVec.y, testVec.z);
    
    // Test fmt
    std::string formatted = fmt::format("Formatted string: {}", 42);
    spdlog::info(formatted);
    
    // Test JSON
    nlohmann::json testJson = {{"test", "value"}, {"number", 123}};
    spdlog::info("JSON test: {}", testJson.dump());
    
    // Test Box2D 3.x (new API)
    b2Vec2 gravity = {0.0f, -10.0f};
    b2WorldDef worldDef = b2DefaultWorldDef();
    worldDef.gravity = gravity;
    b2WorldId worldId = b2CreateWorld(&worldDef);
    spdlog::info("Box2D world created with gravity: ({}, {})", gravity.x, gravity.y);
    b2DestroyWorld(worldId);
    
    // Test ImGui (just create context, don't render)
    ImGui::CreateContext();
    spdlog::info("ImGui context created successfully");
    ImGui::DestroyContext();
    
    spdlog::info("All libraries loaded successfully!");
    spdlog::info("Template is ready for game development!");

    SDL_Quit();
    return 0;
}