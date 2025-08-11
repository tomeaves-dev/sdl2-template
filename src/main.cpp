#include "engine/public/core/Game.h"
#include "engine/public/utils/Logger.h"
#include <spdlog/spdlog.h>
#include <exception>

int main(int argc, char* argv[]) {
    // Initialize logger first
    utils::Logger::Initialize();
    
    try {
        spdlog::info("Starting SDL2 Template...");
        
        // Create and run the game
        core::Game game;
        
        if (!game.Initialize()) {
            spdlog::error("Failed to initialize game");
            utils::Logger::Shutdown();
            return 1;
        }
        
        // Run the main game loop
        game.Run();
        
        // Game shutdown is handled in destructor
        spdlog::info("Game finished successfully");
        
    } catch (const std::exception& e) {
        spdlog::error("Unhandled exception: {}", e.what());
        utils::Logger::Shutdown();
        return 1;
    } catch (...) {
        spdlog::error("Unknown exception occurred");
        utils::Logger::Shutdown();
        return 1;
    }
    
    // Shutdown logger last
    utils::Logger::Shutdown();
    return 0;
}