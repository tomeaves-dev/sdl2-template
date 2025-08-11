#include "engine/public/core/Engine.h"
#include "engine/public/utils/Logger.h"
#include <spdlog/spdlog.h>
#include <exception>

int main(int argc, char* argv[]) {
    // Initialize logger first
    utils::Logger::Initialize();
    
    try {
        spdlog::info("Starting SDL2 Template...");
        
        // Initialize the engine
        if (!core::Engine::Initialize()) {
            spdlog::error("Failed to initialize engine");
            utils::Logger::Shutdown();
            return 1;
        }
        
        // Run the main game loop
        core::Engine::Run();
        
        spdlog::info("Game finished successfully");
        
        // Shutdown the engine
        core::Engine::Shutdown();
        
    } catch (const std::exception& e) {
        spdlog::error("Unhandled exception: {}", e.what());
        core::Engine::Shutdown();
        utils::Logger::Shutdown();
        return 1;
    } catch (...) {
        spdlog::error("Unknown exception occurred");
        core::Engine::Shutdown();
        utils::Logger::Shutdown();
        return 1;
    }
    
    return 0;
}