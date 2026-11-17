#include "camera/CameraController.h"

#include "camera/GameCamera.h"

#include <raylib.h>

void CameraController::Update(float dt) 
{
    float forward = 0.0f;
    float right = 0.0f;
    if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP))    forward += 1.0f;
    if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN))  forward -= 1.0f;
    if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) right   += 1.0f;
    if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT))  right   -= 1.0f;

    Camera.Move(forward, right, dt);
}
