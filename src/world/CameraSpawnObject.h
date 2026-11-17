#pragma once

#include "world/GameObject.h"

// Marker of the camera point. Exists only for the editor (IsEditorOnly), so it is
// never drawn and never counted - during the game.
class CameraSpawnObject : public GameObject 
{
    public:
    explicit CameraSpawnObject(Vector3 position)
        : GameObject(ObjectKind::CameraSpawn, /*dynamic*/ false, /*renderable*/ true, /*editorOnly*/ true), Position(position) {}

    BoundingBox Bounds() const override;
    void Draw() const override;

    Vector3 GetPosition() const { return Position; }
    void SetPosition(Vector3 position) { Position = position; }

    private:
    Vector3 Position;
};
