#include "objWriter.hpp"
#include <fstream>
#include <iostream>
#include <iomanip>

bool Output::writeMesh(const std::string& filePath, const Mesh& mesh, std::string& errorMessage) {
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

void Output::printStats(const OutputStats& stats) {
    std::cout << "Banyaknya voxel yang terbentuk: " << stats.voxelCount << "\n";
    std::cout << "Banyaknya vertex yang terbentuk: " << stats.vertexCount << "\n";
    std::cout << "Banyaknya faces yang terbentuk: " << stats.faceCount << "\n";

    std::cout << "\nStatistik node octree yang terbentuk:\n";
    for (int depth = 1; depth < (int)stats.nodesFormedPerDepth.size(); ++depth) {
        std::cout << depth << " : " << stats.nodesFormedPerDepth[depth] << "\n";
    }

    std::cout << "\nStatistik node yang tidak perlu ditelusuri:\n";
    for (int depth = 1; depth < (int)stats.nodesSkippedPerDepth.size(); ++depth) {
        std::cout << depth << " : " << stats.nodesSkippedPerDepth[depth] << "\n";
    }

    std::cout << "\nKedalaman octree: " << stats.octreeDepth << "\n";
    std::cout << "Lama waktu program berjalan: "
              << std::fixed << std::setprecision(3)
              << stats.runtimeMs << " ms\n";
    std::cout << "Path dimana file .obj disimpan: " << stats.outputPath << "\n";
}