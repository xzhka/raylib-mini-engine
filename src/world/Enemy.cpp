#include "world/Enemy.h"

#include "core/Config.h"
#include "tools/EventLog.h"
#include "world/GameWorld.h"
#include "world/ModelAsset.h"

#include <algorithm>
#include <cmath>
#include <cstdio>

Enemy::Enemy(unsigned id, Vector3 feetPosition, float speed, GameWorld& world, const ModelAsset& model)
    : GameObject(ObjectKind::Enemy, /*dynamic*/ true), ID(id), Pos(feetPosition), EnemySpeed(speed), World(world), Model(model) {}

void Enemy::Update(float dt) 
{
    const Vector3 target = World.PlayerPosition();
    const float dx = target.x - Pos.x;
    const float dz = target.z - Pos.z;
    const float dist = std::hypot(dx, dz);

    if (dist > 1e-4f) 
    {
        // Never step beyond distance
        const float step = std::min(EnemySpeed * dt, dist);
        Pos.x += dx / dist * step;
        Pos.z += dz / dist * step;
        // The model looks along +Z at yaw 0
        YawDeg = std::atan2(dx, dz) * RAD2DEG;
    }

    if (std::hypot(target.x - Pos.x, target.z - Pos.z) <= Config::kDestroyDistance) 
    {
        char text[96];
        std::snprintf(text, sizeof(text), "Enemy #%u reached target", ID);
        World.Log().Log(text);
        Destroy();
    }
}

void Enemy::Draw() const 
{
    Model.Draw(Pos, YawDeg, WHITE);
}

BoundingBox Enemy::Bounds() const 
{
    // XZ extent uses the bounding radius so the box is valid for any yaw (never culls a visible enemy).
    const BoundingBox local = Model.LocalBounds();
    const float r = Model.RadiusXZ();
    return BoundingBox{ Vector3{ Pos.x - r, Pos.y + local.min.y, Pos.z - r },
                        Vector3{ Pos.x + r, Pos.y + local.max.y, Pos.z + r } };
}
