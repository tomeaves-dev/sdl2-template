#pragma once

#include <string>
#include <SDL.h>

namespace rendering {
    class Window {
    public:
        Window();
        ~Window();
        
        bool Initialize(const std::string& title, int width, int height);
        void Shutdown();
        
        SDL_Window* GetSDLWindow() const { return m_window; }
        int GetWidth() const { return m_width; }
        int GetHeight() const { return m_height; }
        
    private:
        SDL_Window* m_window;
        int m_width;
        int m_height;
    };
}