#pragma once

#include <vector>
#include <unordered_set>

#include "../math/vec2.hpp"

class GameEngine;
class EntityManager; 

struct Boundary {
    Vec2 min;
    Vec2 max;

    Boundary(const Vec2& min_in, const Vec2& max_in)
        : min(min_in), max(max_in)
    {
    }
};

struct Edge {
    Vec2 start;
    Vec2 end;

    Edge(const Vec2& start_in, const Vec2& end_in)
        : start(start_in), end(end_in)
    {
    }
};

class EdgePool {
    enum Direction: int {
        NORTH,
        SOUTH,
        EAST,
        WEST
    };

    struct Cell {
        std::array<size_t, 4> edge_id{ 0, 0, 0, 0 };
        std::array<bool, 4> edge_exists{ false, false, false, false };
        bool exists{ false };
    };

    std::vector<Edge> m_edges;

    const Boundary getWorldBoundary(
        EntityManager& entity_manager, GameEngine* engine, std::unordered_set<Vec2, Vec2Hasher>& vertices,
        bool padding = true, const Vec2& cell_size = Vec2(64.0f, 64.0f)
    );

    bool validIndex(size_t index, std::vector<Cell> cells);

public:
    void constructPool(EntityManager& entity_manager, GameEngine* engine, const Vec2& cell_size);

    const std::vector<Edge>& getEdges() const { return m_edges; }
};
