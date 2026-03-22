#pragma once
#include <string>
#include "../geometry/Mesh.hpp"
#include <vector>

struct OutputStats {
    int voxelCount = 0;
    int vertexCount = 0;
    int faceCount = 0;
    int octreeDepth = 0;

    std::vector<int> nodesFormedPerDepth;
    std::vector<int> nodesSkippedPerDepth;

    double runtimeMs = 0.0;
    std::string outputPath;
};

class Output {
public:
    static bool writeMesh(const std::string& filePath, const Mesh& mesh, std::string& errorMessage);
    static void printStats(const OutputStats& stats);
};