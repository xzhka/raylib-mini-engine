#pragma once

#include "world/GameObject.h"

// The player: the box the enemies run towards (the old "Target"). It does not move by itself,
// its position is the player spawn point set in the editor, so it is a static object.
class PlayerObject : public GameObject 
{
    public:
    PlayerObject(float width, float height, Vector3 center);

    BoundingBox Bounds() const override;
    void Draw() const override;

    Vector3 Position() const { return Center; } // box centre
    void SetPosition(Vector3 center) { Center = center; }

    private:
    Vector3 Center;
    float Width;
    float Height;
};
