#pragma once
#include "../geometry/Mesh.hpp"
#include "../io/objWriter.hpp"

struct VoxelizationResult {
    Mesh outputMesh;
    OutputStats stats;
};

class Voxelizer {
public:
    static VoxelizationResult voxelize(
        const Mesh& inputMesh,
        int maxDepth,
        int parallelDepthLimit = 2
    );
};