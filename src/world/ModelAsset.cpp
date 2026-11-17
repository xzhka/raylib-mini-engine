#include "world/ModelAsset.h"

#include "core/Config.h"

#include <raymath.h>

#include <algorithm>
#include <cmath>

std::string ModelAsset::FindFile(const std::string& fileName) 
{
    const std::string folders[] = { Config::kAssetsDir, "assets", "../assets", "../../assets" };
    for (const std::string& folder : folders) 
    {
        const std::string path = folder + "/" + fileName;
        if (FileExists(path.c_str())) return path;
    }
    return {};
}

ModelAsset::ModelAsset(const std::string& fileName, float targetHeight) : Height(targetHeight) 
{
    const std::string path = FindFile(fileName);
    if (!path.empty()) 
    {
        Mesh = LoadModel(path.c_str());
        Loaded = Mesh.meshCount > 0;
    }

    if (!Loaded) 
    {
        TraceLog(LOG_WARNING, "MODEL: '%s' not found, enemies fall back to boxes", fileName.c_str());
        const float h = targetHeight * 0.5f;
        Local = BoundingBox{ Vector3{ -h, 0.0f, -h }, Vector3{ h, targetHeight, h } };
        Radius = h * 1.4143f;
        return;
    }

    for (int i = 0; i < Mesh.meshCount; ++i) Triangles += Mesh.meshes[i].triangleCount;

    // The texture is far bigger than the model is ever on screen, so mipmaps stop it shimmering.
    for (int i = 0; i < Mesh.materialCount; ++i) 
    {
        Texture2D& texture = Mesh.materials[i].maps[MATERIAL_MAP_ALBEDO].texture;
        // Skip default texture of unused materials
        if (texture.id == 0 || texture.width <= 1) continue;
        GenTextureMipmaps(&texture);
        SetTextureFilter(texture, TEXTURE_FILTER_TRILINEAR);
    }

    const BoundingBox raw = GetModelBoundingBox(Mesh);
    const float rawHeight = raw.max.y - raw.min.y;
    Scale = rawHeight > 1e-6f ? targetHeight / rawHeight : 1.0f;

    // Bake the offset into the model transform so that Draw() can use the position directly.
    const Vector3 centreXZ{ (raw.min.x + raw.max.x) * 0.5f, raw.min.y, (raw.min.z + raw.max.z) * 0.5f };
    Mesh.transform = MatrixTranslate(-centreXZ.x, -centreXZ.y, -centreXZ.z);

    Local = BoundingBox{ Vector3{ (raw.min.x - centreXZ.x) * Scale, 0.0f, (raw.min.z - centreXZ.z) * Scale },
                         Vector3{ (raw.max.x - centreXZ.x) * Scale, rawHeight * Scale, (raw.max.z - centreXZ.z) * Scale } };
    const float rx = std::max(std::fabs(Local.min.x), std::fabs(Local.max.x));
    const float rz = std::max(std::fabs(Local.min.z), std::fabs(Local.max.z));
    Radius = std::hypot(rx, rz);

    TraceLog(LOG_INFO, "MODEL: %s - %d triangles, height %.2f", path.c_str(), Triangles, targetHeight);
}

ModelAsset::~ModelAsset() 
{
    if (Loaded) UnloadModel(Mesh);
}

void ModelAsset::Draw(Vector3 feetPosition, float yawDeg, Color tint) const 
{
    if (!Loaded) 
    {
        const Vector3 size{ Local.max.x - Local.min.x, Height, Local.max.z - Local.min.z };
        const Vector3 centre{ feetPosition.x, feetPosition.y + Height * 0.5f, feetPosition.z };
        DrawCubeV(centre, size, RED);
        DrawCubeWiresV(centre, size, MAROON);
        return;
    }
    DrawModelEx(Mesh, feetPosition, Vector3{ 0.0f, 1.0f, 0.0f }, yawDeg, Vector3{ Scale, Scale, Scale }, tint);
}
