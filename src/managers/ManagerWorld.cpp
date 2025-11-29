#include "ManagerWorld.h"
#include <iostream>
#include <string>

WorldManager::WorldManager(GameEngine* gameEngine) :
  m_gameEngine(gameEngine)
{
  currentStage = Stage::FIRST;
}

void WorldManager::m_createPlayer(EntityManager& entityManager)
{
  if(!m_playerEntity)
    {

      m_playerEntity = entityManager.addEntity("player");
      std::cout << "Player entity created." << std::endl;
      m_playerEntity->addComponent<CTransform>();
      std::cout << "Added transform component." << std::endl;
      m_playerEntity->addComponent<CProfile>();
      std::cout << "Added profile component." << std::endl;
    }
}

std::shared_ptr<Entity> WorldManager::getPlayer() const
{
  return m_playerEntity;
}
