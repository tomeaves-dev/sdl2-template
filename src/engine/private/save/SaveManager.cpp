#include "engine/public/save/SaveManager.h"
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <chrono>
#include <algorithm>

#ifdef _WIN32
#include <windows.h>
#include <shlobj.h>
#elif defined(__APPLE__)
#include <pwd.h>
#include <unistd.h>
#elif defined(__linux__)
#include <pwd.h>
#include <unistd.h>
#endif

namespace save {

SaveManager::SaveManager() = default;

SaveManager::~SaveManager() {
    Shutdown();
}

bool SaveManager::Initialize() {
    if (m_initialized) {
        spdlog::warn("SaveManager already initialized");
        return true;
    }
    
    spdlog::info("Initializing save manager...");
    
    // Set default save directory if not already set
    if (m_saveDirectory.empty()) {
        std::string defaultDir = GetDefaultSaveDirectory();
        if (defaultDir.empty()) {
            spdlog::error("Failed to determine default save directory");
            return false;
        }
        m_saveDirectory = defaultDir;
    }
    
    // Ensure base save directory exists
    if (!EnsureDirectoryExists(m_saveDirectory)) {
        spdlog::error("Failed to create save directory: {}", m_saveDirectory);
        return false;
    }
    
    m_initialized = true;
    spdlog::info("Save manager initialized successfully");
    spdlog::info("Game: '{}', Save directory: '{}'", m_gameName, m_saveDirectory);
    
    return true;
}

void SaveManager::Shutdown() {
    if (m_initialized) {
        spdlog::info("Shutting down save manager...");
        
        m_registeredTypes.clear();
        m_initialized = false;
        
        spdlog::info("Save manager shutdown complete");
    }
}

void SaveManager::SetGameName(const std::string& gameName) {
    if (gameName.empty()) {
        spdlog::warn("Game name cannot be empty, keeping current name: '{}'", m_gameName);
        return;
    }
    
    m_gameName = gameName;
    
    // Update save directory if using default
    if (m_saveDirectory.find("UnknownGame") != std::string::npos) {
        m_saveDirectory = GetDefaultSaveDirectory();
        spdlog::info("Updated save directory for game '{}': {}", m_gameName, m_saveDirectory);
    }
}

void SaveManager::SetSaveDirectory(const std::string& directory) {
    if (directory.empty()) {
        spdlog::warn("Save directory cannot be empty");
        return;
    }
    
    m_saveDirectory = directory;
    spdlog::info("Save directory set to: {}", m_saveDirectory);
}

void SaveManager::SetCompressionEnabled(bool enabled) {
    m_compressionEnabled = enabled;
    spdlog::info("Save compression {}", enabled ? "enabled" : "disabled");
}

std::vector<std::string> SaveManager::GetSavesInSlot(const std::string& slotName) const {
    std::vector<std::string> saves;
    
    if (!m_initialized) {
        spdlog::error("SaveManager not initialized");
        return saves;
    }
    
    std::string slotPath = GetSlotPath(slotName);
    
    if (!std::filesystem::exists(slotPath)) {
        return saves; // Empty vector for non-existent slot
    }
    
    try {
        for (const auto& entry : std::filesystem::directory_iterator(slotPath)) {
            if (entry.is_regular_file()) {
                std::string filename = entry.path().filename().string();
                if (filename.ends_with(".json")) {
                    saves.push_back(filename);
                }
            }
        }
        
        // Sort saves by filename (which includes timestamp)
        std::sort(saves.begin(), saves.end());
        
    } catch (const std::exception& e) {
        spdlog::error("Failed to list saves in slot '{}': {}", slotName, e.what());
    }
    
    return saves;
}

std::vector<std::string> SaveManager::GetAllSlots() const {
    std::vector<std::string> slots;
    
    if (!m_initialized) {
        spdlog::error("SaveManager not initialized");
        return slots;
    }
    
    if (!std::filesystem::exists(m_saveDirectory)) {
        return slots; // Empty vector if save directory doesn't exist
    }
    
    try {
        for (const auto& entry : std::filesystem::directory_iterator(m_saveDirectory)) {
            if (entry.is_directory()) {
                std::string slotName = entry.path().filename().string();
                if (slotName != "quicksave") { // Don't include quicksave in regular slots
                    slots.push_back(slotName);
                }
            }
        }
        
        std::sort(slots.begin(), slots.end());
        
    } catch (const std::exception& e) {
        spdlog::error("Failed to list save slots: {}", e.what());
    }
    
    return slots;
}

std::vector<std::string> SaveManager::GetQuicksaveTypes() const {
    std::vector<std::string> types;
    
    if (!m_initialized) {
        spdlog::error("SaveManager not initialized");
        return types;
    }
    
    std::string quicksavePath = GetQuicksavePath();
    
    if (!std::filesystem::exists(quicksavePath)) {
        return types; // Empty vector if quicksave directory doesn't exist
    }
    
    try {
        for (const auto& entry : std::filesystem::directory_iterator(quicksavePath)) {
            if (entry.is_directory()) {
                std::string typeName = entry.path().filename().string();
                types.push_back(typeName);
            }
        }
        
        std::sort(types.begin(), types.end());
        
    } catch (const std::exception& e) {
        spdlog::error("Failed to list quicksave types: {}", e.what());
    }
    
    return types;
}

bool SaveManager::DeleteSave(const std::string& slotName, const std::string& filename) {
    if (!m_initialized) {
        spdlog::error("SaveManager not initialized");
        return false;
    }
    
    try {
        std::string fullPath = GetSlotPath(slotName) + "/" + filename;
        
        if (!std::filesystem::exists(fullPath)) {
            spdlog::warn("Save file does not exist: {}", fullPath);
            return false;
        }
        
        std::filesystem::remove(fullPath);
        spdlog::info("Deleted save from slot '{}': {}", slotName, filename);
        return true;
        
    } catch (const std::exception& e) {
        spdlog::error("Failed to delete save from slot '{}', file '{}': {}", slotName, filename, e.what());
        return false;
    }
}

bool SaveManager::DeleteSlot(const std::string& slotName) {
    if (!m_initialized) {
        spdlog::error("SaveManager not initialized");
        return false;
    }
    
    if (slotName == "quicksave") {
        spdlog::warn("Cannot delete quicksave slot, use DeleteAllQuicksaves() instead");
        return false;
    }
    
    try {
        std::string slotPath = GetSlotPath(slotName);
        
        if (!std::filesystem::exists(slotPath)) {
            spdlog::warn("Slot does not exist: {}", slotName);
            return false;
        }
        
        std::filesystem::remove_all(slotPath);
        spdlog::info("Deleted slot: {}", slotName);
        return true;
        
    } catch (const std::exception& e) {
        spdlog::error("Failed to delete slot '{}': {}", slotName, e.what());
        return false;
    }
}

bool SaveManager::DeleteQuicksave(const std::string& typeName) {
    if (!m_initialized) {
        spdlog::error("SaveManager not initialized");
        return false;
    }
    
    try {
        std::string quicksaveTypePath = GetQuicksavePath() + "/" + typeName;
        
        if (!std::filesystem::exists(quicksaveTypePath)) {
            spdlog::info("No quicksave to delete for type: {}", typeName);
            return true;
        }
        
        // Delete all files in the quicksave type directory
        for (const auto& entry : std::filesystem::directory_iterator(quicksaveTypePath)) {
            if (entry.is_regular_file()) {
                std::filesystem::remove(entry.path());
            }
        }
        
        spdlog::info("Deleted quicksave for type: {}", typeName);
        return true;
        
    } catch (const std::exception& e) {
        spdlog::error("Failed to delete quicksave for type '{}': {}", typeName, e.what());
        return false;
    }
}

bool SaveManager::DeleteAllQuicksaves() {
    if (!m_initialized) {
        spdlog::error("SaveManager not initialized");
        return false;
    }
    
    try {
        std::string quicksavePath = GetQuicksavePath();
        
        if (!std::filesystem::exists(quicksavePath)) {
            spdlog::info("No quicksaves to delete");
            return true;
        }
        
        // Delete all quicksave type directories
        std::filesystem::remove_all(quicksavePath);
        
        spdlog::info("Deleted all quicksaves");
        return true;
        
    } catch (const std::exception& e) {
        spdlog::error("Failed to delete all quicksaves: {}", e.what());
        return false;
    }
}

bool SaveManager::SlotExists(const std::string& slotName) const {
    std::string slotPath = GetSlotPath(slotName);
    return std::filesystem::exists(slotPath) && std::filesystem::is_directory(slotPath);
}

bool SaveManager::SaveExists(const std::string& slotName, const std::string& filename) const {
    std::string fullPath = GetSlotPath(slotName) + "/" + filename;
    return std::filesystem::exists(fullPath) && std::filesystem::is_regular_file(fullPath);
}

size_t SaveManager::GetSlotSaveCount(const std::string& slotName) const {
    auto saves = GetSavesInSlot(slotName);
    return saves.size();
}

std::string SaveManager::GetSaveDirectory() const {
    return m_saveDirectory;
}

std::string SaveManager::GetSlotPath(const std::string& slotName) const {
    return m_saveDirectory + "/" + slotName;
}

std::string SaveManager::GetQuicksavePath() const {
    return m_saveDirectory + "/quicksave";
}

std::string SaveManager::GetDefaultSaveDirectory() const {
    std::string documentsPath;
    
    #ifdef _WIN32
    // Windows: Get Documents folder
    CHAR path[MAX_PATH];
    if (SUCCEEDED(SHGetFolderPathA(NULL, CSIDL_MYDOCUMENTS, NULL, SHGFP_TYPE_CURRENT, path))) {
        documentsPath = path;
    }
    #elif defined(__APPLE__)
    // macOS: Get Documents folder
    const char* homeDir = getpwuid(getuid())->pw_dir;
    if (homeDir) {
        documentsPath = std::string(homeDir) + "/Documents";
    }
    #elif defined(__linux__)
    // Linux: Get Documents folder (with XDG fallback)
    const char* xdgDocuments = std::getenv("XDG_DOCUMENTS_DIR");
    if (xdgDocuments) {
        documentsPath = xdgDocuments;
    } else {
        const char* homeDir = getpwuid(getuid())->pw_dir;
        if (homeDir) {
            documentsPath = std::string(homeDir) + "/Documents";
        }
    }
    #endif
    
    if (documentsPath.empty()) {
        spdlog::error("Failed to determine documents directory");
        return "";
    }
    
    return documentsPath + "/" + m_gameName + "/saves";
}

std::string SaveManager::GenerateTimestamp() const {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    auto tm = *std::localtime(&time_t);
    
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d_%H-%M-%S");
    return oss.str();
}

std::string SaveManager::GenerateSaveFilename(const std::string& slotName) const {
    return m_gameName + "_" + slotName + "_" + GenerateTimestamp() + ".json";
}

bool SaveManager::EnsureDirectoryExists(const std::string& path) const {
    try {
        if (!std::filesystem::exists(path)) {
            std::filesystem::create_directories(path);
            spdlog::debug("Created directory: {}", path);
        }
        return true;
    } catch (const std::exception& e) {
        spdlog::error("Failed to create directory '{}': {}", path, e.what());
        return false;
    }
}

bool SaveManager::SaveJson(const std::string& filePath, const nlohmann::json& data) {
    try {
        std::ofstream file(filePath);
        if (!file.is_open()) {
            spdlog::error("Failed to open file for writing: {}", filePath);
            return false;
        }
        
        if (m_compressionEnabled) {
            // For now, just compact JSON (no compression library)
            // In future could add actual compression with zlib
            file << data.dump();
        } else {
            // Pretty print for development
            file << data.dump(4);
        }
        
        file.close();
        return true;
        
    } catch (const std::exception& e) {
        spdlog::error("Failed to save JSON to '{}': {}", filePath, e.what());
        return false;
    }
}

std::optional<nlohmann::json> SaveManager::LoadJson(const std::string& filePath) {
    try {
        if (!std::filesystem::exists(filePath)) {
            spdlog::error("Save file does not exist: {}", filePath);
            return std::nullopt;
        }
        
        std::ifstream file(filePath);
        if (!file.is_open()) {
            spdlog::error("Failed to open file for reading: {}", filePath);
            return std::nullopt;
        }
        
        nlohmann::json data;
        file >> data;
        file.close();
        
        return data;
        
    } catch (const std::exception& e) {
        spdlog::error("Failed to load JSON from '{}': {}", filePath, e.what());
        return std::nullopt;
    }
}

} // namespace save