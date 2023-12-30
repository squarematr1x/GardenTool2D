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

class Entity
{
public:
	// TODO: Guess have to remove these:
	std::shared_ptr<CTransform> cTransform; 
	std::shared_ptr<CShape> cShape;
	std::shared_ptr<CCollision> cCollision;
	std::shared_ptr<CScore> cScore;
	std::shared_ptr<CInput> cInput;
	std::shared_ptr<CLifespan> cLifespan;
	std::shared_ptr<CAnimation> cAnimation;
	std::shared_ptr<CBBox> cBBox;
	std::shared_ptr<CGravity> cGravity;
	std::shared_ptr<CState> cState;

	std::string tag() const { return m_tag; }
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


private:
	friend class EntityManager; // EntityManager can construct Entities

	Entity(const size_t id, const std::string tag) // Only EntityManager can access this
		: m_id(id), m_tag(tag) {
    }

	const size_t m_id{ 0 };
	const std::string m_tag{ "default" };
	bool m_alive{ true };
	ComponentTuple m_components;
};
