#pragma once

#include <spdlog/spdlog.h>
#include <string>

namespace utils {
    class Logger {
    public:
        static void Initialize();
        static void Shutdown();
        
        template<typename... Args>
        static void Info(const std::string& message, Args&&... args) {
            spdlog::info(message, std::forward<Args>(args)...);
        }
        
        template<typename... Args>
        static void Warning(const std::string& message, Args&&... args) {
            spdlog::warn(message, std::forward<Args>(args)...);
        }
        
        template<typename... Args>
        static void Error(const std::string& message, Args&&... args) {
            spdlog::error(message, std::forward<Args>(args)...);
        }
    };
}