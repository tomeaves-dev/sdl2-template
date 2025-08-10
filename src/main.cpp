#include "public/core/Game.h"
#include <spdlog/spdlog.h>

int main(int argc, char* argv[]) {
    spdlog::info("Starting SDL2 Template...");
    
    core::Game game;
    
    if (!game.Initialize()) {
        spdlog::error("Failed to initialize game");
        return 1;
    }
    
    game.Run();
    
    spdlog::info("Game finished");
    return 0;
}