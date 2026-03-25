#pragma once

#include <vector>
#include "../geometry/Mesh.hpp"
#include "../io/objWriter.hpp"
#include "../geometry/AABB.hpp"

struct VoxelizationResult {
    Mesh outputMesh;
    OutputStats stats;
    std::vector<AABB> voxels;
};

class Voxelizer {
public:
    static VoxelizationResult voxelize(
        const Mesh& inputMesh,
        int maxDepth,
        int parallelDepthLimit = 2
    );
};