#pragma once

#include "render/Frustum.h"
#include "world/GameObject.h"

#include <array>
#include <cstddef>
#include <vector>

struct CullingStats 
{
    static constexpr std::size_t kKinds = static_cast<std::size_t>(ObjectKind::Count);

    std::size_t Total = 0; // objects submitted this frame
    std::size_t Visible = 0; // passed the frustum test (drawn)
    std::size_t Culled = 0; // outside the frustum (not drawn)
    std::array<std::size_t, kKinds> TotalByKind{};
    std::array<std::size_t, kKinds> VisibleByKind{};
};

// Rendering subsystem. A frame has three steps:
//   BeginScene(camera) - sets the 3D camera and builds the view frustum
//   Submit(object) - frustum culling: objects outside are counted and dropped, the others go to a
//                         visible list (the world calls this for every object)
//   DrawVisible() - draws the visible list only
//   EndScene()
// Keeping culling (Submit) and drawing (DrawVisible) apart lets the profiler show what each costs.
//
// Debug tools: culling can be switched off to compare frame times, and the frustum can be
// frozen (and drawn) - move the camera away and see which objects the frozen frustum drops.
class Renderer 
{
    public:
    Renderer();

    void BeginScene(const Camera3D& camera);
    void Submit(const GameObject& object);
    void DrawVisible() const;
    void DrawFrustumDebug() const; // call inside the scene; draws the frozen frustum only
    void EndScene();

    void SetCullingEnabled(bool enabled) { CullingOn = enabled; }
    bool IsCullingEnabled() const { return CullingOn; }
    void ToggleFrustumFreeze();
    bool IsFrustumFrozen() const { return Frozen; }

    const CullingStats& Stats() const { return LastStats; }

    private:
    Frustum ActiveFrustum;
    bool CullingOn = true;
    bool Frozen = false;

    // Reused every frame, no allocation in steady state
    std::vector<const GameObject*> VisibleList;
    CullingStats Current; 
    CullingStats LastStats; 
};
