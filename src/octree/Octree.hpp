#pragma once
#include "../geometry/Mesh.hpp"
#include "../geometry/AABB.hpp"
#include "octreeNode.hpp"
#include <memory>
#include <vector>
#include <mutex>

struct OctreeStats {
    int voxelCount = 0;
    int maxDepth = 0;

    std::vector<int> nodesFormedPerDepth;
    std::vector<int> nodesSkippedPerDepth;

    void reset(int depthLimit){
        voxelCount= 0;
        maxDepth = depthLimit;
        nodesFormedPerDepth.assign(depthLimit+1, 0);
        nodesSkippedPerDepth.assign(depthLimit+1, 0);
    }
};

class Octree {
public:
    Octree();
    bool build(const Mesh& mesh, int maxDepth, int parallelLevel = 2);

    const OctreeNode* getRoot() const;
    const OctreeStats& getStats() const;
    void collectVoxels(std::vector<AABB>& outBoxes) const;

private:
    std::unique_ptr<OctreeNode> root;
    OctreeStats stats;
    int parallelLevel;
    mutable std::mutex statsMutex;

    bool buildRecursive(OctreeNode* node, const std::vector<const Triangle*>& triangles, int maxDepth);
    void collectVoxelsRecursive(const OctreeNode* node, std::vector<AABB>& outBoxes) const;
    void incrementNodeCount(int depth);
    void incrementVoxelCount();
    void incrementSkippedCount(int depth);
};

