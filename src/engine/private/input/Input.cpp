#include "engine/public/input/Input.h"
#include <spdlog/spdlog.h>
#include <cstring>

namespace input {

Input::Input() : m_currentKeyState(nullptr), m_previousKeyState(nullptr), 
                 m_numKeys(0), m_currentMouseState(0), m_previousMouseState(0),
                 m_mouseX(0), m_mouseY(0) {
}

Input::~Input() {
    delete[] m_previousKeyState;
}

void Input::Update() {
    // Store previous key state
    if (m_currentKeyState && m_previousKeyState) {
        memcpy(m_previousKeyState, m_currentKeyState, m_numKeys);
    }
    
    // Get current key state
    m_currentKeyState = SDL_GetKeyboardState(&m_numKeys);
    
    // Initialize previous state if needed
    if (!m_previousKeyState) {
        m_previousKeyState = new Uint8[m_numKeys];
        memcpy(m_previousKeyState, m_currentKeyState, m_numKeys);
    }
    
    // Store previous mouse state
    m_previousMouseState = m_currentMouseState;
    
    // Get current mouse state
    m_currentMouseState = SDL_GetMouseState(&m_mouseX, &m_mouseY);
}

void Input::HandleEvent(const SDL_Event& event) {
    // TODO: Handle specific events if needed
    // This method can be used for events that need immediate handling
}

bool Input::IsKeyPressed(SDL_Scancode key) const {
    if (!m_currentKeyState || key >= m_numKeys) return false;
    return m_currentKeyState[key] != 0;
}

bool Input::IsKeyJustPressed(SDL_Scancode key) const {
    if (!m_currentKeyState || !m_previousKeyState || key >= m_numKeys) return false;
    return m_currentKeyState[key] && !m_previousKeyState[key];
}

bool Input::IsMouseButtonPressed(int button) const {
    return (m_currentMouseState & SDL_BUTTON(button)) != 0;
}

void Input::GetMousePosition(int& x, int& y) const {
    x = m_mouseX;
    y = m_mouseY;
}

} // namespace input