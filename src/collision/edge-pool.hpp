#pragma once

#include <vector>

#include "../math/vec2.hpp"

class EntityManager;

class EdgePool {
    std::vector<Vec2> m_pool;

public:
    void constructPool(EntityManager& entity_manager);
};
