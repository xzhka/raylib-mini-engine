#pragma once

#include <raylib.h>

class GameObject;
class GameWorld;
class GameCamera;
class EventLog;

// Internal level editor (F1). Only adds and removes objects:
//   LMB - add the current object under the cursor     RMB - remove the cube / wall under the cursor
//   1 - cube 1x1x1   2 - wall 10x10x1   3 - player spawn   4 - camera point   (Tab - turn the wall 90 degrees)
// The object is placed on the clicked surface (floor or the top / side of another object) and
// snapped to the 1-unit grid of the floor. The simulation is paused while the editor is open.
// Saving / loading (F2 / F3) is handled by Application through LevelFile.
class LevelEditor 
{
    public:
    LevelEditor(GameWorld& world, GameCamera& camera, EventLog& log);

    void Toggle();
    bool IsActive() const { return Active; }

    void Update(); // input + hover calculation; call after the camera was updated
    void DrawGizmos() const; // 3D: placement preview + highlight of the object to remove (inside the scene)
    void DrawHud() const; // 2D help panel

    private:
    enum class Tool { Cube = 1, Wall = 2, PlayerSpawn = 3, CameraSpawn = 4 };

    struct Placement 
    {
        bool valid = false;
        Vector3 center{};
        Vector3 size{};
    };

    Vector3 CurrentSize() const;
    Placement ComputePlacement(const Ray& ray) const;
    void SelectTool();
    void AddAtCursor();
    void RemoveAtCursor();

    GameWorld& World;
    GameCamera& Camera;
    EventLog& Log;

    bool Active = false;
    Tool CurrentTool = Tool::Cube;
    bool WallTurned = false; // false: 10x10x1 (wide along X), true: 1x10x10 (wide along Z)

    Placement Hover; // where the current tool would place its object
    GameObject* HoverTarget = nullptr; // object under the cursor (RMB removes it)
};
