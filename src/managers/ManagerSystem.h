#pragma once

#include "ManagerEntity.h"
#include "ManagerGrammar.h"
#include "System.h"
#include "Entity.h"
#include <memory>
#include <vector>
#include <unordered_map>
#include <functional>
#include <iostream>

class SystemManager
{
private:
   EntityManager* m_entityManager;
   GrammarManager* m_grammarManager;

   //system name -> function the implements the system
   std::unordered_map<std::string, std::unique_ptr<System>> m_systems;

public:
   SystemManager(EntityManager* entityManager, GrammarManager* grammarManager)
      : m_entityManager(entityManager), m_grammarManager(grammarManager)
   {

   }

   //register a system, player inputs verb to register them
   void registerSystem(const std::string& systemName, std::unique_ptr<System> system)
   {
      m_systems[systemName] = std::move(system);
   }

   //execute a system
   bool executeSystem(const std::string& systemName, const std::map<std::string, std::shared_ptr<Entity>>& markedEntities, const std::string& voice, const std::string& aspect)
   {
      auto it = m_systems.find(systemName);
      if(it != m_systems.end() && it->second)
      {
         it->second->execute(markedEntities, voice, aspect);
         return true;
      }
      std::cout << "System not found: " << systemName << std::endl;
      return false;
   }

   //check if a system exists
   bool systemExists(const std::string& systemName) const
   {
      return m_systems.find(systemName) != m_systems.end();
   }

private:
   void registerDefaultSystems(){}

};
