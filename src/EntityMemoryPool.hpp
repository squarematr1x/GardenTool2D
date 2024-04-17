#pragma once

#include "EntityManager.hpp"

constexpr size_t MAX_ENTITIES{ 1000 }; // TODO: Use something else than global variable?

class EntityMemoryPool
{
    size_t m_entity_count{ 0 };
    EntityComponentVectorTuple m_pool;
    std::vector<Tag> m_tags;
    std::vector<bool> m_active;

    EntityMemoryPool(size_t max_entities);
    
public:
    static EntityMemoryPool & Instance() {
        static EntityMemoryPool pool(MAX_ENTITIES);
        return pool;
    }

    template <typename T>
    T& getComponent(size_t entity_id) { return std::get<std::vector<T>>(m_pool)[entity_id]; }

    const Tag getTag(size_t entity_id) const { return m_tags[entity_id]; }

    const size_t getNextEntityIndex() const {
        // Loop trough and find the first index that m_active = false
        return 0;
    } // TODO: Implement this

    // Entity addEntity(const Tag tag) {
    //     size_t index = getNextEntityIndex();
    //     // set all vectors[index] = 0
    //     // set tag[index], active[index] = defaults
    //     return Entity(index);
    // }

    // NOTE: destorying entity is just to mark it m_active = false
    //       the component data can be safely overwritten
};
