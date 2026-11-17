#include "core/Window.h"

#include <raylib.h>

Window::Window(int width, int height, const char* title, int targetFps) 
{
    InitWindow(width, height, title);
    SetTargetFPS(targetFps);
}

Window::~Window() 
{
    CloseWindow();
}
