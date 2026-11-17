#pragma once

#include "world/GameObject.h"

#include <random>

struct SimulationParams;
class GameWorld;
class ModelAsset;

// Invisible logic object: spawns enemies on a circle around the player at a fixed period,
// up to SimulationParams::maxEnemies alive. It lives in the world like everything else, so
// the world updates it through the same loop as the enemies.
class EnemySpawner : public GameObject 
{
    public:
    EnemySpawner(const SimulationParams& params, GameWorld& world, const ModelAsset& model);

    void Update(float dt) override;
    BoundingBox Bounds() const override { return BoundingBox{}; }

    private:
    void SpawnEnemy();

    // Read every frame -> live parameter changes just work
    const SimulationParams& SpawnParams;
    GameWorld& World;
    const ModelAsset& Model;

    float SpawnTimer = 0.0f;
    unsigned NextID = 1;
    std::mt19937 Rng;
};
