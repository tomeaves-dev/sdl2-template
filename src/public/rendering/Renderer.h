#pragma once

namespace rendering {
    class Window;
    
    class Renderer {
    public:
        Renderer();
        ~Renderer();
        
        bool Initialize(Window* window);
        void Shutdown();
        
        void BeginFrame();
        void EndFrame();
        void Clear(float r = 0.0f, float g = 0.0f, float b = 0.0f, float a = 1.0f);
        
    private:
        void* m_context; // SDL_GLContext (using void* to avoid including SDL_opengl.h in header)
    };
}