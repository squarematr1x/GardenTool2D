#pragma once

#include <vector>

#include "../math/vec2.hpp"

class GameEngine;

class EntityManager; 


struct Boundaries {
    Vec2 min;
    Vec2 max;

    Boundaries(const Vec2& min_in, const Vec2& max_in)
        : min(min_in), max(max_in)
    {
    }
};

class EdgePool {
    enum Direction: int {
        WEST,
        EAST,
        NORTH,
        SOUTH
    };

    struct Edge {
        Vec2 start;
        Vec2 end;
    };

    struct Cell {
        std::array<int, 4> edge_id{ 0, 0, 0, 0 };
        std::array<bool, 4> edge_exists{ false, false, false, false };
        bool exists{ false };
    };

    std::vector<Edge> m_edges;
    std::vector<Cell> m_world;
    GameEngine* m_engine{ nullptr };

public:
    const Boundaries getWorldBoundaries(EntityManager& entity_manager, GameEngine* engine);
    void constructPool(EntityManager& entity_manager, GameEngine* engine);
    void tileMapToPolyMap(const Vec2& start, const Vec2& size, float block_w, int pitch);
};
