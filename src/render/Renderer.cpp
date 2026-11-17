#include "render/Renderer.h"

#include <raylib.h>
#include <rlgl.h>

Renderer::Renderer() 
{
    VisibleList.reserve(16384);
}

void Renderer::BeginScene(const Camera3D& camera) 
{
    Current = CullingStats{};
    VisibleList.clear();

    if (!Frozen) 
    {
        // Use the same lens parameters as Raylib BeginMode3D
        const float aspect = static_cast<float>(GetRenderWidth()) / static_cast<float>(GetRenderHeight());
        ActiveFrustum = Frustum::FromCamera(camera, aspect, static_cast<float>(rlGetCullDistanceNear()), static_cast<float>(rlGetCullDistanceFar()));
    }
    BeginMode3D(camera);
}

void Renderer::Submit(const GameObject& object) 
{
    const std::size_t kind = static_cast<std::size_t>(object.Kind());
    ++Current.Total;
    ++Current.TotalByKind[kind];

    if (CullingOn && !ActiveFrustum.Intersects(object.Bounds())) 
    {
        ++Current.Culled;
        return;
    }

    ++Current.Visible;
    ++Current.VisibleByKind[kind];
    VisibleList.push_back(&object);
}

void Renderer::DrawVisible() const 
{
    for (const GameObject* object : VisibleList) 
    {
        object->Draw();
    }
}

void Renderer::DrawFrustumDebug() const 
{
    if (!Frozen) return;

    const std::array<Vector3, 8>& c = ActiveFrustum.Corners();
    for (int i = 0; i < 4; ++i) 
    {
        const int next = (i + 1) % 4;
        DrawLine3D(c[i], c[next], YELLOW); // near rectangle
        DrawLine3D(c[4 + i], c[4 + next], YELLOW); // far rectangle
        DrawLine3D(c[i], c[4 + i], YELLOW); // side edges
    }
}

void Renderer::EndScene() 
{
    EndMode3D();
    LastStats = Current;
}

void Renderer::ToggleFrustumFreeze() 
{
    // The frustum built in the last BeginScene() stays in ActiveFrustum
    Frozen = !Frozen; 
}
