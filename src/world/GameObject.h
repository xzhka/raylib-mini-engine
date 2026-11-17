#pragma once

#include <raylib.h>

// Every kind of thing that lives in the game world.
enum class ObjectKind 
{
    Floor, // one tile of the floor (not editable, not saved)
    StaticCube, // editor: 1x1x1
    StaticWall, // editor: 10x10x1
    PlayerSpawn, // the player (enemy target) - exactly one exists
    CameraSpawn, // camera point marker (editor only) - exactly one exists
    Enemy, // chases the player
    Spawner, // invisible logic object that spawns the enemies
    Count
};

const char* ObjectKindName(ObjectKind kind);

// Base class of everything GameWorld manages. GameWorld and Renderer only talk to this
// interface, so adding a new type of object never changes the engine loop.
//
//  Update()  - simulation. Called ONLY for objects created with dynamic = true, i.e. the
//              ones that really change state while the game runs (enemies, the spawner).
//              Static objects are never touched by the update loop.
//  Draw()    - visualisation, called by the Renderer only for objects that survived culling.
//  Bounds()  - world-space axis aligned box: used by frustum culling and editor picking.
class GameObject 
{
    public:
    GameObject(ObjectKind kind, bool dynamic, bool renderable = true, bool editorOnly = false)
        : KindValue(kind), Dynamic(dynamic), Renderable(renderable), EditorOnlyObject(editorOnly) {}
    virtual ~GameObject() = default;

    GameObject(const GameObject&) = delete;
    GameObject& operator=(const GameObject&) = delete;

    virtual void Update(float /*dt*/) {}
    virtual void Draw() const {}
    virtual BoundingBox Bounds() const = 0;

    ObjectKind Kind() const { return KindValue; }
    bool IsDynamic() const { return Dynamic; } // must be Updated() every frame
    bool IsRenderable() const { return Renderable; } // false for pure logic objects
    bool IsEditorOnly() const { return EditorOnlyObject; } // drawn only while the editor is open

    void Destroy() { Destroyed = true; }
    bool IsDestroyed() const { return Destroyed; }

    private:
    ObjectKind KindValue;
    bool Dynamic;
    bool Renderable;
    bool EditorOnlyObject;
    bool Destroyed = false;
};
