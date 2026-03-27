#include "Mesh.hpp"
#include <stdexcept>

Vector3D Mesh::computeMinPoint() const {
    if (vertices.empty()) {
        throw std::runtime_error("Cannot compute min point of empty mesh.");
    }

    Vector3D minPoint = vertices[0];
    for (size_t i = 1; i < vertices.size(); ++i) {
        minPoint = Vector3D::min(minPoint, vertices[i]);
    }
    return minPoint;
}

Vector3D Mesh::computeMaxPoint() const {
    if (vertices.empty()) {
        throw std::runtime_error("Cannot compute max point of empty mesh.");
    }

    Vector3D maxPoint = vertices[0];
    for (size_t i = 1; i < vertices.size(); ++i) {
        maxPoint = Vector3D::max(maxPoint, vertices[i]);
    }
    return maxPoint;
}

AABB Mesh::computeBoundingCube() const {
    return AABB::fromBounds(computeMinPoint(), computeMaxPoint());
}

void Mesh::append(const Mesh& other) {
    for (const Vector3D& v : other.vertices) {
        vertices.push_back(v);
    }

    for (const Triangle& t : other.triangles) {
        triangles.push_back(t);
    }
}

Mesh Mesh::createCubeMesh(const AABB& box) {
    Mesh mesh;

    const Vector3D& mn = box.minCorner;
    const Vector3D& mx = box.maxCorner;

    Vector3D v000(mn.x, mn.y, mn.z);
    Vector3D v001(mn.x, mn.y, mx.z);
    Vector3D v010(mn.x, mx.y, mn.z);
    Vector3D v011(mn.x, mx.y, mx.z);
    Vector3D v100(mx.x, mn.y, mn.z);
    Vector3D v101(mx.x, mn.y, mx.z);
    Vector3D v110(mx.x, mx.y, mn.z);
    Vector3D v111(mx.x, mx.y, mx.z);

    mesh.vertices = {v000, v001, v010, v011, v100, v101, v110, v111};

    mesh.triangles = {
        Triangle(v000, v001, v011),
        Triangle(v000, v011, v010),

        Triangle(v100, v110, v111),
        Triangle(v100, v111, v101),

        Triangle(v000, v100, v101),
        Triangle(v000, v101, v001),

        Triangle(v010, v011, v111),
        Triangle(v010, v111, v110),

        Triangle(v000, v010, v110),
        Triangle(v000, v110, v100),

        Triangle(v001, v101, v111),
        Triangle(v001, v111, v011)
    };

    return mesh;
}