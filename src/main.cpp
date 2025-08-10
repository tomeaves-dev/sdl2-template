#include "public/core/Game.h"
#include "public/utils/Logger.h"
#include <spdlog/spdlog.h>
#include <exception>

int main(int argc, char* argv[]) {
    // Initialize logger first
    utils::Logger::Initialize();
    
    try {
        spdlog::info("Starting SDL2 Template...");
        
        // Create game in a scope to ensure proper destruction order
        {
            core::Game game;
            
            if (!game.Initialize()) {
                spdlog::error("Failed to initialize game");
                utils::Logger::Shutdown();
                return 1;
            }
            
            game.Run();
        } // Game destructor called here
        
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