#include "public/utils/Logger.h"
#include <spdlog/sinks/stdout_color_sinks.h>

namespace utils {

void Logger::Initialize() {
    // Create console logger with colors
    auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    console_sink->set_level(spdlog::level::trace);
    
    auto logger = std::make_shared<spdlog::logger>("main", console_sink);
    logger->set_level(spdlog::level::trace);
    
    spdlog::set_default_logger(logger);
    spdlog::set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%l] %v");
    
    spdlog::info("Logger initialized");
}

void Logger::Shutdown() {
    spdlog::info("Logger shutdown");
    spdlog::shutdown();
}

} // namespace utils