#pragma once

class Renderer;

// Visual debugging tool for the frustum culling: a panel with the number of visible and
// culled objects (total and per object type) plus the state of the debug switches.
class CullingOverlay 
{
    public:
    void Draw(const Renderer& renderer, int x, int y) const;
    static int Width();
};
