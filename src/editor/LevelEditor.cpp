#include "editor/LevelEditor.h"

#include "camera/GameCamera.h"
#include "core/Config.h"
#include "tools/EventLog.h"
#include "world/GameWorld.h"

#include <raylib.h>

#include <algorithm>
#include <cmath>
#include <cstdio>

namespace 
{
    // Snap a coordinate to the floor grid for an object that is `size` wide along that axis:
    // even widths are centred on a grid line, odd widths in the middle of a cell.
    float SnapAxis(float value, float size) 
    {
        const bool even = static_cast<int>(std::lround(size)) % 2 == 0;
        return even ? std::round(value) : std::floor(value) + 0.5f;
    }

    const char* ToolName(int tool) 
    {
        switch (tool) 
        {
            case 1: return "Cube 1x1x1";
            case 2: return "Wall 10x10x1";
            case 3: return "Player spawn";
            default: return "Camera point";
        }
    }
}

LevelEditor::LevelEditor(GameWorld& world, GameCamera& camera, EventLog& log) : World(world), Camera(camera), Log(log) {}

void LevelEditor::Toggle() 
{
    Active = !Active;
    World.SetSimulationPaused(Active);
    Hover = Placement{};
    HoverTarget = nullptr;
    Log.Log(Active ? "Editor ON (simulation paused)" : "Editor OFF");
}

Vector3 LevelEditor::CurrentSize() const 
{
    switch (CurrentTool) 
    {
        case Tool::Cube: return Vector3{ Config::kCubeSizeX, Config::kCubeSizeY, Config::kCubeSizeZ };
        case Tool::Wall: 
            return WallTurned ? Vector3{ Config::kWallSizeZ, Config::kWallSizeY, Config::kWallSizeX }
                              : Vector3{ Config::kWallSizeX, Config::kWallSizeY, Config::kWallSizeZ };
        case Tool::PlayerSpawn: return Vector3{ Config::kPlayerWidth, Config::kPlayerHeight, Config::kPlayerWidth };
        default: return Vector3{ 1.0f, 1.0f, 1.0f };    // camera point: only a position
    }
}

void LevelEditor::SelectTool() 
{
    if (IsKeyPressed(KEY_ONE) || IsKeyPressed(KEY_KP_1)) CurrentTool = Tool::Cube;
    if (IsKeyPressed(KEY_TWO) || IsKeyPressed(KEY_KP_2)) CurrentTool = Tool::Wall;
    if (IsKeyPressed(KEY_THREE) || IsKeyPressed(KEY_KP_3)) CurrentTool = Tool::PlayerSpawn;
    if (IsKeyPressed(KEY_FOUR) || IsKeyPressed(KEY_KP_4)) CurrentTool = Tool::CameraSpawn;
    if (IsKeyPressed(KEY_TAB)) WallTurned = !WallTurned;
}

LevelEditor::Placement LevelEditor::ComputePlacement(const Ray& ray) const 
{
    Placement placement;
    placement.size = CurrentSize();

    // 1. what does the ray hit first: an existing object or the floor plane (y = 0)?
    float objectDistance = 0.0f;
    GameObject* object = World.PickStatic(ray, objectDistance);

    float floorDistance = -1.0f;
    if (ray.direction.y < -1e-5f) floorDistance = -ray.position.y / ray.direction.y;

    const bool useObject = object && (floorDistance < 0.0f || objectDistance <= floorDistance);
    if (!useObject && floorDistance < 0.0f) return placement;

    Vector3 point;
    float baseY = 0.0f;
    if (useObject) 
    {
        const RayCollision hit = GetRayCollisionBox(ray, object->Bounds());
        point = hit.point;
        if (hit.normal.y > 0.5f) 
        {
            baseY = object->Bounds().max.y; 
        }
        else 
        {
            baseY = object->Bounds().min.y; 
            point.x += hit.normal.x * placement.size.x * 0.5f;
            point.z += hit.normal.z * placement.size.z * 0.5f;
        }
    }
    else 
    {
        point = Vector3{ ray.position.x + ray.direction.x * floorDistance, 0.0f, ray.position.z + ray.direction.z * floorDistance };
    }

    // 2. snap to the grid
    placement.center = Vector3{ SnapAxis(point.x, placement.size.x), baseY + placement.size.y * 0.5f, SnapAxis(point.z, placement.size.z) };

    // 3. must be on the floor
    const BoundingBox floor = World.FloorBounds();
    placement.valid = placement.center.x > floor.min.x && placement.center.x < floor.max.x &&
                      placement.center.z > floor.min.z && placement.center.z < floor.max.z;
    return placement;
}

void LevelEditor::Update() 
{
    if (!Active) return;

    SelectTool();

    const Ray ray = GetScreenToWorldRay(GetMousePosition(), Camera.GetCamera());
    Hover = ComputePlacement(ray);

    float distance = 0.0f;
    HoverTarget = World.PickStatic(ray, distance);

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) AddAtCursor();
    if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) RemoveAtCursor();
}

void LevelEditor::AddAtCursor() 
{
    if (!Hover.valid) return;

    char text[96];
    switch (CurrentTool) 
    {
        case Tool::Cube:
        case Tool::Wall: 
        {
            const ObjectKind kind = CurrentTool == Tool::Cube ? ObjectKind::StaticCube : ObjectKind::StaticWall;
            if (World.HasStatic(kind, Hover.center)) return; // if it already there
            World.AddStatic(kind, Hover.center, Hover.size);
            std::snprintf(text, sizeof(text), "Added %s at (%.1f, %.1f, %.1f)", ToolName(static_cast<int>(CurrentTool)), Hover.center.x, Hover.center.y, Hover.center.z);
            break;
        }
        case Tool::PlayerSpawn:
            World.SetPlayerSpawn(Hover.center);
            std::snprintf(text, sizeof(text), "Player spawn set to (%.1f, %.1f)", Hover.center.x, Hover.center.z);
            break;
        case Tool::CameraSpawn: 
        {
            const Vector3 position{ Hover.center.x, Hover.center.y - Hover.size.y * 0.5f + Config::kCameraSpawnHeight, Hover.center.z };
            World.SetCameraSpawn(position);
            Camera.SetPosition(position); // shows the result immediately 
            std::snprintf(text, sizeof(text), "Camera point set to (%.1f, %.1f, %.1f)", position.x, position.y, position.z);
            break;
        }
    }
    Log.Log(text);
}

void LevelEditor::RemoveAtCursor() 
{
    if (!HoverTarget) return;
    World.Remove(*HoverTarget);
    HoverTarget = nullptr;
    Log.Log("Object removed");
}

void LevelEditor::DrawGizmos() const 
{
    if (!Active) return;

    if (HoverTarget) 
    {
        const BoundingBox b = HoverTarget->Bounds();
        const Vector3 size{ b.max.x - b.min.x + 0.06f, b.max.y - b.min.y + 0.06f, b.max.z - b.min.z + 0.06f };
        DrawCubeWiresV(Vector3{ (b.min.x + b.max.x) * 0.5f, (b.min.y + b.max.y) * 0.5f, (b.min.z + b.max.z) * 0.5f }, size, RED);
    }

    if (Hover.valid) 
    {
        if (CurrentTool == Tool::CameraSpawn) 
        {
            const Vector3 position{ Hover.center.x, Hover.center.y - Hover.size.y * 0.5f + Config::kCameraSpawnHeight, Hover.center.z };
            DrawSphereWires(position, 0.6f, 8, 8, LIME);
            DrawLine3D(position, Vector3{ position.x, 0.0f, position.z }, LIME);
        }
        else 
        {
            DrawCubeWiresV(Hover.center, Hover.size, LIME);
        }
    }
}

void LevelEditor::DrawHud() const 
{
    if (!Active) return;

    const int width = 620;
    const int height = 92;
    const int x = 10;
    const int y = GetScreenHeight() - height - 10;
    DrawRectangle(x, y, width, height, Fade(BLACK, 0.7f));
    DrawText("LEVEL EDITOR  (F1 - exit, simulation paused)", x + 8, y + 6, 16, ORANGE);

    int toolX = x + 8;
    for (int tool = 1; tool <= 4; ++tool) 
    {
        const bool current = static_cast<int>(CurrentTool) == tool;
        const char* label = TextFormat("[%d] %s", tool, ToolName(tool));
        const int w = MeasureText(label, 15);
        if (current) DrawRectangle(toolX - 3, y + 28, w + 6, 20, Fade(LIME, 0.35f));
        DrawText(label, toolX, y + 30, 15, current ? WHITE : LIGHTGRAY);
        toolX += w + 16;
    }

    DrawText("LMB - add     RMB - remove cube / wall     Tab - turn wall 90 deg", x + 8, y + 54, 14, RAYWHITE);
    DrawText("F2 - save level.map     F3 - load level.map     WASD - move camera", x + 8, y + 72, 14, GRAY);
}
