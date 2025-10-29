#include "ManagerEntity.h"
#include "Entity.h"
#include <algorithm>

EntityManager::EntityManager(){}

std::shared_ptr<Entity> EntityManager::addEntity(const std::string &tag){
  auto e = std::shared_ptr<Entity>(new Entity(m_totalEntities++, tag));
  m_toAdd.push_back(e);
  return e;
}

void EntityManager::update(){
  for(auto& e : m_toAdd){
    m_entities.push_back(e);
    m_entityMap[e->tag()].push_back(e);
  }

  for(auto& e : m_entities){
    if(e->isDead()){
      auto& vec = m_entityMap[e->tag()];
      vec.erase(std::remove(vec.begin(), vec.end(), e), vec.end());
    }
  }
  m_entities.erase(std::remove_if(m_entities.begin(), m_entities.end(), [](const std::shared_ptr<Entity>& e){return e->isDead(); }), m_entities.end());

  m_toAdd.clear();
}

EntityVec& EntityManager::getEntities(){
  return m_entities;
}

EntityVec& EntityManager::getEntities(const std::string& tag){
  return m_entityMap[tag];
}

