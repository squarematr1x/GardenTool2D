#pragma once

#include <memory>
#include <string>
#include <tuple>

#include "entity-memory-pool.hpp"

class Entity {
    size_t m_id;

public:
	Entity() = default;
	Entity(size_t id)
		: m_id(id) 
	{
	}

	size_t id() const { return m_id; }
	Tag tag() const {return EntityMemoryPool::Instance().getTag(m_id); }

    template<typename T>
    T& getComponent() const { return EntityMemoryPool::Instance().getComponent<T>(m_id); }

    template<typename T>
    bool hasComponent() const { return EntityMemoryPool::Instance().getComponent<T>(m_id).has; }
	bool isActive() const { return EntityMemoryPool::Instance().isActive(m_id); }

	template<typename T, typename... TArgs>
	T& addComponent(TArgs&&... args) const {
		auto& component = EntityMemoryPool::Instance().getComponent<T>(m_id);
		component = T(std::forward<TArgs>(args)...);
		component.has = true;
		return component;
	}

	template<typename T>
    void removeComponent() const { EntityMemoryPool::Instance().getComponent<T>(m_id) = T(); }
	void destroy() const { EntityMemoryPool::Instance().destroyEntity(m_id); }
	void setTag(Tag tag) const { EntityMemoryPool::Instance().setTag(tag, m_id); }
};
