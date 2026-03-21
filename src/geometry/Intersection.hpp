#pragma once
#include "AABB.hpp"
#include "Triangle.hpp"

namespace Intersection {
    bool aabbIntersectsAABB(const AABB& a, const AABB& b);
    bool triangleIntersectsAABB(const Triangle& triangle, const AABB& box);
}