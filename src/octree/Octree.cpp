#include "Octree.hpp"
#include "../geometry/Intersection.hpp"
#include <future>
#include <stdexcept>
#include <utility>

Octree::Octree() : root(nullptr), parallelLevel(2) {}

bool Octree::build(const Mesh& mesh, int maxDepth, int parallelLevel) {
    if (mesh.triangles.empty()) {
        throw std::runtime_error("Cannot build octree from an empty mesh.");
    }
    if (maxDepth < 1){
        throw std::runtime_error("Max depth must be at least 1.");
    }
    if (parallelLevel < 1) {
        parallelLevel = 1;
    }
    this->parallelLevel = parallelLevel;
    stats.reset(maxDepth);

    AABB rootBox = mesh.computeBoundingCube();
    root = std::make_unique<OctreeNode>(rootBox, 1);
    std::vector<const Triangle*> candidates;
    candidates.reserve(mesh.triangles.size());

    for (const Triangle& t : mesh.triangles) {
        candidates.push_back(&t);
    }

    return buildRecursive(root.get(), candidates, maxDepth);

}

const OctreeNode* Octree::getRoot() const {
    return root.get();
}

const OctreeStats& Octree::getStats() const {
    return stats;
}

void Octree::incrementNodeCount(int depth) {
    std::lock_guard<std::mutex> lock(statsMutex);
    stats.nodesFormedPerDepth[depth]++;
}

void Octree::incrementSkippedCount(int depth) {
    std::lock_guard<std::mutex> lock(statsMutex);
    stats.nodesSkippedPerDepth[depth]++;
}

void Octree::incrementVoxelCount() {
    std::lock_guard<std::mutex> lock(statsMutex);
    stats.voxelCount++;
}

bool Octree::buildRecursive(OctreeNode* node, const std::vector<const Triangle*>& triangles, int maxDepth) {
    incrementNodeCount(node->depth);

    // filter candidate triangles yg intersect sm node box
    std::vector<const Triangle*> intersectingTriangles;
    intersectingTriangles.reserve(triangles.size());

    for (const Triangle* tri : triangles) {
        if (Intersection::triangleIntersectsAABB(*tri, node->box)) {
            intersectingTriangles.push_back(tri);
        }
    }

    // klo gaada triangle yg intersect then node kosong, stop
    if (intersectingTriangles.empty()) {
        node->state = NodeState::Empty;
        incrementSkippedCount(node->depth);
        return false;
    }

    // depth max, bikin voxel leaf
    if (node->depth >= maxDepth) {
        node->state = NodeState::VoxelLeaf;
        incrementVoxelCount();
        return true;
    }

    // blm depthmax, subdivide node jd 8 anak
    node->state = NodeState::Internal;
    AABB children[8];
    node->box.subdivide(children);
    bool hasVoxelLeaf = false;

    if (node->depth <= parallelLevel) {
        std::future<bool> futures[8];

        for (int i = 0; i < 8; ++i) {
            node->children[i] = std::make_unique<OctreeNode>(children[i], node->depth + 1);
            OctreeNode* childNode = node->children[i].get();

             // buat list triangle yg intersect sama child ini
            futures[i] = std::async(std::launch::async,
                [this, childNode, intersectingTriangles, maxDepth]() -> bool {
                    return buildRecursive(childNode, intersectingTriangles, maxDepth);
                }
            );
        }

        for (int i = 0; i < 8; ++i) {
            bool childHasVoxel = futures[i].get();
            if (!childHasVoxel) {
                node->children[i].reset();
            } else {
                hasVoxelLeaf = true;
            }   
        }
    }
    else {
        for (int i = 0; i < 8; ++i) {
            node->children[i] = std::make_unique<OctreeNode>(children[i], node->depth + 1);
    
            bool childHasVoxel = buildRecursive(node->children[i].get(), intersectingTriangles, maxDepth);

            if (!childHasVoxel) {
                node->children[i].reset();
            } else {
                hasVoxelLeaf = true;
            }
        }
    }
    return hasVoxelLeaf;
}

void Octree::collectVoxels(std::vector<AABB>& outBoxes) const {
    outBoxes.clear();
    collectVoxelsRecursive(root.get(), outBoxes);
}

void Octree::collectVoxelsRecursive(const OctreeNode* node, std::vector<AABB>& outBoxes) const {
    if (node == nullptr) return;

    if (node->state == NodeState::VoxelLeaf) {
        outBoxes.push_back(node->box);
        return;
    }

    if (node->state == NodeState::Internal) {
        for (const auto& child : node->children) {
            collectVoxelsRecursive(child.get(), outBoxes);
        }
    }
}