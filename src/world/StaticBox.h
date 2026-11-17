#pragma once

#include "world/GameObject.h"

// An immobile box: a floor tile (Floor), or an editor-placed cube / wall.
// Never updated (not dynamic), only culled and drawn.
class StaticBox : public GameObject 
{
    public:
    StaticBox(ObjectKind kind, Vector3 center, Vector3 size, Color color, bool drawWires);

    BoundingBox Bounds() const override { return Box; }
    void Draw() const override;

    Vector3 Center() const { return BoxCenter; }
    Vector3 Size() const { return BoxSize; }

    private:
    Vector3 BoxCenter;
    Vector3 BoxSize;
    BoundingBox Box;
    Color BoxColor;
    bool Wires;
};
