#include "Intersection.hpp"
#include <cmath>
#include <algorithm>

namespace {

const double EPS = 1e-9;

bool pointInAABB(const Vector3D& p, const AABB& box) {
    return (p.x >= box.minCorner.x - EPS && p.x <= box.maxCorner.x + EPS) &&
           (p.y >= box.minCorner.y - EPS && p.y <= box.maxCorner.y + EPS) &&
           (p.z >= box.minCorner.z - EPS && p.z <= box.maxCorner.z + EPS);
}

AABB triangleToAABB(const Triangle& t) {
    Vector3D minPoint = t.min();
    Vector3D maxPoint = t.max();
    return AABB::fromBounds(minPoint, maxPoint);
}

double clamp(double x, double lo, double hi) {
    return std::max(lo, std::min(x, hi));
}

Vector3D closestPointOnAABB(const Vector3D& p, const AABB& box) {
    return Vector3D(
        clamp(p.x, box.minCorner.x, box.maxCorner.x),
        clamp(p.y, box.minCorner.y, box.maxCorner.y),
        clamp(p.z, box.minCorner.z, box.maxCorner.z)
    );
}

double pointPlaneSignedDistance(const Vector3D& p, const Vector3D& normal, const Vector3D& planePoint) {
    return normal.dot(p - planePoint);
}

bool segmentIntersectsAABB(const Vector3D& p0, const Vector3D& p1, const AABB& box) {
    double tmin = 0.0;
    double tmax = 1.0;

    Vector3D d = p1 - p0;

    // X axis
    if (std::fabs(d.x) < EPS) {
        if (p0.x < box.minCorner.x || p0.x > box.maxCorner.x) return false;
    } else {
        double inv = 1.0 / d.x;
        double t1 = (box.minCorner.x - p0.x) * inv;
        double t2 = (box.maxCorner.x - p0.x) * inv;
        if (t1 > t2) std::swap(t1, t2);
        tmin = std::max(tmin, t1);
        tmax = std::min(tmax, t2);
        if (tmin > tmax) return false;
    }

    // Y axis
    if (std::fabs(d.y) < EPS) {
        if (p0.y < box.minCorner.y || p0.y > box.maxCorner.y) return false;
    } else {
        double inv = 1.0 / d.y;
        double t1 = (box.minCorner.y - p0.y) * inv;
        double t2 = (box.maxCorner.y - p0.y) * inv;
        if (t1 > t2) std::swap(t1, t2);
        tmin = std::max(tmin, t1);
        tmax = std::min(tmax, t2);
        if (tmin > tmax) return false;
    }

    // Z axis
    if (std::fabs(d.z) < EPS) {
        if (p0.z < box.minCorner.z || p0.z > box.maxCorner.z) return false;
    } else {
        double inv = 1.0 / d.z;
        double t1 = (box.minCorner.z - p0.z) * inv;
        double t2 = (box.maxCorner.z - p0.z) * inv;
        if (t1 > t2) std::swap(t1, t2);
        tmin = std::max(tmin, t1);
        tmax = std::min(tmax, t2);
        if (tmin > tmax) return false;
    }

    return true;
}

bool triangleDegenerate(const Triangle& t) {
    Vector3D e1 = t.v1 - t.v0;
    Vector3D e2 = t.v2 - t.v0;
    return e1.cross(e2).lengthSqr() < EPS;
}

} // anonymous namespace

namespace Intersection {

bool aabbIntersectsAABB(const AABB& a, const AABB& b) {
    if (a.maxCorner.x < b.minCorner.x || a.minCorner.x > b.maxCorner.x) return false;
    if (a.maxCorner.y < b.minCorner.y || a.minCorner.y > b.maxCorner.y) return false;
    if (a.maxCorner.z < b.minCorner.z || a.minCorner.z > b.maxCorner.z) return false;
    return true;
}

bool triangleIntersectsAABB(const Triangle& triangle, const AABB& box) {
    // 1. Fast reject using bounding boxes
    AABB triBox = triangleToAABB(triangle);
    if (!aabbIntersectsAABB(triBox, box)) {
        return false;
    }

    // 2. If any triangle vertex is inside the box, intersection exists
    if (pointInAABB(triangle.v0, box)) return true;
    if (pointInAABB(triangle.v1, box)) return true;
    if (pointInAABB(triangle.v2, box)) return true;

    // 3. If any edge intersects the box, intersection exists
    if (segmentIntersectsAABB(triangle.v0, triangle.v1, box)) return true;
    if (segmentIntersectsAABB(triangle.v1, triangle.v2, box)) return true;
    if (segmentIntersectsAABB(triangle.v2, triangle.v0, box)) return true;

    // 4. Handle triangle plane passing through box
    //    Useful when triangle fully covers a box face region but no box contains a triangle vertex.
    if (!triangleDegenerate(triangle)) {
        Vector3D normal = (triangle.v1 - triangle.v0).cross(triangle.v2 - triangle.v0);

        Vector3D corners[8] = {
            Vector3D(box.minCorner.x, box.minCorner.y, box.minCorner.z),
            Vector3D(box.minCorner.x, box.minCorner.y, box.maxCorner.z),
            Vector3D(box.minCorner.x, box.maxCorner.y, box.minCorner.z),
            Vector3D(box.minCorner.x, box.maxCorner.y, box.maxCorner.z),
            Vector3D(box.maxCorner.x, box.minCorner.y, box.minCorner.z),
            Vector3D(box.maxCorner.x, box.minCorner.y, box.maxCorner.z),
            Vector3D(box.maxCorner.x, box.maxCorner.y, box.minCorner.z),
            Vector3D(box.maxCorner.x, box.maxCorner.y, box.maxCorner.z)
        };

        bool hasPositive = false;
        bool hasNegative = false;
        for (int i = 0; i < 8; ++i) {
            double d = pointPlaneSignedDistance(corners[i], normal, triangle.v0);
            if (d > EPS) hasPositive = true;
            if (d < -EPS) hasNegative = true;
        }

        // If the box straddles the plane, and bounding boxes already overlap,
        // we conservatively treat it as intersection.
        if (hasPositive && hasNegative) {
            return true;
        }
    }

    return false;
}

}