#pragma once

#include "world/GameObject.h"

class GameWorld;
class ModelAsset;

// Represent enemy which runs towards the player and destroys itself when it gets there.
// It is drawn with the shared 3D model, turned in the direction of movement.
class Enemy : public GameObject 
{
    public:
    Enemy(unsigned id, Vector3 feetPosition, float speed, GameWorld& world, const ModelAsset& model);

    void Update(float dt) override;
    void Draw() const override;
    BoundingBox Bounds() const override;

    unsigned Id() const { return ID; }
    float Speed() const { return EnemySpeed; }
    Vector3 Position() const { return Pos; }

    private:
    unsigned ID;
    Vector3 Pos;
    float EnemySpeed;
    float YawDeg = 0.0f;
    GameWorld& World;
    const ModelAsset& Model;
};
