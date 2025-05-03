#include "edge-pool.hpp"

#include "../ecs/entity-manager.hpp"

void EdgePool::constructPool(EntityManager& entity_manager) {
    /*
    for tile : tiles
        if !tile.bbox
        continue
    
        combine consecutive tiles to larger rectangle shapes
    
    
    */

    for (const auto& e : entity_manager.getEntities(Tag::TILE)) {
        if (!e->hasComponent<CBBox>()) {
            continue;
        }
        // ...
    }

}
