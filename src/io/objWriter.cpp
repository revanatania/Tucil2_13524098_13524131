#include "ObjWriter.hpp"
#include <fstream>

bool ObjWriter::writeMesh(const std::string& filePath, const Mesh& mesh, std::string& errorMessage) {
    std::ofstream out(filePath);
    if (!out.is_open()) {
        errorMessage = "Failed to open output file: " + filePath;
        return false;
    }

    out << "# Generated voxelized OBJ\n";

    int currentIndex = 1;

    for (const Triangle& tri : mesh.triangles) {
        out << "v " << tri.v0.x << " " << tri.v0.y << " " << tri.v0.z << "\n";
        out << "v " << tri.v1.x << " " << tri.v1.y << " " << tri.v1.z << "\n";
        out << "v " << tri.v2.x << " " << tri.v2.y << " " << tri.v2.z << "\n";

        out << "f " << currentIndex << " " << currentIndex + 1 << " " << currentIndex + 2 << "\n";
        currentIndex += 3;
    }

    if (!out.good()) {
        errorMessage = "Failed while writing output file: " + filePath;
        return false;
    }

    return true;
}