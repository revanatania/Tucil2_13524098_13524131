#include "Intersection.hpp"
#include <cmath>
#include <algorithm>
namespace {

inline void projectTriangle(const Vector3D& axis,
                             const Vector3D& v0,
                             const Vector3D& v1,
                             const Vector3D& v2,
                             double& outMin, double& outMax)
{
    double p0 = axis.dot(v0);
    double p1 = axis.dot(v1);
    double p2 = axis.dot(v2);
    outMin = std::min({p0, p1, p2});
    outMax = std::max({p0, p1, p2});
}

inline double aabbRadius(const Vector3D& axis, double hx, double hy, double hz)
{
    return hx * std::fabs(axis.x) +
           hy * std::fabs(axis.y) +
           hz * std::fabs(axis.z);
}

inline bool overlaps(double triMin, double triMax, double boxCenter, double boxR)
{
    return (triMin <= boxCenter + boxR) && (triMax >= boxCenter - boxR);
}

} 

namespace Intersection {

bool aabbIntersectsAABB(const AABB& a, const AABB& b) {
    if (a.maxCorner.x < b.minCorner.x || a.minCorner.x > b.maxCorner.x) return false;
    if (a.maxCorner.y < b.minCorner.y || a.minCorner.y > b.maxCorner.y) return false;
    if (a.maxCorner.z < b.minCorner.z || a.minCorner.z > b.maxCorner.z) return false;
    return true;
}

bool triangleIntersectsAABB(const Triangle& tri, const AABB& box)
{
    Vector3D c = box.center;
    double   h = box.halfSideLength;   

    Vector3D v0 = tri.v0 - c;
    Vector3D v1 = tri.v1 - c;
    Vector3D v2 = tri.v2 - c;

    {
        double mn, mx;

        mn = std::min({v0.x, v1.x, v2.x});
        mx = std::max({v0.x, v1.x, v2.x});
        if (mn > h || mx < -h) return false;

        mn = std::min({v0.y, v1.y, v2.y});
        mx = std::max({v0.y, v1.y, v2.y});
        if (mn > h || mx < -h) return false;
        mn = std::min({v0.z, v1.z, v2.z});
        mx = std::max({v0.z, v1.z, v2.z});
        if (mn > h || mx < -h) return false;
    }

    {
        Vector3D e0 = v1 - v0;
        Vector3D e1 = v2 - v0;
        Vector3D normal = e0.cross(e1);

        double d  = normal.dot(v0);       
        double r  = aabbRadius(normal, h, h, h);
        double absD = std::fabs(d);
        if (absD > r) return false;
    }

    Vector3D triEdges[3] = { v1 - v0, v2 - v1, v0 - v2 };

    for (int i = 0; i < 3; ++i) {
        const Vector3D& e = triEdges[i];
        {
            Vector3D axis(0.0, -e.z, e.y);
            double triMin, triMax;
            projectTriangle(axis, v0, v1, v2, triMin, triMax);
            double r = aabbRadius(axis, h, h, h);
            if (!overlaps(triMin, triMax, 0.0, r)) return false;
        }

        {
            Vector3D axis(e.z, 0.0, -e.x);
            double triMin, triMax;
            projectTriangle(axis, v0, v1, v2, triMin, triMax);
            double r = aabbRadius(axis, h, h, h);
            if (!overlaps(triMin, triMax, 0.0, r)) return false;
        }

        {
            Vector3D axis(-e.y, e.x, 0.0);
            double triMin, triMax;
            projectTriangle(axis, v0, v1, v2, triMin, triMax);
            double r = aabbRadius(axis, h, h, h);
            if (!overlaps(triMin, triMax, 0.0, r)) return false;
        }
    }
    return true;
}

} 
