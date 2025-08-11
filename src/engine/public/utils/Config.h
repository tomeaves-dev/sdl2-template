#pragma once

#include <nlohmann/json.hpp>
#include <string>
#include <memory>

namespace utils {
    
    /**
     * Configuration Management System
     * 
     * Provides a centralized way to manage game settings using JSON.
     * Supports hierarchical configuration with dot notation access.
     * 
     * Example usage:
     *   auto config = Config::Instance();
     *   config->Load("settings.json");
     *   
     *   int width = config->GetInt("window.width", 1280);
     *   config->SetBool("graphics.vsync", true);
     *   config->Save();
     */
    class Config {
    public:
        // Singleton access
        static std::shared_ptr<Config> Instance();
        
        // File operations
        bool Load(const std::string& filename = "settings.json");
        bool Save(const std::string& filename = "");
        void LoadDefaults();
        
        // Getters with default values
        int GetInt(const std::string& path, int defaultValue = 0) const;
        float GetFloat(const std::string& path, float defaultValue = 0.0f) const;
        bool GetBool(const std::string& path, bool defaultValue = false) const;
        std::string GetString(const std::string& path, const std::string& defaultValue = "") const;
        
        // Setters
        void SetInt(const std::string& path, int value);
        void SetFloat(const std::string& path, float value);
        void SetBool(const std::string& path, bool value);
        void SetString(const std::string& path, const std::string& value);
        
        // Advanced access
        nlohmann::json GetSection(const std::string& path) const;
        void SetSection(const std::string& path, const nlohmann::json& section);
        
        // Utility functions
        bool HasKey(const std::string& path) const;
        void Reset();
        void Shutdown(); // Clean shutdown method
        std::string GetConfigPath() const { return m_configPath; }
        
        // Get the raw JSON (for advanced usage)
        const nlohmann::json& GetRawJson() const { return m_config; }
        
    private:
        Config() = default;
        
        // Helper functions
        nlohmann::json* GetValuePointer(const std::string& path, bool createPath = false);
        const nlohmann::json* GetValuePointer(const std::string& path) const;
        std::vector<std::string> SplitPath(const std::string& path) const;
        void CreateDefaultConfig();
        
        static std::shared_ptr<Config> s_instance;
        nlohmann::json m_config;
        std::string m_configPath;
        bool m_hasUnsavedChanges = false;
    };
    
} // namespace utils