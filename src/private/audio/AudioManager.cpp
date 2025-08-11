#include "public/audio/AudioManager.h"
#include <spdlog/spdlog.h>
#include <filesystem>

namespace audio {

AudioManager::AudioManager() = default;

AudioManager::~AudioManager() {
    Shutdown();
}

bool AudioManager::Initialize() {
    spdlog::info("Initializing audio manager...");
    
    // Initialize SDL_mixer
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024) < 0) {
        spdlog::error("Failed to initialize SDL_mixer: {}", Mix_GetError());
        return false;
    }
    
    // Set number of mixing channels
    Mix_AllocateChannels(32);
    
    // Initialize supported audio formats
    int flags = MIX_INIT_OGG | MIX_INIT_MP3;
    int initialized = Mix_Init(flags);
    
    if ((initialized & flags) != flags) {
        spdlog::warn("Some audio formats may not be supported: {}", Mix_GetError());
    }
    
    m_initialized = true;
    spdlog::info("Audio manager initialized successfully");
    spdlog::info("Audio format: 44100 Hz, {} channels, {} mixing channels", 
                Mix_QuerySpec(nullptr, nullptr, nullptr), Mix_AllocateChannels(-1));
    
    return true;
}

void AudioManager::Shutdown() {
    if (m_initialized) {
        spdlog::info("Shutting down audio manager...");
        
        // Stop all audio
        StopAll();
        
        // Unload all sounds
        for (auto& [name, sound] : m_sounds) {
            if (sound.chunk) {
                Mix_FreeChunk(sound.chunk);
            }
        }
        m_sounds.clear();
        
        // Unload all streams
        for (auto& [name, stream] : m_streams) {
            if (stream.music) {
                Mix_FreeMusic(stream.music);
            }
        }
        m_streams.clear();
        
        m_playingAudio.clear();
        
        // Shutdown SDL_mixer
        Mix_Quit();
        Mix_CloseAudio();
        
        m_initialized = false;
        spdlog::info("Audio manager shutdown complete");
    }
}

void AudioManager::Update() {
    if (!m_initialized) return;
    
    CleanupFinishedSounds();
}

bool AudioManager::LoadSound(const std::string& filepath, const std::string& name, AudioCategory category) {
    if (!m_initialized) {
        spdlog::warn("Audio manager not initialized, cannot load sound: {}", name);
        return false;
    }
    
    // Check if already loaded
    if (m_sounds.find(name) != m_sounds.end()) {
        spdlog::warn("Sound '{}' already loaded", name);
        return true;
    }
    
    // Construct full path
    std::string fullPath = "assets/audio/";
    if (category == AudioCategory::SFX) {
        fullPath += "sfx/";
    }
    fullPath += filepath;
    
    // Check if file exists
    if (!std::filesystem::exists(fullPath)) {
        spdlog::error("Sound file not found: {}", fullPath);
        return false;
    }
    
    // Load the sound
    Mix_Chunk* chunk = Mix_LoadWAV(fullPath.c_str());
    if (!chunk) {
        spdlog::error("Failed to load sound '{}': {}", name, Mix_GetError());
        return false;
    }
    
    // Store sound data
    SoundData soundData;
    soundData.chunk = chunk;
    soundData.category = category;
    soundData.filepath = fullPath;
    
    m_sounds[name] = soundData;
    
    spdlog::info("Loaded sound '{}' from '{}'", name, fullPath);
    return true;
}

bool AudioManager::LoadStream(const std::string& filepath, const std::string& name, AudioCategory category) {
    if (!m_initialized) {
        spdlog::warn("Audio manager not initialized, cannot load stream: {}", name);
        return false;
    }
    
    // Check if already loaded
    if (m_streams.find(name) != m_streams.end()) {
        spdlog::warn("Stream '{}' already loaded", name);
        return true;
    }
    
    // Construct full path
    std::string fullPath = "assets/audio/";
    if (category == AudioCategory::Music) {
        fullPath += "music/";
    } else if (category == AudioCategory::Ambient) {
        fullPath += "ambient/";
    }
    fullPath += filepath;
    
    // Check if file exists
    if (!std::filesystem::exists(fullPath)) {
        spdlog::error("Stream file not found: {}", fullPath);
        return false;
    }
    
    // Load the music (don't load into memory yet - SDL_mixer handles streaming)
    Mix_Music* music = Mix_LoadMUS(fullPath.c_str());
    if (!music) {
        spdlog::error("Failed to load stream '{}': {}", name, Mix_GetError());
        return false;
    }
    
    // Store stream data
    StreamData streamData;
    streamData.music = music;
    streamData.category = category;
    streamData.filepath = fullPath;
    
    m_streams[name] = streamData;
    
    spdlog::info("Loaded stream '{}' from '{}'", name, fullPath);
    return true;
}

void AudioManager::UnloadSound(const std::string& name) {
    auto it = m_sounds.find(name);
    if (it != m_sounds.end()) {
        if (it->second.chunk) {
            Mix_FreeChunk(it->second.chunk);
        }
        m_sounds.erase(it);
        spdlog::info("Unloaded sound '{}'", name);
    }
}

void AudioManager::UnloadStream(const std::string& name) {
    auto it = m_streams.find(name);
    if (it != m_streams.end()) {
        // Stop the music if it's currently playing
        if (m_currentMusicHandle != INVALID_AUDIO_HANDLE) {
            auto playingIt = m_playingAudio.find(m_currentMusicHandle);
            if (playingIt != m_playingAudio.end() && playingIt->second.isStream) {
                Mix_HaltMusic();
                m_playingAudio.erase(playingIt);
                m_currentMusicHandle = INVALID_AUDIO_HANDLE;
            }
        }
        
        if (it->second.music) {
            Mix_FreeMusic(it->second.music);
        }
        m_streams.erase(it);
        spdlog::info("Unloaded stream '{}'", name);
    }
}

AudioHandle AudioManager::PlaySound(const std::string& name, float volume) {
    if (!m_initialized) {
        spdlog::warn("Audio manager not initialized, cannot play sound: {}", name);
        return INVALID_AUDIO_HANDLE;
    }
    
    auto it = m_sounds.find(name);
    if (it == m_sounds.end()) {
        spdlog::warn("Sound '{}' not found", name);
        return INVALID_AUDIO_HANDLE;
    }
    
    const SoundData& soundData = it->second;
    
    // Play the sound on any available channel
    int channel = Mix_PlayChannel(-1, soundData.chunk, 0);
    if (channel == -1) {
        spdlog::warn("Failed to play sound '{}': {}", name, Mix_GetError());
        return INVALID_AUDIO_HANDLE;
    }
    
    // Create handle and track playing audio
    AudioHandle handle = GetNextHandle();
    PlayingAudio playingAudio;
    playingAudio.category = soundData.category;
    playingAudio.volume = volume;
    playingAudio.channel = channel;
    playingAudio.isStream = false;
    playingAudio.isPaused = false;
    
    m_playingAudio[handle] = playingAudio;
    
    // Set the volume
    UpdateChannelVolume(channel, soundData.category, volume);
    
    spdlog::debug("Playing sound '{}' on channel {} with handle {}", name, channel, handle);
    return handle;
}

AudioHandle AudioManager::PlayStream(const std::string& name, float volume, bool loop) {
    if (!m_initialized) {
        spdlog::warn("Audio manager not initialized, cannot play stream: {}", name);
        return INVALID_AUDIO_HANDLE;
    }
    
    auto it = m_streams.find(name);
    if (it == m_streams.end()) {
        spdlog::warn("Stream '{}' not found", name);
        return INVALID_AUDIO_HANDLE;
    }
    
    const StreamData& streamData = it->second;
    
    // Stop current music if any
    if (m_currentMusicHandle != INVALID_AUDIO_HANDLE) {
        StopAudio(m_currentMusicHandle);
    }
    
    // Play the music
    int loops = loop ? -1 : 0; // -1 means loop indefinitely
    if (Mix_PlayMusic(streamData.music, loops) == -1) {
        spdlog::warn("Failed to play stream '{}': {}", name, Mix_GetError());
        return INVALID_AUDIO_HANDLE;
    }
    
    // Create handle and track playing audio
    AudioHandle handle = GetNextHandle();
    PlayingAudio playingAudio;
    playingAudio.category = streamData.category;
    playingAudio.volume = volume;
    playingAudio.channel = -1; // Music doesn't use channels
    playingAudio.isStream = true;
    playingAudio.isPaused = false;
    
    m_playingAudio[handle] = playingAudio;
    m_currentMusicHandle = handle;
    
    // Set the volume
    float finalVolume = CalculateFinalVolume(streamData.category, volume);
    Mix_VolumeMusic(static_cast<int>(finalVolume * MIX_MAX_VOLUME));
    
    spdlog::debug("Playing stream '{}' with handle {}", name, handle);
    return handle;
}

void AudioManager::PauseAudio(AudioHandle handle) {
    auto it = m_playingAudio.find(handle);
    if (it == m_playingAudio.end()) return;
    
    PlayingAudio& audio = it->second;
    if (audio.isPaused) return;
    
    if (audio.isStream) {
        Mix_PauseMusic();
    } else {
        Mix_Pause(audio.channel);
    }
    
    audio.isPaused = true;
}

void AudioManager::ResumeAudio(AudioHandle handle) {
    auto it = m_playingAudio.find(handle);
    if (it == m_playingAudio.end()) return;
    
    PlayingAudio& audio = it->second;
    if (!audio.isPaused) return;
    
    if (audio.isStream) {
        Mix_ResumeMusic();
    } else {
        Mix_Resume(audio.channel);
    }
    
    audio.isPaused = false;
}

void AudioManager::StopAudio(AudioHandle handle) {
    auto it = m_playingAudio.find(handle);
    if (it == m_playingAudio.end()) return;
    
    const PlayingAudio& audio = it->second;
    
    if (audio.isStream) {
        Mix_HaltMusic();
        if (m_currentMusicHandle == handle) {
            m_currentMusicHandle = INVALID_AUDIO_HANDLE;
        }
    } else {
        Mix_HaltChannel(audio.channel);
    }
    
    m_playingAudio.erase(it);
}

void AudioManager::SetAudioVolume(AudioHandle handle, float volume) {
    auto it = m_playingAudio.find(handle);
    if (it == m_playingAudio.end()) return;
    
    PlayingAudio& audio = it->second;
    audio.volume = volume;
    
    if (audio.isStream) {
        float finalVolume = CalculateFinalVolume(audio.category, volume);
        Mix_VolumeMusic(static_cast<int>(finalVolume * MIX_MAX_VOLUME));
    } else {
        UpdateChannelVolume(audio.channel, audio.category, volume);
    }
}

bool AudioManager::IsAudioPlaying(AudioHandle handle) const {
    auto it = m_playingAudio.find(handle);
    if (it == m_playingAudio.end()) return false;
    
    const PlayingAudio& audio = it->second;
    
    if (audio.isStream) {
        return Mix_PlayingMusic() != 0;
    } else {
        return Mix_Playing(audio.channel) != 0;
    }
}

void AudioManager::SetMasterVolume(float volume) {
    m_masterVolume = std::clamp(volume, 0.0f, 1.0f);
    
    // Update all currently playing audio
    for (auto& [handle, audio] : m_playingAudio) {
        if (audio.isStream) {
            float finalVolume = CalculateFinalVolume(audio.category, audio.volume);
            Mix_VolumeMusic(static_cast<int>(finalVolume * MIX_MAX_VOLUME));
        } else {
            UpdateChannelVolume(audio.channel, audio.category, audio.volume);
        }
    }
}

void AudioManager::SetCategoryVolume(AudioCategory category, float volume) {
    volume = std::clamp(volume, 0.0f, 1.0f);
    
    switch (category) {
        case AudioCategory::SFX:
            m_sfxVolume = volume;
            break;
        case AudioCategory::Music:
            m_musicVolume = volume;
            break;
        case AudioCategory::Ambient:
            m_ambientVolume = volume;
            break;
    }
    
    // Update all currently playing audio of this category
    for (auto& [handle, audio] : m_playingAudio) {
        if (audio.category == category) {
            if (audio.isStream) {
                float finalVolume = CalculateFinalVolume(audio.category, audio.volume);
                Mix_VolumeMusic(static_cast<int>(finalVolume * MIX_MAX_VOLUME));
            } else {
                UpdateChannelVolume(audio.channel, audio.category, audio.volume);
            }
        }
    }
}

float AudioManager::GetCategoryVolume(AudioCategory category) const {
    switch (category) {
        case AudioCategory::SFX: return m_sfxVolume;
        case AudioCategory::Music: return m_musicVolume;
        case AudioCategory::Ambient: return m_ambientVolume;
        default: return 1.0f;
    }
}

void AudioManager::SetMuted(bool muted) {
    m_muted = muted;
    
    if (muted) {
        Mix_VolumeMusic(0);
        for (int i = 0; i < Mix_AllocateChannels(-1); ++i) {
            Mix_Volume(i, 0);
        }
    } else {
        // Restore volumes
        for (auto& [handle, audio] : m_playingAudio) {
            if (audio.isStream) {
                float finalVolume = CalculateFinalVolume(audio.category, audio.volume);
                Mix_VolumeMusic(static_cast<int>(finalVolume * MIX_MAX_VOLUME));
            } else {
                UpdateChannelVolume(audio.channel, audio.category, audio.volume);
            }
        }
    }
}

void AudioManager::PauseAll() {
    Mix_PauseMusic();
    Mix_Pause(-1); // Pause all channels
    
    for (auto& [handle, audio] : m_playingAudio) {
        audio.isPaused = true;
    }
}

void AudioManager::ResumeAll() {
    Mix_ResumeMusic();
    Mix_Resume(-1); // Resume all channels
    
    for (auto& [handle, audio] : m_playingAudio) {
        audio.isPaused = false;
    }
}

void AudioManager::StopAll() {
    Mix_HaltMusic();
    Mix_HaltChannel(-1); // Stop all channels
    
    m_playingAudio.clear();
    m_currentMusicHandle = INVALID_AUDIO_HANDLE;
}

float AudioManager::CalculateFinalVolume(AudioCategory category, float soundVolume) const {
    if (m_muted) return 0.0f;
    
    float categoryVolume = GetCategoryVolume(category);
    return m_masterVolume * categoryVolume * soundVolume;
}

void AudioManager::UpdateChannelVolume(int channel, AudioCategory category, float soundVolume) {
    float finalVolume = CalculateFinalVolume(category, soundVolume);
    Mix_Volume(channel, static_cast<int>(finalVolume * MIX_MAX_VOLUME));
}

AudioHandle AudioManager::GetNextHandle() {
    return m_nextHandle++;
}

void AudioManager::CleanupFinishedSounds() {
    auto it = m_playingAudio.begin();
    while (it != m_playingAudio.end()) {
        const PlayingAudio& audio = it->second;
        
        bool isFinished = false;
        if (audio.isStream) {
            isFinished = (Mix_PlayingMusic() == 0);
            if (isFinished && m_currentMusicHandle == it->first) {
                m_currentMusicHandle = INVALID_AUDIO_HANDLE;
            }
        } else {
            isFinished = (Mix_Playing(audio.channel) == 0);
        }
        
        if (isFinished) {
            it = m_playingAudio.erase(it);
        } else {
            ++it;
        }
    }
}

} // namespace audio