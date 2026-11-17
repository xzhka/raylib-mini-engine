#pragma once

#include <raylib.h>

#include <array>

// View frustum of a perspective camera: 6 planes with normals pointing INSIDE.
//
// The planes are built from the camera basis (position, forward, right, up) and the lens
// (tan of the half FOV, aspect, near, far). In camera space a point (x, y, z) - z along the
// view direction - is inside when
//     z >= near,  z <= far,
//     x + tanX * z >= 0 (left),  -x + tanX * z >= 0 (right),
//     y + tanY * z >= 0 (bottom), -y + tanY * z >= 0 (top)
// and each line is exactly a plane  n . p + d >= 0  through the camera position.
class Frustum 
{
    public:
    struct Plane 
    {
        Vector3 Normal; // unit length, points into the frustum
        float D; // signed distance of the plane from the origin
    };

    static Frustum FromCamera(const Camera3D& camera, float aspect, float nearPlane, float farPlane);

    // Conservative test: false ONLY when the box is completely outside one of the planes
    // (so a visible object is never culled; a few boxes near the corners may be kept).
    bool Intersects(const BoundingBox& box) const;

    // 8 corners (near plane first, far clipped to debugFar) for the debug wireframe.
    const std::array<Vector3, 8>& Corners() const { return CornerPoints; }

    static constexpr float kDebugFarDistance = 120.0f;

    private:
    std::array<Plane, 6> Planes{};
    std::array<Vector3, 8> CornerPoints{};
};
