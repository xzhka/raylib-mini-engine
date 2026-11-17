#include "world/CameraSpawnObject.h"

namespace 
{
    constexpr float kRadius = 0.6f;
}

BoundingBox CameraSpawnObject::Bounds() const 
{
    return BoundingBox{ Vector3{ Position.x - kRadius, Position.y - kRadius, Position.z - kRadius },
                        Vector3{ Position.x + kRadius, Position.y + kRadius, Position.z + kRadius } };
}

void CameraSpawnObject::Draw() const 
{
    DrawSphere(Position, kRadius, SKYBLUE);
    DrawSphereWires(Position, kRadius, 8, 8, DARKBLUE);
    DrawLine3D(Position, Vector3{ Position.x, 0.0f, Position.z }, SKYBLUE);
}
