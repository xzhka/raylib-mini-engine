#pragma once

#include <raylib.h>

#include <string>

// A model loaded ONCE and shared by every object that uses it (all enemies draw the same
// GPU mesh with a different transform). The model is scaled to `targetHeight` and its
// origin is the centre of the feet, so an object only has to supply position and yaw.
//
// If the file cannot be found/loaded a plain box of the same height is used instead, so
// the game still runs.
class ModelAsset 
{
    public:
    ModelAsset(const std::string& fileName, float targetHeight);
    ~ModelAsset();
    ModelAsset(const ModelAsset&) = delete;
    ModelAsset& operator=(const ModelAsset&) = delete;

    void Draw(Vector3 feetPosition, float yawDeg, Color tint) const;

    bool IsLoaded() const { return Loaded; }
    int TriangleCount() const { return Triangles; }
    BoundingBox LocalBounds() const { return Local; } // scaled, relative to the feet position, yaw 0
    float RadiusXZ() const { return Radius; } // bounds radius in the XZ plane: independent of yaw

    private:
    static std::string FindFile(const std::string& fileName);

    Model Mesh{};
    bool Loaded = false;
    int Triangles = 0;
    float Scale = 1.0f;
    float Height;
    BoundingBox Local{};
    float Radius = 0.0f;
};
