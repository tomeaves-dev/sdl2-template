#pragma once

#include <SDL_mixer.h>
#include <string>
#include <unordered_map>
#include <memory>

namespace audio {

    enum class AudioCategory {
        SFX,
        Music,
        Ambient
    };

    using AudioHandle = int;
    static constexpr AudioHandle INVALID_AUDIO_HANDLE = -1;

    /**
     * Audio Manager
     * 
     * Manages audio playback with category-based volume control.
     * Supports both one-shot sounds (SFX) and streaming audio (Music/Ambient).
     * 
     * Volume calculation: final_volume = sound_volume * category_volume * master_volume
     */
    class AudioManager {
    public:
        AudioManager();
        ~AudioManager();

        // System lifecycle
        bool Initialize();
        void Shutdown();
        void Update(); // Call each frame to clean up finished sounds
        
        bool IsInitialized() const { return m_initialized; }

        // Loading/Unloading
        bool LoadSound(const std::string& filepath, const std::string& name, AudioCategory category);
        bool LoadStream(const std::string& filepath, const std::string& name, AudioCategory category);
        void UnloadSound(const std::string& name);
        void UnloadStream(const std::string& name);

        // Playback
        AudioHandle PlaySound(const std::string& name, float volume = 1.0f);
        AudioHandle PlayStream(const std::string& name, float volume = 1.0f, bool loop = true);

        // Individual audio control
        void PauseAudio(AudioHandle handle);
        void ResumeAudio(AudioHandle handle);
        void StopAudio(AudioHandle handle);
        void SetAudioVolume(AudioHandle handle, float volume);
        bool IsAudioPlaying(AudioHandle handle) const;

        // Category volume control
        void SetMasterVolume(float volume);
        void SetCategoryVolume(AudioCategory category, float volume);
        float GetMasterVolume() const { return m_masterVolume; }
        float GetCategoryVolume(AudioCategory category) const;

        // Global controls
        void SetMuted(bool muted);
        bool IsMuted() const { return m_muted; }
        void PauseAll();
        void ResumeAll();
        void StopAll();

    private:
        struct SoundData {
            Mix_Chunk* chunk = nullptr;
            AudioCategory category = AudioCategory::SFX;
            std::string filepath;
        };

        struct StreamData {
            Mix_Music* music = nullptr;
            AudioCategory category = AudioCategory::Music;
            std::string filepath;
        };

        struct PlayingAudio {
            AudioCategory category = AudioCategory::SFX;
            float volume = 1.0f;
            int channel = -1;  // For Mix_Chunk sounds (-1 for Mix_Music)
            bool isStream = false;
            bool isPaused = false;
        };

        // Internal helpers
        float CalculateFinalVolume(AudioCategory category, float soundVolume) const;
        void UpdateChannelVolume(int channel, AudioCategory category, float soundVolume);
        AudioHandle GetNextHandle();
        void CleanupFinishedSounds();

        bool m_initialized = false;
        bool m_muted = false;
        
        // Volume controls
        float m_masterVolume = 1.0f;
        float m_sfxVolume = 1.0f;
        float m_musicVolume = 1.0f;
        float m_ambientVolume = 1.0f;

        // Audio data storage
        std::unordered_map<std::string, SoundData> m_sounds;
        std::unordered_map<std::string, StreamData> m_streams;
        
        // Playing audio tracking
        std::unordered_map<AudioHandle, PlayingAudio> m_playingAudio;
        AudioHandle m_nextHandle = 1;
        
        // Currently playing music/stream (SDL_mixer only supports one music at a time)
        AudioHandle m_currentMusicHandle = INVALID_AUDIO_HANDLE;
    };

} // namespace audio