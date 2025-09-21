#include "entity-memory-pool.hpp"

EntityMemoryPool::EntityMemoryPool(size_t max_entities) {
    m_active.reserve(max_entities);
    m_tags.reserve(max_entities);

    for (size_t i = 0; i < max_entities; i++) {
        m_active.push_back(false);
        m_tags.push_back(Tag::DEFAULT);
    }

    reserveComponents();
}

size_t EntityMemoryPool::getNextEntityIndex() const {
    for (size_t i = 0; i < MAX_ENTITIES; i++) {
        if (!m_active[i]) {
            return i;
        }
    }
    // NOTE: handle edge case where id >= MAX_ENTITIES
    //       prevent from adding entity or reallocate more memory?
    return MAX_ENTITIES;
}

size_t EntityMemoryPool::addEntity(const Tag tag) {
    size_t index = getNextEntityIndex();

    m_active[index] = true;
    m_tags[index] = tag;
    m_entity_count++;
    clearComponents(index);

    return index;
}

void EntityMemoryPool::destroyEntity(size_t entity_id) {
    m_entity_count--;
    m_active[entity_id] = false;
    m_tags[entity_id] = Tag::DEFAULT;
    clearComponents(entity_id);
}

void EntityMemoryPool::clearComponents(size_t index) {
    clearComponent<CTransform>(index);
    clearComponent<CScore>(index);
    clearComponent<CHealth>(index);
    clearComponent<CInput>(index);
    clearComponent<CLifespan>(index);
    clearComponent<CDamage>(index);
    clearComponent<CInvincibility>(index);
    clearComponent<CBBox>(index);
    clearComponent<CAnimation>(index);
    clearComponent<CGravity>(index);
    clearComponent<CState>(index);
    clearComponent<CDraggable>(index);
    clearComponent<CFollowPlayer>(index);
    clearComponent<CPatrol>(index);
    clearComponent<CBehavior>(index);
    clearComponent<CWeapon>(index);
    clearComponent<CPath>(index);
    clearComponent<CTrigger>(index);
    clearComponent<CTriggerable>(index);
    clearComponent<CInteractable>(index);
}

void EntityMemoryPool::reserveComponents() {
    reserveComponent<CTransform>();
    reserveComponent<CScore>();
    reserveComponent<CHealth>();
    reserveComponent<CInput>();
    reserveComponent<CLifespan>();
    reserveComponent<CDamage>();
    reserveComponent<CInvincibility>();
    reserveComponent<CBBox>();
    reserveComponent<CAnimation>();
    reserveComponent<CGravity>();
    reserveComponent<CState>();
    reserveComponent<CDraggable>();
    reserveComponent<CFollowPlayer>();
    reserveComponent<CPatrol>();
    reserveComponent<CBehavior>();
    reserveComponent<CWeapon>();
    reserveComponent<CPath>();
    reserveComponent<CTrigger>();
    reserveComponent<CTriggerable>();
    reserveComponent<CInteractable>();
}

template <typename T>
void EntityMemoryPool::clearComponent(size_t entity_id) {
    auto& component_vec = std::get<std::vector<T>>(m_pool);
    component_vec[entity_id] = T();
    component_vec[entity_id].has = false;
}

template <typename T>
void EntityMemoryPool::reserveComponent() {
    auto& component_vec = std::get<std::vector<T>>(m_pool);
    component_vec.reserve(MAX_ENTITIES);
    for (size_t i = 0; i < MAX_ENTITIES; i++) {
        component_vec.push_back(T());
    }
}
