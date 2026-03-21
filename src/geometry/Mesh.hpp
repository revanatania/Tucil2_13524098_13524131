#pragma once
#include <vector>
#include <string>
#include "Vector3D.hpp"
#include "Triangle.hpp"

struct Mesh {
    std::vector<Vector3D> vertices;
    std::vector<Triangle> triangles;
};

struct ParseResult {
    bool success = false;
    Mesh mesh;
    std::string errorMessage;
};