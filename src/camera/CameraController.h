#pragma once

class GameCamera;

// Translates keyboard input into GameCamera commands: WASD / arrow keys move the camera
// over the floor (it keeps looking at the player). Resetting to the camera point (R)
// is handled by Application, which knows the world.
class CameraController 
{
    public:
    explicit CameraController(GameCamera& camera) : Camera(camera) {}

    void Update(float dt);

    private:
    GameCamera& Camera;
};
