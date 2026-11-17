#pragma once

// RAII wrapper over the raylib window. Declared as the FIRST member of
// Application so the GL context exists before any other member is built.
class Window 
{
    public:
    Window(int width, int height, const char* title, int targetFps);
    ~Window();

    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;
};
