#pragma once

#include <memory>

// Forward declarations
namespace rendering {
    class Window;
    class Renderer;
}

namespace physics {
    class Physics;
}

namespace input {
    class Input;
}

namespace audio {
    class AudioManager;
}

namespace text {
    class FontManager;
    class TextRenderer;
    class Text;
}

namespace save {
    class SaveManager;
}

namespace core {
    
    class Game {
    public:
        Game();
        ~Game();
        
        bool Initialize();
        void Run();
        void Shutdown();
        
        bool IsRunning() const { return m_isRunning; }
        
        // System access (for advanced usage)
        audio::AudioManager* GetAudioManager() const { return m_audioManager.get(); }
        physics::Physics* GetPhysics() const { return m_physics.get(); }
        text::FontManager* GetFontManager() const { return m_fontManager.get(); }
        text::TextRenderer* GetTextRenderer() const { return m_textRenderer.get(); }
        save::SaveManager* GetSaveManager() const { return m_saveManager.get(); }
        
    private:
        void Update(float deltaTime);
        void Render();
        void HandleEvents();
        
        bool m_isRunning;
        
        // Core systems
        std::unique_ptr<rendering::Window> m_window;
        std::unique_ptr<rendering::Renderer> m_renderer;
        std::unique_ptr<physics::Physics> m_physics;
        std::unique_ptr<input::Input> m_input;
        std::unique_ptr<audio::AudioManager> m_audioManager;
        std::unique_ptr<text::FontManager> m_fontManager;
        std::unique_ptr<text::TextRenderer> m_textRenderer;
        std::unique_ptr<save::SaveManager> m_saveManager;
    };
}