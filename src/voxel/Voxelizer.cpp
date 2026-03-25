#include "Voxelizer.hpp"
#include "../octree/Octree.hpp"
#include <chrono>
#include <vector>

VoxelizationResult Voxelizer::voxelize(
    const Mesh& inputMesh,
    int maxDepth,
    int parallelDepthLimit
) {
    auto start = std::chrono::high_resolution_clock::now();

    Octree octree;
    octree.build(inputMesh, maxDepth, parallelDepthLimit);

    std::vector<AABB> voxelBoxes;
    octree.collectVoxels(voxelBoxes);

    Mesh outputMesh;
    for (const AABB& box : voxelBoxes) {
        Mesh cubeMesh = Mesh::createCubeMesh(box);
        outputMesh.append(cubeMesh);
    }

    auto end = std::chrono::high_resolution_clock::now();
    double runtimeMs = std::chrono::duration<double, std::milli>(end - start).count();

    const OctreeStats& octreeStats = octree.getStats();

    OutputStats stats;
    stats.voxelCount = octreeStats.voxelCount;
    // Dihitung per voxel: 1 voxel = 8 vertex + 12 face
    stats.vertexCount = stats.voxelCount * 8;
    stats.faceCount = stats.voxelCount * 12;
    stats.octreeDepth = octreeStats.maxDepth;
    stats.nodesFormedPerDepth = octreeStats.nodesFormedPerDepth;
    stats.nodesSkippedPerDepth = octreeStats.nodesSkippedPerDepth;
    stats.runtimeMs = runtimeMs;

    VoxelizationResult result;
    result.outputMesh = std::move(outputMesh);
    result.stats = std::move(stats);
    result.voxels = std::move(voxelBoxes);

    return result;
}