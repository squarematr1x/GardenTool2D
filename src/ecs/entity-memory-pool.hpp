#pragma once

#include <tuple>
#include <queue>

#include "components.hpp"

constexpr size_t MAX_ENTITIES{ 1000 };

typedef std::tuple<
	std::vector<CTransform>,
    std::vector<CScore>,
    std::vector<CHealth>,
    std::vector<CInput>,
    std::vector<CLifespan>,
    std::vector<CDamage>,
    std::vector<CInvincibility>,
    std::vector<CBBox>,
	std::vector<CAnimation>,
	std::vector<CGravity>,
	std::vector<CState>,
	std::vector<CDraggable>,
	std::vector<CFollowPlayer>,
	std::vector<CPatrol>,
    std::vector<CBehavior>,
	std::vector<CWeapon>,
	std::vector<CPath>,
	std::vector<CTrigger>,
	std::vector<CTriggerable>,
	std::vector<CInteractable>,
    std::vector<CBreakable>,
    std::vector<CLightSource>,
    std::vector<CNoise>
> ComponentVectorTuple;

class EntityMemoryPool {
    ComponentVectorTuple m_components;
    std::vector<Tag> m_tags;
    std::vector<bool> m_active;
    size_t m_entity_count{ 0 };
    // std::queue<size_t> m_free_indexes; // NOTE: check later

    EntityMemoryPool(size_t max_entities);

    size_t getNextEntityIndex() const;

    void clearComponents(size_t entity_id);
    void reserveComponents();

    template <typename T>
    void clearComponent(size_t entity_id);  

    template <typename T>
    void reserveComponent();
    
public:
    static EntityMemoryPool & Instance() {
        static EntityMemoryPool pool(MAX_ENTITIES);
        return pool;
    }

    template <typename T>
    T& getComponent(size_t entity_id) { return std::get<std::vector<T>>(m_components)[entity_id]; }

    Tag getTag(size_t entity_id) const { return m_tags[entity_id]; }
    void setTag(Tag tag, size_t entity_id) { m_tags[entity_id] = tag; }

    bool isActive(size_t entity_id) const { return m_active[entity_id]; }

    size_t addEntity(const Tag tag);

    void destroyEntity(size_t entity_id);
    void resetAll();
};
