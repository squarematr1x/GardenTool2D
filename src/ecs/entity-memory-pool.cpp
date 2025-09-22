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

void EntityMemoryPool::clearComponents(size_t entity_id) {
    clearComponent<CTransform>(entity_id);
    clearComponent<CScore>(entity_id);
    clearComponent<CHealth>(entity_id);
    clearComponent<CInput>(entity_id);
    clearComponent<CLifespan>(entity_id);
    clearComponent<CDamage>(entity_id);
    clearComponent<CInvincibility>(entity_id);
    clearComponent<CBBox>(entity_id);
    clearComponent<CAnimation>(entity_id);
    clearComponent<CGravity>(entity_id);
    clearComponent<CState>(entity_id);
    clearComponent<CDraggable>(entity_id);
    clearComponent<CFollowPlayer>(entity_id);
    clearComponent<CPatrol>(entity_id);
    clearComponent<CBehavior>(entity_id);
    clearComponent<CWeapon>(entity_id);
    clearComponent<CPath>(entity_id);
    clearComponent<CTrigger>(entity_id);
    clearComponent<CTriggerable>(entity_id);
    clearComponent<CInteractable>(entity_id);
    clearComponent<CBreakable>(entity_id);
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
    reserveComponent<CBreakable>();
}

template <typename T>
void EntityMemoryPool::clearComponent(size_t entity_id) {
    auto& component_vec = std::get<std::vector<T>>(m_components);
    component_vec[entity_id] = T();
}

template <typename T>
void EntityMemoryPool::reserveComponent() {
    auto& component_vec = std::get<std::vector<T>>(m_components);
    component_vec.reserve(MAX_ENTITIES);
    for (size_t i = 0; i < MAX_ENTITIES; i++) {
        component_vec.push_back(T());
    }
}

void EntityMemoryPool::resetAll() {
    for (size_t i = 0; i < MAX_ENTITIES; i++) {
        m_active.push_back(false);
        m_tags.push_back(Tag::DEFAULT);
    }
    reserveComponents();
}
