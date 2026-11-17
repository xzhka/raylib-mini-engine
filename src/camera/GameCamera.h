#pragma once

#include <raylib.h>

// Perspective camera that always LOOKS AT THE PLAYER. Its position comes from the camera
// point (set in the editor) and can be moved over the floor with the keyboard; the
// orientation is recomputed from the player position every frame.
class GameCamera 
{
    public:
    GameCamera(Vector3 position, float fovYDeg, float moveSpeed);

    // Where to put a camera with a fixed pitch so that the whole box fits in view
    // (used for the default camera point, when the level does not define one).
    static Vector3 FitPosition(const BoundingBox& sceneBounds, float pitchDeg, float fovYDeg, float aspect);

    // forwardAxis / rightAxis in [-1, 1]; movement is relative to the view direction
    // projected on the floor.
    void Move(float forwardAxis, float rightAxis, float dt);
    void SetPosition(Vector3 position);
    void LookAt(Vector3 target);

    const Camera3D& GetCamera() const { return Camera; }

    private:
    Camera3D Camera;
    float CameraMoveSpeed;
};
