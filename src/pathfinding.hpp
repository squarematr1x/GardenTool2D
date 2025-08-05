#pragma once

#include "math/vec2.hpp"

class EntityManager;

namespace path
{

struct SearchNode {
    Vec2 pos;
    Vec2 parent { 0, 0 };
    bool root{ false };
    float f; // cost, f(n) = g(n) + w*h(n)
    float g; // g(n)

    SearchNode(const Vec2& pos_in, float f_in, float g_in)
        : pos(pos_in), root(true), f(f_in), g(g_in)
    {
    }

    SearchNode(const Vec2& pos_in, const Vec2& parent_in, float f_in, float g_in)
        : pos(pos_in), parent(parent_in), f(f_in), g(g_in)
    {
    }

    bool operator> (const SearchNode& node) const {
        return f > node.f;
    }

    bool operator< (const SearchNode& node) const {
        return f < node.f;
    }
};

struct Vec2Hasher {
    std::size_t operator()(const Vec2& v) const {
        std::size_t hx = std::hash<float>{}(v.x);
        std::size_t hy = std::hash<float>{}(v.y);
        return hx ^ (hy << 1);
    }
};

std::vector<Vec2> getPath(const Vec2& start, const Vec2& goal, EntityManager& entity_manager);
std::vector<SearchNode> getNeighbours(const SearchNode& node, const Vec2& goal, EntityManager& entity_manager, const Vec2& cell_size=Vec2(64, 64));

bool contains(const std::vector<Vec2> v, const Vec2& p);
bool validGoal(const Vec2& goal, EntityManager& entity_manager);

} // namespace path
