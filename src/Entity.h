#pragma once

#include <memory>
#include <string>

#include "Components.h"

class Entity
{
public:
	std::shared_ptr<CTransform> cTransform;
	std::shared_ptr<CShape> cShape;
	std::shared_ptr<CCollision> cCollision;
	std::shared_ptr<CScore> cScore;
	std::shared_ptr<CInput> cInput;
	std::shared_ptr<CLifespan> cLifespan;

	std::string tag() const { return m_tag; }
	bool isActive() const { return m_alive; }
	size_t id() const { return m_id; }

	void destroy() { m_alive = false; }

private:
	friend class EntityManager; // EntityManager can construct Entities

	Entity(const size_t id, const std::string tag) // Only EntityManager can access this
		: m_id(id), m_tag(tag)
	{
	}

	const size_t m_id{ 0 };
	const std::string m_tag{ "default" };
	bool m_alive{ true };
};

// Note
// template <class T> using ptr = std::shared_ptr<T>;