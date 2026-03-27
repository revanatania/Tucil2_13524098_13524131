#pragma once
#include <vector>
#include <string>
#include "Vector3D.hpp"
#include "Triangle.hpp"
#include "AABB.hpp"

struct Mesh {
    std::vector<Vector3D> vertices;
    std::vector<Triangle> triangles;

    Vector3D computeMinPoint() const;
    Vector3D computeMaxPoint() const;
    AABB computeBoundingCube() const;

    static Mesh createCubeMesh(const AABB& box);
    void append(const Mesh& other);
};

struct ParseResult {
    bool success = false;
    Mesh mesh;
    std::string errorMessage;
};