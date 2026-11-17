#pragma once

#include "world/GameObject.h"

#include <array>
#include <cstddef>
#include <memory>
#include <utility>
#include <vector>

class EventLog;
class Renderer;
class PlayerObject;
class CameraSpawnObject;

// Owns and manages EVERY object of the game world, of any type, in one uniform way.
//
// Data structures:
//  - Objects   : std::vector<std::unique_ptr<GameObject>>  - the one list the Renderer walks
//                (contiguous pointers, owner of the objects, polymorphic).
//  - Updatables: std::vector<GameObject*> - only the objects with IsDynamic() == true.
//                Update() iterates this list, so the ~10 000 static tiles / cubes cost
//                nothing in the simulation step.
//  - removal is deferred (Destroy() flag + one sweep per frame), so objects may safely
//    destroy themselves or be removed by the editor in the middle of an update.
class GameWorld 
{
    public:
    GameWorld(EventLog& log, Vector3 defaultCameraSpawn);
    ~GameWorld();
    GameWorld(const GameWorld&) = delete;
    GameWorld& operator=(const GameWorld&) = delete;

    // --- unified object management -----------------------------------------------------
    template <typename T, typename... Args>
    T* Add(Args&&... args) 
    {
        std::unique_ptr<T> object = std::make_unique<T>(std::forward<Args>(args)...);
        T* raw = object.get();
        Register(std::move(object));
        return raw;
    }

    // Swept at the end of Update()
    void Remove(GameObject& object) { object.Destroy(); SweepNeeded = true; }

    // Editor objects: a cube / wall of the given centre and size.
    GameObject* AddStatic(ObjectKind kind, Vector3 center, Vector3 size);

    // --- main loop ---------------------------------------------------------------------
    void Update(float dt); // simulates the dynamic objects only
    void Render(Renderer& renderer, bool showEditorObjects) const; // submits every object to the renderer

    void SetSimulationPaused(bool paused) { Paused = paused; }
    bool IsSimulationPaused() const { return Paused; }

    // --- spawn points ------------------------------------------------------------------
    Vector3 PlayerPosition() const; // centre of the player box
    Vector3 CameraSpawnPosition() const;
    void SetPlayerSpawn(Vector3 center);
    void SetCameraSpawn(Vector3 position);

    // --- level -------------------------------------------------------------------------
    // Removes everything the editor placed and all enemies; resets both spawn points.
    void ClearLevel();

    // --- queries -----------------------------------------------------------------------
    // Nearest editor-placed (removable) object hit by the ray, or nullptr.
    GameObject* PickStatic(const Ray& ray, float& distance) const;
    bool HasStatic(ObjectKind kind, Vector3 center) const; // duplicate check for the editor

    const std::vector<std::unique_ptr<GameObject>>& Objects() const { return AllObjects; }
    std::size_t CountOf(ObjectKind kind) const { return KindCounts[static_cast<std::size_t>(kind)]; }
    std::size_t ObjectCount() const { return AllObjects.size(); }
    std::size_t UpdatableCount() const { return Updatables.size(); }
    BoundingBox FloorBounds() const { return Floor; }
    EventLog& Log() { return WorldLog; }

    private:
    void Register(std::unique_ptr<GameObject> object);
    void BuildFloor();
    // Removes the Destroyed() objects
    void Sweep(); 

    EventLog& WorldLog;
    std::vector<std::unique_ptr<GameObject>> AllObjects;
    std::vector<GameObject*> Updatables;
    std::array<std::size_t, static_cast<std::size_t>(ObjectKind::Count)> KindCounts{};

    PlayerObject* Player = nullptr; 
    CameraSpawnObject* CameraPoint = nullptr; 
    Vector3 DefaultCameraSpawn;
    BoundingBox Floor{};
    bool Paused = false;
    bool SweepNeeded = false;
};
