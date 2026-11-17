#pragma once

// Draws the profiler's last completed frame as a nested table over the scene.
class ProfilerOverlay 
{
    public:
    void Draw(int x, int y) const;
    static int Width();
};
