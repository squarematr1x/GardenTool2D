#pragma once

#include <vector>
#include <map>

#include "Entity.h"

typedef std::vector<std::shared_ptr<Entity>> EntityVec;
typedef std::map<std::string, EntityVec> EntityMap;

class EntityManager
{
public:
	EntityManager()
	{
	}

	std::shared_ptr<Entity> addEntity(const std::string tag);

	EntityVec& getEntities() { return m_entities; }
	EntityVec& getEntities(const std::string tag);

	void update();

private:
	EntityVec m_entities;
	EntityVec m_entities_to_add;
	EntityMap m_entity_map;
	size_t m_total_entities{ 0 };

	void removeDeadEntities(EntityVec& vec);
};
