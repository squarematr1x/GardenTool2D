#include "EntityManager.h"

std::shared_ptr<Entity> EntityManager::addEntity(const Tag tag) {
	auto e = std::shared_ptr<Entity>(new Entity(m_total_entities++, tag));
	m_entities_to_add.push_back(e);
	return e;
}

std::shared_ptr<Entity> EntityManager::getEntity(const size_t id) {
	for (auto& e : m_entities) {
		if (e->id() == id) {
			return e;
		}
	}
	return nullptr;
}

void EntityManager::update() {
	for (auto& e : m_entities_to_add) {
		m_entities.push_back(e);
		m_entity_map[e->tag()].push_back(e);
	}

	removeDeadEntities(m_entities);

	for (auto& [tag, entityVec] : m_entity_map) {
		removeDeadEntities(entityVec);
	}

	m_entities_to_add.clear();
}

void EntityManager::removeDeadEntities(EntityVec& vec) {
	vec.erase(std::remove_if(vec.begin(), vec.end(),
		[](std::shared_ptr<Entity> e) { return !e->isActive(); }), vec.end());
}

EntityVec& EntityManager::getEntities(const Tag tag) {
	return m_entity_map[tag];
}
