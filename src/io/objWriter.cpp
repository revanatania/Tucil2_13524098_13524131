#include "objWriter.hpp"
#include <fstream>
#include <iostream>
#include <iomanip>

using namespace std;

bool Output::writeMesh(const string& filePath, const Mesh& mesh, string& errorMessage) {
    ofstream out(filePath);
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
    cout << "Banyaknya voxel yang terbentuk: " << stats.voxelCount << "\n";
    cout << "Banyaknya vertex yang terbentuk: " << stats.vertexCount << "\n";
    cout << "Banyaknya faces yang terbentuk: " << stats.faceCount << "\n";

    cout << "\nStatistik node octree yang terbentuk:\n";
    for (int depth = 1; depth < (int)stats.nodesFormedPerDepth.size(); ++depth) {
        cout << depth << " : " << stats.nodesFormedPerDepth[depth] << "\n";
    }

    cout << "\nStatistik node yang tidak perlu ditelusuri:\n";
    for (int depth = 1; depth < (int)stats.nodesSkippedPerDepth.size(); ++depth) {
        cout << depth << " : " << stats.nodesSkippedPerDepth[depth] << "\n";
    }

    cout << "\nKedalaman octree: " << stats.octreeDepth << "\n";
    cout << "Lama waktu program berjalan: "
              << fixed << setprecision(3)
              << stats.runtimeMs << " ms\n";
    cout << "Path dimana file .obj disimpan: " << stats.outputPath << "\n";
}