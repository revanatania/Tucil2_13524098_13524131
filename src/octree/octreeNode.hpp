#pragma once
#include "../geometry/AABB.hpp"
#include <array>
#include <memory>

enum class NodeState {
    Empty,
    Internal,
    VoxelLeaf,
};
struct OctreeNode {
    AABB box;
    int depth;
    NodeState state;

    std::array<std::unique_ptr<OctreeNode>, 8> children;

    OctreeNode(const AABB& box, int depth)
        : box(box), depth(depth), state(NodeState::Empty) {}

};

