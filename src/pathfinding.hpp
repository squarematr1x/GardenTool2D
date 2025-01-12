#pragma once

#include "math/vec2.hpp"

class EntityManager;

namespace path
{

enum class Hearistic : unsigned char {
    MANHATTAN,
    OCTAGONAL,
    EUCLIDIC
};

struct SearchNode {
    Vec2 pos;
    float f; // cost, f(n) = g(n) + w*h(n)
    float g; // g(n)

    SearchNode(const Vec2& pos_in, float f_in, float g_in)
        : pos(pos_in), f(f_in), g(g_in)
    {
    }

    bool operator> (const SearchNode& node) const {
        return f > node.f;
    }

    bool operator< (const SearchNode& node) const {
        return f < node.f;
    }

};

struct CompareNode {
    bool operator()(const SearchNode& a, const SearchNode& b) {
        return a.f > b.f;
    }
};

std::vector<Vec2> getPath(const Vec2& start, const Vec2& goal, EntityManager& entity_manager);
std::vector<SearchNode> getNeighbours(const SearchNode& node, const Vec2& goal, EntityManager& entity_manager, const Vec2& cell_size=Vec2(64, 64));

bool contains(const std::vector<Vec2> v, const Vec2& p);

} // namespace path
