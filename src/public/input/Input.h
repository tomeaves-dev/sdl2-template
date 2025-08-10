#pragma once

#include <SDL.h>

namespace input {
    class Input {
    public:
        Input();
        ~Input();
        
        void Update();
        void HandleEvent(const SDL_Event& event);
        
        bool IsKeyPressed(SDL_Scancode key) const;
        bool IsKeyJustPressed(SDL_Scancode key) const;
        bool IsMouseButtonPressed(int button) const;
        
        void GetMousePosition(int& x, int& y) const;
        
    private:
        const Uint8* m_currentKeyState;
        Uint8* m_previousKeyState;
        int m_numKeys;
        
        Uint32 m_currentMouseState;
        Uint32 m_previousMouseState;
        int m_mouseX, m_mouseY;
    };
}