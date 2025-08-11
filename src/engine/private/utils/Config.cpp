#include "engine/public/utils/Config.h"
#include <fstream>
#include <spdlog/spdlog.h>

namespace utils {

// Static member definition
std::shared_ptr<Config> Config::s_instance = nullptr;

std::shared_ptr<Config> Config::Instance() {
    if (!s_instance) {
        s_instance = std::shared_ptr<Config>(new Config());
    }
    return s_instance;
}

bool Config::Load(const std::string& filename) {
    m_configPath = filename;
    
    // Check if file exists by trying to open it
    std::ifstream testFile(filename);
    if (!testFile.good()) {
        testFile.close();
        spdlog::info("Config file '{}' not found, creating with defaults", filename);
        LoadDefaults();
        return Save(filename);
    }
    testFile.close();
    
    try {
        std::ifstream file(filename);
        if (!file.is_open()) {
            spdlog::error("Failed to open config file: {}", filename);
            LoadDefaults();
            return false;
        }
        
        file >> m_config;
        file.close();
        
        spdlog::info("Config loaded successfully from: {}", filename);
        m_hasUnsavedChanges = false;
        return true;
        
    } catch (const std::exception& e) {
        spdlog::error("Failed to parse config file '{}': {}", filename, e.what());
        spdlog::info("Loading default configuration");
        LoadDefaults();
        return false;
    }
}

bool Config::Save(const std::string& filename) {
    std::string saveFile = filename.empty() ? m_configPath : filename;
    
    if (saveFile.empty()) {
        spdlog::error("No config file path specified for saving");
        return false;
    }
    
    try {
        std::ofstream file(saveFile);
        if (!file.is_open()) {
            spdlog::error("Failed to open config file for writing: {}", saveFile);
            return false;
        }
        
        // Pretty print with 4 space indentation
        file << m_config.dump(4);
        file.close();
        
        spdlog::info("Config saved successfully to: {}", saveFile);
        m_hasUnsavedChanges = false;
        return true;
        
    } catch (const std::exception& e) {
        spdlog::error("Failed to save config file '{}': {}", saveFile, e.what());
        return false;
    }
}

void Config::LoadDefaults() {
    CreateDefaultConfig();
    m_hasUnsavedChanges = true;
    spdlog::info("Default configuration loaded");
}

int Config::GetInt(const std::string& path, int defaultValue) const {
    const auto* value = GetValuePointer(path);
    if (value && value->is_number_integer()) {
        return value->get<int>();
    }
    return defaultValue;
}

float Config::GetFloat(const std::string& path, float defaultValue) const {
    const auto* value = GetValuePointer(path);
    if (value && value->is_number()) {
        return value->get<float>();
    }
    return defaultValue;
}

bool Config::GetBool(const std::string& path, bool defaultValue) const {
    const auto* value = GetValuePointer(path);
    if (value && value->is_boolean()) {
        return value->get<bool>();
    }
    return defaultValue;
}

std::string Config::GetString(const std::string& path, const std::string& defaultValue) const {
    const auto* value = GetValuePointer(path);
    if (value && value->is_string()) {
        return value->get<std::string>();
    }
    return defaultValue;
}

void Config::SetInt(const std::string& path, int value) {
    auto* configValue = GetValuePointer(path, true);
    if (configValue) {
        *configValue = value;
        m_hasUnsavedChanges = true;
    }
}

void Config::SetFloat(const std::string& path, float value) {
    auto* configValue = GetValuePointer(path, true);
    if (configValue) {
        *configValue = value;
        m_hasUnsavedChanges = true;
    }
}

void Config::SetBool(const std::string& path, bool value) {
    auto* configValue = GetValuePointer(path, true);
    if (configValue) {
        *configValue = value;
        m_hasUnsavedChanges = true;
    }
}

void Config::SetString(const std::string& path, const std::string& value) {
    auto* configValue = GetValuePointer(path, true);
    if (configValue) {
        *configValue = value;
        m_hasUnsavedChanges = true;
    }
}

nlohmann::json Config::GetSection(const std::string& path) const {
    const auto* value = GetValuePointer(path);
    if (value && value->is_object()) {
        return *value;
    }
    return nlohmann::json::object();
}

void Config::SetSection(const std::string& path, const nlohmann::json& section) {
    auto* configValue = GetValuePointer(path, true);
    if (configValue) {
        *configValue = section;
        m_hasUnsavedChanges = true;
    }
}

bool Config::HasKey(const std::string& path) const {
    return GetValuePointer(path) != nullptr;
}

void Config::Reset() {
    m_config.clear();
    LoadDefaults();
}

void Config::Shutdown() {
    if (!m_configPath.empty()) {
        Save(m_configPath);
    }
    // Reset the singleton instance
    s_instance.reset();
}

nlohmann::json* Config::GetValuePointer(const std::string& path, bool createPath) {
    auto pathParts = SplitPath(path);
    nlohmann::json* current = &m_config;
    
    for (size_t i = 0; i < pathParts.size(); ++i) {
        const auto& part = pathParts[i];
        
        if (i == pathParts.size() - 1) {
            // Last part - this is our target
            if (createPath || current->contains(part)) {
                return &(*current)[part];
            }
            return nullptr;
        } else {
            // Intermediate part - navigate or create
            if (!current->contains(part)) {
                if (createPath) {
                    (*current)[part] = nlohmann::json::object();
                } else {
                    return nullptr;
                }
            }
            
            if (!(*current)[part].is_object()) {
                if (createPath) {
                    (*current)[part] = nlohmann::json::object();
                } else {
                    return nullptr;
                }
            }
            
            current = &(*current)[part];
        }
    }
    
    return current;
}

const nlohmann::json* Config::GetValuePointer(const std::string& path) const {
    auto pathParts = SplitPath(path);
    const nlohmann::json* current = &m_config;
    
    for (const auto& part : pathParts) {
        if (!current->contains(part)) {
            return nullptr;
        }
        current = &(*current)[part];
    }
    
    return current;
}

std::vector<std::string> Config::SplitPath(const std::string& path) const {
    std::vector<std::string> parts;
    std::string current;
    
    for (char c : path) {
        if (c == '.') {
            if (!current.empty()) {
                parts.push_back(current);
                current.clear();
            }
        } else {
            current += c;
        }
    }
    
    if (!current.empty()) {
        parts.push_back(current);
    }
    
    return parts;
}

void Config::CreateDefaultConfig() {
    m_config = nlohmann::json{
        {"window", {
            {"width", 1280},
            {"height", 720},
            {"fullscreen", false},
            {"vsync", true},
            {"title", "SDL2 Game"}
        }},
        {"graphics", {
            {"renderer", "opengl"},
            {"antialiasing", true},
            {"texture_filtering", "linear"},
            {"shadow_quality", "medium"}
        }},
        {"audio", {
            {"master_volume", 1.0f},
            {"music_volume", 0.8f},
            {"sfx_volume", 1.0f},
            {"muted", false}
        }},
        {"input", {
            {"mouse_sensitivity", 1.0f},
            {"invert_mouse", false},
            {"key_bindings", {
                {"move_up", "W"},
                {"move_down", "S"},
                {"move_left", "A"},
                {"move_right", "D"},
                {"jump", "Space"},
                {"action", "E"},
                {"menu", "Escape"}
            }}
        }},
        {"game", {
            {"difficulty", "normal"},
            {"auto_save", true},
            {"show_fps", false},
            {"language", "en"}
        }}
    };
}

} // namespace utils