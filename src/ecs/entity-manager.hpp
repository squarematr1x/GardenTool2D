#pragma once

#include <map>

#include "entity.hpp"

class EntityManager {
	std::vector<Entity> m_entities;
	std::vector<Entity> m_entities_to_add;
	std::map<Tag, std::vector<Entity>> m_entity_map;

	void removeDeadEntities(std::vector<Entity>& vec);

public:
	EntityManager() {}

	Entity& getEntity(const size_t id);
	Entity addEntity(const Tag tag);

	std::vector<Entity>& getEntities() { return m_entities; }
	std::vector<Entity>& getEntities(const Tag tag);

	bool isActive(const size_t id) const { return EntityMemoryPool::Instance().isActive(id); }

	void update();
	void setTag(size_t id, const Tag tag);
};
