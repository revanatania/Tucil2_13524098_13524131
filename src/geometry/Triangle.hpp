#pragma once
#include "Vector3D.hpp"

struct Triangle {
    Vector3D v0, v1, v2;
    Triangle() {}
    Triangle(const Vector3D& a, const Vector3D& b, const Vector3D& c)
        : v0(a), v1(b), v2(c) {}


    Vector3D min() const {
        return {
            std::fmin(v0.x, std::fmin(v1.x, v2.x)),
            std::fmin(v0.y, std::fmin(v1.y, v2.y)),
            std::fmin(v0.z, std::fmin(v1.z, v2.z))
        };
    }

    Vector3D max() const {
        return {
            std::fmax(v0.x, std::fmax(v1.x, v2.x)),
            std::fmax(v0.y, std::fmax(v1.y, v2.y)),
            std::fmax(v0.z, std::fmax(v1.z, v2.z))
        };
    }
};