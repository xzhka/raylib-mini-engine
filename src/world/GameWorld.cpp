#include "world/GameWorld.h"

#include "core/Config.h"
#include "render/Renderer.h"
#include "world/CameraSpawnObject.h"
#include "world/PlayerObject.h"
#include "world/StaticBox.h"

#include <algorithm>
#include <cmath>

GameWorld::GameWorld(EventLog& log, Vector3 defaultCameraSpawn)
    : WorldLog(log), DefaultCameraSpawn(defaultCameraSpawn) 
{
    const float half = Config::kFloorSide * Config::kFloorTileSize * 0.5f;
    Floor = BoundingBox{ Vector3{ -half, -Config::kFloorTileSize, -half }, Vector3{ half, 0.0f, half } };

    AllObjects.reserve(static_cast<std::size_t>(Config::kFloorSide) * Config::kFloorSide + 256);
    BuildFloor();

    Player = Add<PlayerObject>(Config::kPlayerWidth, Config::kPlayerHeight, Vector3{ 0.0f, Config::kPlayerHeight * 0.5f, 0.0f });
    CameraPoint = Add<CameraSpawnObject>(defaultCameraSpawn);
}

GameWorld::~GameWorld() = default;

void GameWorld::BuildFloor() 
{
    const int side = Config::kFloorSide;
    const float size = Config::kFloorTileSize;
    const float half = side * size * 0.5f;
    const Color light{ 95, 100, 110, 255 };
    const Color dark { 70,  75,  85, 255 };

    for (int ix = 0; ix < side; ++ix) 
    {
        for (int iz = 0; iz < side; ++iz) 
        {
            // Top surface of the floor is at y = 0
            const Vector3 center{ -half + (ix + 0.5f) * size, -size * 0.5f, -half + (iz + 0.5f) * size };
            Add<StaticBox>(ObjectKind::Floor, center, Vector3{ size, size, size }, ((ix + iz) % 2 == 0) ? light : dark, false);
        }
    }
}

void GameWorld::Register(std::unique_ptr<GameObject> object) 
{
    ++KindCounts[static_cast<std::size_t>(object->Kind())];
    if (object->IsDynamic()) Updatables.push_back(object.get());
    AllObjects.push_back(std::move(object));
}

GameObject* GameWorld::AddStatic(ObjectKind kind, Vector3 center, Vector3 size) 
{
    const Color color = (kind == ObjectKind::StaticWall) ? Color{ 150, 120, 95, 255 } : Color{ 90, 150, 200, 255 };
    return Add<StaticBox>(kind, center, size, color, true);
}

void GameWorld::Update(float dt) 
{
    if (!Paused) 
    {
        // Visit only dynamic objects. Objects added during the loop land at the end 
        // of the list and are first updated next frame, so the size is
        // frozen up front and the loop uses indices (push_back may reallocate).
        const std::size_t count = Updatables.size();
        for (std::size_t i = 0; i < count; ++i) 
        {
            GameObject* object = Updatables[i];
            if (object->IsDestroyed()) continue;
            object->Update(dt);
            if (object->IsDestroyed()) SweepNeeded = true;
        }
    }
    Sweep();
}

void GameWorld::Sweep() 
{
    if (!SweepNeeded) return;
    SweepNeeded = false;

    Updatables.erase(std::remove_if(Updatables.begin(), Updatables.end(),
                     [](const GameObject* o) { return o->IsDestroyed(); }), Updatables.end());

    AllObjects.erase(std::remove_if(AllObjects.begin(), AllObjects.end(),
                     [this](const std::unique_ptr<GameObject>& o) 
                     {
                         if (!o->IsDestroyed()) return false;
                         --KindCounts[static_cast<std::size_t>(o->Kind())];
                         return true;
                     }), AllObjects.end());
}

void GameWorld::Render(Renderer& renderer, bool showEditorObjects) const 
{
    for (const auto& object : AllObjects) 
    {
        if (object->IsDestroyed() || !object->IsRenderable()) continue;
        if (object->IsEditorOnly() && !showEditorObjects) continue;
        renderer.Submit(*object);
    }
}

Vector3 GameWorld::PlayerPosition() const { return Player->Position(); }
Vector3 GameWorld::CameraSpawnPosition() const { return CameraPoint->GetPosition(); }
void GameWorld::SetPlayerSpawn(Vector3 center) { Player->SetPosition(center); }
void GameWorld::SetCameraSpawn(Vector3 position) { CameraPoint->SetPosition(position); }

void GameWorld::ClearLevel() 
{
    for (const auto& object : AllObjects) 
    {
        const ObjectKind kind = object->Kind();
        if (kind == ObjectKind::StaticCube || kind == ObjectKind::StaticWall || kind == ObjectKind::Enemy) 
        {
            object->Destroy();
        }
    }
    SweepNeeded = true;
    Sweep();

    Player->SetPosition(Vector3{ 0.0f, Config::kPlayerHeight * 0.5f, 0.0f });
    CameraPoint->SetPosition(DefaultCameraSpawn);
}

GameObject* GameWorld::PickStatic(const Ray& ray, float& distance) const 
{
    GameObject* best = nullptr;
    float bestDistance = 0.0f;
    for (const auto& object : AllObjects) 
    {
        const ObjectKind kind = object->Kind();
        if (kind != ObjectKind::StaticCube && kind != ObjectKind::StaticWall) continue;
        if (object->IsDestroyed()) continue;

        const RayCollision hit = GetRayCollisionBox(ray, object->Bounds());
        if (hit.hit && (!best || hit.distance < bestDistance)) 
        {
            best = object.get();
            bestDistance = hit.distance;
        }
    }
    distance = bestDistance;
    return best;
}

bool GameWorld::HasStatic(ObjectKind kind, Vector3 center) const 
{
    for (const auto& object : AllObjects) 
    {
        if (object->Kind() != kind || object->IsDestroyed()) continue;
        const BoundingBox b = object->Bounds();
        const Vector3 c{ (b.min.x + b.max.x) * 0.5f, (b.min.y + b.max.y) * 0.5f, (b.min.z + b.max.z) * 0.5f };
        if (std::fabs(c.x - center.x) < 0.01f && std::fabs(c.y - center.y) < 0.01f && std::fabs(c.z - center.z) < 0.01f) return true;
    }
    return false;
}
