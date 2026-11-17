#include "render/Frustum.h"

#include <raymath.h>

#include <algorithm>
#include <cmath>

namespace 
{
    Frustum::Plane MakePlane(Vector3 normal, Vector3 pointOnPlane) 
    {
        normal = Vector3Normalize(normal);
        return { normal, -Vector3DotProduct(normal, pointOnPlane) };
    }
}

Frustum Frustum::FromCamera(const Camera3D& camera, float aspect, float nearPlane, float farPlane) 
{
    const Vector3 position = camera.position;
    const Vector3 forward = Vector3Normalize(Vector3Subtract(camera.target, camera.position));
    const Vector3 right = Vector3Normalize(Vector3CrossProduct(forward, camera.up));
    const Vector3 up = Vector3CrossProduct(right, forward);

    // In Raylib fovy stand out as full vertical angle.
    const float tanY = std::tan(camera.fovy * DEG2RAD * 0.5f);
    const float tanX = tanY * aspect;

    Frustum f;
    f.Planes[0] = MakePlane(forward, Vector3Add(position, Vector3Scale(forward, nearPlane))); // near
    f.Planes[1] = MakePlane(Vector3Negate(forward), Vector3Add(position, Vector3Scale(forward, farPlane))); // far
    f.Planes[2] = MakePlane(Vector3Add(right, Vector3Scale(forward, tanX)), position); // left
    f.Planes[3] = MakePlane(Vector3Add(Vector3Negate(right), Vector3Scale(forward, tanX)), position); // right
    f.Planes[4] = MakePlane(Vector3Add(up, Vector3Scale(forward, tanY)), position); // bottom
    f.Planes[5] = MakePlane(Vector3Add(Vector3Negate(up), Vector3Scale(forward, tanY)), position); // top

    const float distances[2] = { nearPlane, std::min(farPlane, kDebugFarDistance) };
    for (int i = 0; i < 2; ++i) 
    {
        const float z = distances[i];
        const Vector3 centre = Vector3Add(position, Vector3Scale(forward, z));
        const Vector3 dx = Vector3Scale(right, tanX * z);
        const Vector3 dy = Vector3Scale(up, tanY * z);
        f.CornerPoints[i * 4 + 0] = Vector3Subtract(Vector3Subtract(centre, dx), dy);
        f.CornerPoints[i * 4 + 1] = Vector3Subtract(Vector3Add(centre, dx), dy);
        f.CornerPoints[i * 4 + 2] = Vector3Add(Vector3Add(centre, dx), dy);
        f.CornerPoints[i * 4 + 3] = Vector3Add(Vector3Subtract(centre, dx), dy);
    }
    return f;
}

bool Frustum::Intersects(const BoundingBox& box) const 
{
    const Vector3 centre{ (box.min.x + box.max.x) * 0.5f, (box.min.y + box.max.y) * 0.5f, (box.min.z + box.max.z) * 0.5f };
    const Vector3 extent{ (box.max.x - box.min.x) * 0.5f, (box.max.y - box.min.y) * 0.5f, (box.max.z - box.min.z) * 0.5f };

    for (const Plane& plane : Planes) 
    {
        // Distance of the box centre to the plane, and how far the box reaches towards the plane
        const float s = Vector3DotProduct(plane.Normal, centre) + plane.D;
        const float r = extent.x * std::fabs(plane.Normal.x) + extent.y * std::fabs(plane.Normal.y) + extent.z * std::fabs(plane.Normal.z);
        if (s < -r) return false; 
    }
    return true;
}
