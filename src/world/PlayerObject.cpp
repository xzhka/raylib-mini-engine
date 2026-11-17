#include "world/PlayerObject.h"

PlayerObject::PlayerObject(float width, float height, Vector3 center)
    : GameObject(ObjectKind::PlayerSpawn, /*dynamic*/ false), Center(center), Width(width), Height(height) {}

void PlayerObject::Draw() const 
{
    DrawCube(Center, Width, Height, Width, GOLD);
    DrawCubeWires(Center, Width, Height, Width, BLACK);
}

BoundingBox PlayerObject::Bounds() const 
{
    const float hw = Width * 0.5f;
    const float hh = Height * 0.5f;
    return BoundingBox{ Vector3{ Center.x - hw, Center.y - hh, Center.z - hw },
                        Vector3{ Center.x + hw, Center.y + hh, Center.z + hw } };
}
