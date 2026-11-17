#include "world/StaticBox.h"

StaticBox::StaticBox(ObjectKind kind, Vector3 center, Vector3 size, Color color, bool drawWires)
    : GameObject(kind, /*dynamic*/ false), BoxCenter(center), BoxSize(size), BoxColor(color), Wires(drawWires) 
{
    Box = BoundingBox{ Vector3{ center.x - size.x * 0.5f, center.y - size.y * 0.5f, center.z - size.z * 0.5f },
                       Vector3{ center.x + size.x * 0.5f, center.y + size.y * 0.5f, center.z + size.z * 0.5f } };
}

void StaticBox::Draw() const 
{
    DrawCubeV(BoxCenter, BoxSize, BoxColor);
    if (Wires) DrawCubeWiresV(BoxCenter, BoxSize, BLACK);
}
