#include "Octree.hpp"
#include "../geometry/Intersection.hpp"
#include <future>
#include <stdexcept>

Octree::Octree() : root(nullptr), parallelLevel(2) {}

bool Octree::build(const Mesh& mesh, int maxDepth, int parallelLevel) {
    if (mesh.triangles.empty())
        throw std::runtime_error("Cannot build octree from an empty mesh.");
    if (maxDepth < 1)
        throw std::runtime_error("Max depth must be at least 1.");
    if (parallelLevel < 1) parallelLevel = 1;

    this->parallelLevel = parallelLevel;
    stats.reset(maxDepth);

    AABB rootBox = mesh.computeBoundingCube();
    root = std::make_unique<OctreeNode>(rootBox, 1);

    std::vector<const Triangle*> candidates;
    candidates.reserve(mesh.triangles.size());
    for (const Triangle& t : mesh.triangles)
        candidates.push_back(&t);

    buildRecursive(root.get(), candidates, maxDepth);
    return true;
}

const OctreeNode* Octree::getRoot() const { return root.get(); }
const OctreeStats& Octree::getStats() const { return stats; }

void Octree::incrementNodeCount(int depth) {
    std::lock_guard<std::mutex> lk(statsMutex);
    stats.nodesFormedPerDepth[depth]++;
}
void Octree::incrementSkippedCount(int depth) {
    std::lock_guard<std::mutex> lk(statsMutex);
    stats.nodesSkippedPerDepth[depth]++;
}
void Octree::incrementVoxelCount() {
    std::lock_guard<std::mutex> lk(statsMutex);
    stats.voxelCount++;
}

bool Octree::buildRecursive(OctreeNode* node,
                             const std::vector<const Triangle*>& triangles,
                             int maxDepth)
{
    incrementNodeCount(node->depth);

    std::vector<const Triangle*> intersecting;
    intersecting.reserve(triangles.size());
    for (const Triangle* tri : triangles) {
        if (Intersection::triangleIntersectsAABB(*tri, node->box))
            intersecting.push_back(tri);
    }

    if (intersecting.empty()) {
        node->state = NodeState::Empty;
        incrementSkippedCount(node->depth);
        return false;
    }

    if (node->depth >= maxDepth) {
        node->state = NodeState::VoxelLeaf;
        incrementVoxelCount();
        return true;
    }

    node->state = NodeState::Internal;
    AABB childBoxes[8];
    node->box.subdivide(childBoxes);
    bool hasAnyVoxel = false;

    if (node->depth <= parallelLevel) {
        std::vector<std::vector<const Triangle*>> childTris(8);
        for (int i = 0; i < 8; ++i) {
            for (const Triangle* tri : intersecting) {
                if (Intersection::triangleIntersectsAABB(*tri, childBoxes[i]))
                    childTris[i].push_back(tri);
            }
        }

        std::future<bool> futures[8];
        for (int i = 0; i < 8; ++i) {
            node->children[i] = std::make_unique<OctreeNode>(childBoxes[i], node->depth + 1);
            OctreeNode* childPtr = node->children[i].get();
            auto localTris = std::move(childTris[i]);
            futures[i] = std::async(std::launch::async,
                [this, childPtr, tris = std::move(localTris), maxDepth]() mutable -> bool {
                    return buildRecursive(childPtr, tris, maxDepth);
                });
        }

        for (int i = 0; i < 8; ++i) {
            bool childHasVoxel = futures[i].get();
            if (!childHasVoxel)
                node->children[i].reset();
            else
                hasAnyVoxel = true;
        }
    } else {
        for (int i = 0; i < 8; ++i) {
            node->children[i] = std::make_unique<OctreeNode>(childBoxes[i], node->depth + 1);
            bool childHasVoxel = buildRecursive(node->children[i].get(), intersecting, maxDepth);
            if (!childHasVoxel)
                node->children[i].reset();
            else
                hasAnyVoxel = true;
        }
    }

    return hasAnyVoxel;
}

void Octree::collectVoxels(std::vector<AABB>& outBoxes) const {
    outBoxes.clear();
    collectVoxelsRecursive(root.get(), outBoxes);
}

void Octree::collectVoxelsRecursive(const OctreeNode* node,
                                     std::vector<AABB>& outBoxes) const {
    if (!node) return;
    if (node->state == NodeState::VoxelLeaf) {
        outBoxes.push_back(node->box);
        return;
    }
    if (node->state == NodeState::Internal) {
        for (const auto& child : node->children)
            collectVoxelsRecursive(child.get(), outBoxes);
    }
}
