#include "camera/GameCamera.h"

#include <raymath.h>

#include <algorithm>
#include <cmath>

namespace 
{
    constexpr float kFitMargin = 0.9f;
    constexpr float kMinHorizontalDistance = 0.5f;
}

GameCamera::GameCamera(Vector3 position, float fovYDeg, float moveSpeed) : CameraMoveSpeed(moveSpeed) 
{
    Camera.position = position;
    Camera.target = Vector3{ 0.0f, 0.0f, 0.0f };
    Camera.up = Vector3{ 0.0f, 1.0f, 0.0f };
    Camera.fovy = fovYDeg;
    Camera.projection = CAMERA_PERSPECTIVE;
}

Vector3 GameCamera::FitPosition(const BoundingBox& b, float pitchDeg, float fovYDeg, float aspect) 
{
    const float pitch = pitchDeg * DEG2RAD;

    const Vector3 toCamera = { 0.0f, std::sin(pitch), std::cos(pitch) };
    const Vector3 forward = Vector3Negate(toCamera);
    const Vector3 right = Vector3Normalize(Vector3CrossProduct(forward, Vector3{ 0, 1, 0 }));
    const Vector3 up = Vector3CrossProduct(right, forward);

    const Vector3 lookAt = { (b.min.x + b.max.x) * 0.5f, 0.0f, (b.min.z + b.max.z) * 0.5f };

    const float tanY = std::tan(fovYDeg * DEG2RAD * 0.5f) * kFitMargin;
    const float tanX = tanY * aspect;

    float distance = 1.0f;
    for (int i = 0; i < 8; ++i) 
    {
        const Vector3 corner = { (i & 1) ? b.max.x : b.min.x,
                                 (i & 2) ? b.max.y : b.min.y,
                                 (i & 4) ? b.max.z : b.min.z };
        const Vector3 rel = Vector3Subtract(corner, lookAt);
        const float x = Vector3DotProduct(rel, right);
        const float y = Vector3DotProduct(rel, up);
        const float z = Vector3DotProduct(rel, forward);
        distance = std::max(distance, std::max(std::fabs(x) / tanX, std::fabs(y) / tanY) - z);
    }
    return Vector3Add(lookAt, Vector3Scale(toCamera, distance));
}

void GameCamera::Move(float forwardAxis, float rightAxis, float dt) 
{
    Vector2 input = { rightAxis, forwardAxis };
    // Avoid faster diagonals.
    if (Vector2LengthSqr(input) > 1.0f) input = Vector2Normalize(input);
    if (input.x == 0.0f && input.y == 0.0f) return;

    Vector3 flatForward = Vector3Subtract(Camera.target, Camera.position);
    flatForward.y = 0.0f;
    if (Vector3LengthSqr(flatForward) < 1e-6f) return;
    flatForward = Vector3Normalize(flatForward);
    const Vector3 right = Vector3CrossProduct(flatForward, Camera.up);

    const Vector3 offset = Vector3Scale(
        Vector3Add(Vector3Scale(flatForward, input.y), Vector3Scale(right, input.x)),
        CameraMoveSpeed * dt);
    // The target is re-aimed by LookAt() every frame
    Camera.position = Vector3Add(Camera.position, offset);
}

void GameCamera::SetPosition(Vector3 position) 
{
    Camera.position = position;
}

void GameCamera::LookAt(Vector3 target) 
{
    // Keep a minimal horizontal distance so the view direction is never parallel to "up".
    const float dx = target.x - Camera.position.x;
    const float dz = target.z - Camera.position.z;
    const float horizontal = std::hypot(dx, dz);
    if (horizontal < kMinHorizontalDistance) 
    {
        const float dirX = horizontal > 1e-4f ? dx / horizontal : 0.0f;
        const float dirZ = horizontal > 1e-4f ? dz / horizontal : -1.0f;
        Camera.position.x = target.x - dirX * kMinHorizontalDistance;
        Camera.position.z = target.z - dirZ * kMinHorizontalDistance;
    }
    Camera.target = target;
}
