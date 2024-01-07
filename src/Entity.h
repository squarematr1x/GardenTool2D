#pragma once

#include <memory>
#include <string>
#include <tuple>

#include "Components.h"

// Get stuff from tuple std::get<typename>(my_tuple)
typedef std::tuple<
	CTransform,
	CLifespan,
	CInput,
	CBBox,
	CAnimation,
	CGravity,
	CState
> ComponentTuple;

enum class ETag : unsigned char {
	DEFAULT,
	TILE,
	DEC,
	PLAYER,
	ENEMY,
	BULLET
};

class Entity
{
	friend class EntityManager; // EntityManager can construct Entities

	Entity(const size_t id, const ETag tag) // Only EntityManager can access this
		: m_id(id), m_tag(tag) {
    }

	const size_t m_id{ 0 };
	const ETag m_tag{ ETag::DEFAULT };
	bool m_alive{ true };
	ComponentTuple m_components;

public:
	ETag tag() const { return m_tag; }
	bool isActive() const { return m_alive; }
	size_t id() const { return m_id; }

	void destroy() { m_alive = false; }

	template<typename T>
	bool hasComponent() const { return getComponent<T>().has; }

	template<typename T, typename... TArgs>
	T& addComponent(TArgs&&... args) { // Overwrites the previous component
		auto& component = getComponent<T>();
		component = T(std::forward<TArgs>(args)...);
		component.has = true;
		return component;
	}

	template<typename T>
	const T& getComponent() const { return std::get<T>(m_components); }

	template<typename T>
	T& getComponent() { return std::get<T>(m_components); }

	template<typename T>
	void removeComponent() { getComponent<T>() = T(); }
};
