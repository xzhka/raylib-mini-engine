#include "world/EnemySpawner.h"

#include "core/Config.h"
#include "tools/EventLog.h"
#include "world/Enemy.h"
#include "world/GameWorld.h"

#include <algorithm>
#include <cmath>
#include <cstdio>

EnemySpawner::EnemySpawner(const SimulationParams& params, GameWorld& world, const ModelAsset& model)
    : GameObject(ObjectKind::Spawner, /*dynamic*/ true, /*renderable*/ false),
      SpawnParams(params), World(world), Model(model), Rng(std::random_device{}()) {}

void EnemySpawner::Update(float dt) 
{
    SpawnTimer += dt;
    if (SpawnTimer < SpawnParams.spawnPeriodSec) return;

    if (static_cast<int>(World.CountOf(ObjectKind::Enemy)) >= SpawnParams.maxEnemies) 
    {
        // Spawn as fast as a slot frees up.
        SpawnTimer = SpawnParams.spawnPeriodSec;
        return;
    }

    SpawnTimer = std::min(SpawnTimer - SpawnParams.spawnPeriodSec, SpawnParams.spawnPeriodSec);
    SpawnEnemy();
}

void EnemySpawner::SpawnEnemy() 
{
    const float lo = std::min(SpawnParams.minEnemySpeed, SpawnParams.maxEnemySpeed);
    const float hi = std::max(SpawnParams.minEnemySpeed, SpawnParams.maxEnemySpeed);

    std::uniform_real_distribution<float> angleDist(0.0f, 2.0f * PI);
    std::uniform_real_distribution<float> speedDist(lo, hi);

    const float angle = angleDist(Rng);
    const float speed = speedDist(Rng);
    const Vector3 center = World.PlayerPosition();

    // Keep the spawn point on the floor even if the player was moved close to its edge.
    const BoundingBox floor = World.FloorBounds();
    const float margin = 0.5f;
    const Vector3 position{
        std::clamp(center.x + Config::kSpawnRadius * std::cos(angle), floor.min.x + margin, floor.max.x - margin),
        0.0f, std::clamp(center.z + Config::kSpawnRadius * std::sin(angle), floor.min.z + margin, floor.max.z - margin) };

    const unsigned id = NextID++;
    World.Add<Enemy>(id, position, speed, World, Model);

    char text[96];
    std::snprintf(text, sizeof(text), "Enemy #%u spawned (speed %.1f)", id, speed);
    World.Log().Log(text);
}
