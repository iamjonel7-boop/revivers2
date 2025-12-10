#pragma once

#include "Entity.h"
#include <vector>
#include <string>
#include <map>

typedef std::vector<std::shared_ptr<Entity>> EntityVec;
typedef std::map<std::string, EntityVec> EntityMap;

class EntityManager{
		EntityVec m_entities;
		EntityVec m_toAdd; //delaying entities to add, to avoid iterator invalidation
		EntityMap m_entityMap;
		size_t m_totalEntities = 0;

public:
		EntityManager();
		void update(); //called at the beginning of frame to add the entities
		std::shared_ptr<Entity> addEntity(const std::string& tag);
		EntityVec& getEntities();
		EntityVec& getEntities(const std::string& tag);
};
