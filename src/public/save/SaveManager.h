#pragma once

#include <nlohmann/json.hpp>
#include <string>
#include <vector>
#include <optional>
#include <unordered_set>
#include <filesystem>
#include <spdlog/spdlog.h>

namespace save {

    /**
     * Save Manager
     * 
     * Manages game save/load functionality with type safety.
     * Consistent with other game systems (AudioManager, etc.)
     * 
     * Features:
     * - Type-safe save/load with automatic type registration
     * - Multiple save slots with unlimited saves per slot
     * - One quicksave per type (PlayerData quicksave, GameState quicksave, etc.)
     * - JSON format with automatic timestamped file naming
     * - Cross-platform save directory management
     * 
     * File Structure:
     *   {user_documents}/{game_name}/saves/
     *   ├── main-game/
     *   │   ├── MyGame_main-game_2025-08-11_14-30-45.json
     *   │   └── MyGame_main-game_2025-08-11_15-22-10.json
     *   └── quicksave/
     *       ├── PlayerData/
     *       │   └── MyGame_PlayerData_quicksave_2025-08-11_16-50-12.json
     *       └── GameState/
     *           └── MyGame_GameState_quicksave_2025-08-11_17-15-30.json
     * 
     * Usage:
     *   struct PlayerData {
     *       int level = 1;
     *       std::string name = "Player";
     *       NLOHMANN_DEFINE_TYPE_INTRUSIVE(PlayerData, level, name)
     *   };
     * 
     *   saveManager->Save<PlayerData>("main-game", playerData);
     *   auto data = saveManager->Load<PlayerData>("main-game", "filename.json");
     *   saveManager->Quicksave<PlayerData>(playerData);
     *   auto quickData = saveManager->LoadQuicksave<PlayerData>();
     */
    class SaveManager {
    public:
        SaveManager();
        ~SaveManager();

        // System lifecycle (implemented in .cpp)
        bool Initialize();
        void Shutdown();
        bool IsInitialized() const { return m_initialized; }

        // Configuration (implemented in .cpp)
        void SetGameName(const std::string& gameName);
        void SetSaveDirectory(const std::string& directory);
        void SetCompressionEnabled(bool enabled);

        // Type-safe save operations (templates - must be in header)
        template<typename T>
        bool Save(const std::string& slotName, const T& data);

        template<typename T>
        bool Quicksave(const T& data);

        // Type-safe load operations (templates - must be in header)
        template<typename T>
        std::optional<T> Load(const std::string& slotName, const std::string& filename);

        template<typename T>
        std::optional<T> LoadQuicksave();

        // Save management (implemented in .cpp)
        std::vector<std::string> GetSavesInSlot(const std::string& slotName) const;
        std::vector<std::string> GetAllSlots() const;
        std::vector<std::string> GetQuicksaveTypes() const;
        bool DeleteSave(const std::string& slotName, const std::string& filename);
        bool DeleteSlot(const std::string& slotName);
        bool DeleteQuicksave(const std::string& typeName);
        bool DeleteAllQuicksaves();

        // Utility (mix of templates and regular functions)
        bool SlotExists(const std::string& slotName) const;
        bool SaveExists(const std::string& slotName, const std::string& filename) const;
        template<typename T>
        bool QuicksaveExists() const;
        size_t GetSlotSaveCount(const std::string& slotName) const;

        // Path helpers (implemented in .cpp)
        std::string GetSaveDirectory() const;
        std::string GetSlotPath(const std::string& slotName) const;
        std::string GetQuicksavePath() const;

    private:
        // Internal operations (implemented in .cpp)
        std::string GetDefaultSaveDirectory() const;
        std::string GenerateTimestamp() const;
        std::string GenerateSaveFilename(const std::string& slotName) const;
        bool EnsureDirectoryExists(const std::string& path) const;
        bool SaveJson(const std::string& filePath, const nlohmann::json& data);
        std::optional<nlohmann::json> LoadJson(const std::string& filePath);

        // Template helpers (must be in header)
        template<typename T>
        std::string GenerateQuicksaveFilename() const;
        template<typename T>
        std::string GetQuicksaveTypePath() const;
        template<typename T>
        std::string GetTypeName() const;
        template<typename T>
        void RegisterTypeIfNeeded();

        // Member variables
        bool m_initialized = false;
        bool m_compressionEnabled = false;
        std::string m_gameName = "UnknownGame";
        std::string m_saveDirectory;
        std::unordered_set<std::string> m_registeredTypes;
    };

    // Template implementations (must be in header)
    template<typename T>
    bool SaveManager::Save(const std::string& slotName, const T& data) {
        if (!m_initialized) {
            spdlog::error("SaveManager not initialized");
            return false;
        }

        if (slotName.empty()) {
            spdlog::error("Save slot name cannot be empty");
            return false;
        }

        RegisterTypeIfNeeded<T>();

        try {
            nlohmann::json jsonData = data;

            nlohmann::json saveData;
            saveData["version"] = "1.0.0";
            saveData["timestamp"] = GenerateTimestamp();
            saveData["type"] = GetTypeName<T>();
            saveData["data"] = jsonData;

            std::string filename = GenerateSaveFilename(slotName);
            std::string slotPath = GetSlotPath(slotName);

            if (!EnsureDirectoryExists(slotPath)) {
                spdlog::error("Failed to create save slot directory: {}", slotPath);
                return false;
            }

            std::string fullPath = slotPath + "/" + filename;

            bool success = SaveJson(fullPath, saveData);
            if (success) {
                spdlog::info("Saved to slot '{}': {}", slotName, filename);
            }

            return success;

        } catch (const std::exception& e) {
            spdlog::error("Failed to save to slot '{}': {}", slotName, e.what());
            return false;
        }
    }

    template<typename T>
    bool SaveManager::Quicksave(const T& data) {
        if (!m_initialized) {
            spdlog::error("SaveManager not initialized");
            return false;
        }

        RegisterTypeIfNeeded<T>();

        try {
            nlohmann::json jsonData = data;

            nlohmann::json saveData;
            saveData["version"] = "1.0.0";
            saveData["timestamp"] = GenerateTimestamp();
            saveData["type"] = GetTypeName<T>();
            saveData["data"] = jsonData;

            std::string filename = GenerateQuicksaveFilename<T>();
            std::string quicksavePath = GetQuicksaveTypePath<T>();

            if (!EnsureDirectoryExists(quicksavePath)) {
                spdlog::error("Failed to create quicksave directory: {}", quicksavePath);
                return false;
            }

            std::string fullPath = quicksavePath + "/" + filename;

            if (QuicksaveExists<T>()) {
                DeleteQuicksave(GetTypeName<T>());
            }

            bool success = SaveJson(fullPath, saveData);
            if (success) {
                spdlog::info("Quicksaved {}: {}", GetTypeName<T>(), filename);
            }

            return success;

        } catch (const std::exception& e) {
            spdlog::error("Failed to quicksave {}: {}", GetTypeName<T>(), e.what());
            return false;
        }
    }

    template<typename T>
    std::optional<T> SaveManager::Load(const std::string& slotName, const std::string& filename) {
        if (!m_initialized) {
            spdlog::error("SaveManager not initialized");
            return std::nullopt;
        }

        if (slotName.empty() || filename.empty()) {
            spdlog::error("Slot name and filename cannot be empty");
            return std::nullopt;
        }

        try {
            std::string fullPath = GetSlotPath(slotName) + "/" + filename;

            auto jsonData = LoadJson(fullPath);
            if (!jsonData.has_value()) {
                spdlog::error("Failed to load save file: {}", fullPath);
                return std::nullopt;
            }

            if (!jsonData->contains("version") || !jsonData->contains("data")) {
                spdlog::error("Invalid save file format: {}", fullPath);
                return std::nullopt;
            }

            std::string version = (*jsonData)["version"];
            if (version != "1.0.0") {
                spdlog::error("Incompatible save file version '{}': {}", version, fullPath);
                return std::nullopt;
            }

            T result = (*jsonData)["data"];

            spdlog::info("Loaded from slot '{}': {}", slotName, filename);
            return result;

        } catch (const std::exception& e) {
            spdlog::error("Failed to load from slot '{}', file '{}': {}", slotName, filename, e.what());
            return std::nullopt;
        }
    }

    template<typename T>
    std::optional<T> SaveManager::LoadQuicksave() {
        if (!m_initialized) {
            spdlog::error("SaveManager not initialized");
            return std::nullopt;
        }

        if (!QuicksaveExists<T>()) {
            spdlog::info("No quicksave found for type {}", GetTypeName<T>());
            return std::nullopt;
        }

        try {
            std::string quicksavePath = GetQuicksaveTypePath<T>();
            std::vector<std::string> saves;

            if (std::filesystem::exists(quicksavePath)) {
                for (const auto& entry : std::filesystem::directory_iterator(quicksavePath)) {
                    if (entry.is_regular_file() && entry.path().extension() == ".json") {
                        saves.push_back(entry.path().filename().string());
                    }
                }
            }

            if (saves.empty()) {
                spdlog::info("No quicksave found for type {}", GetTypeName<T>());
                return std::nullopt;
            }

            std::string fullPath = quicksavePath + "/" + saves[0];

            auto jsonData = LoadJson(fullPath);
            if (!jsonData.has_value()) {
                spdlog::error("Failed to load quicksave file: {}", fullPath);
                return std::nullopt;
            }

            if (!jsonData->contains("version") || !jsonData->contains("data")) {
                spdlog::error("Invalid quicksave file format: {}", fullPath);
                return std::nullopt;
            }

            std::string version = (*jsonData)["version"];
            if (version != "1.0.0") {
                spdlog::error("Incompatible quicksave version '{}': {}", version, fullPath);
                return std::nullopt;
            }

            T result = (*jsonData)["data"];

            spdlog::info("Loaded quicksave for {}: {}", GetTypeName<T>(), saves[0]);
            return result;

        } catch (const std::exception& e) {
            spdlog::error("Failed to load quicksave for {}: {}", GetTypeName<T>(), e.what());
            return std::nullopt;
        }
    }

    template<typename T>
    bool SaveManager::QuicksaveExists() const {
        std::string quicksavePath = GetQuicksaveTypePath<T>();
        
        if (!std::filesystem::exists(quicksavePath)) {
            return false;
        }

        for (const auto& entry : std::filesystem::directory_iterator(quicksavePath)) {
            if (entry.is_regular_file() && entry.path().extension() == ".json") {
                return true;
            }
        }

        return false;
    }

    template<typename T>
    std::string SaveManager::GetQuicksaveTypePath() const {
        return GetQuicksavePath() + "/" + GetTypeName<T>();
    }

    template<typename T>
    std::string SaveManager::GenerateQuicksaveFilename() const {
        return m_gameName + "_" + GetTypeName<T>() + "_quicksave_" + GenerateTimestamp() + ".json";
    }

    template<typename T>
    std::string SaveManager::GetTypeName() const {
        std::string typeName = typeid(T).name();
        
        // Remove common prefixes from mangled names for better readability
        size_t pos = typeName.find_last_of(':');
        if (pos != std::string::npos && pos < typeName.length() - 1) {
            typeName = typeName.substr(pos + 1);
        }
        
        return typeName;
    }

    template<typename T>
    void SaveManager::RegisterTypeIfNeeded() {
        std::string typeName = GetTypeName<T>();

        if (m_registeredTypes.find(typeName) == m_registeredTypes.end()) {
            m_registeredTypes.insert(typeName);
            spdlog::debug("Auto-registered save type: {}", typeName);
        }
    }

} // namespace save