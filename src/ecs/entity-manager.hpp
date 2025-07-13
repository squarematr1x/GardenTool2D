#pragma once

#include <map>

#include "entity.hpp"

typedef std::vector<std::shared_ptr<Entity>> EntityVec;

typedef std::map<Tag, EntityVec> EntityMap;

typedef std::tuple<
	std::vector<CTransform>,
	std::vector<CHealth>,
	std::vector<CInput>,
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
	std::vector<CInteractable>,
	std::vector<CPath>
> EntityComponentVectorTuple;

class EntityManager {
	EntityVec m_entities; // TODO: replace with std::vector<Entity> m_entities;
	EntityVec m_entities_to_add;
	EntityMap m_entity_map;
	size_t m_total_entities{ 0 };

	void removeDeadEntities(EntityVec& vec);

public:
	EntityManager() {}

	std::shared_ptr<Entity> addEntity(const Tag tag);
	std::shared_ptr<Entity> getEntity(const size_t id);

	// Entity addEntity(const Tag tag) {
	//   Entity e = EntityMemoryPool::Instance().addEntity(tag);
	//	 m_entities_to_add.push_back(e);
	// 	 return e;
	// }

	EntityVec& getEntities() { return m_entities; }
	EntityVec& getEntities(const Tag tag);

	void update();

	void setTag(size_t id, const Tag tag);
};
