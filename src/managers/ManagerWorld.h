#pragma once

#include "Entity.h"
#include "ManagerEntity.h"
#include "GameEngine.h"
#include <memory>

class GameEngine;

class WorldManager
{
  GameEngine* m_gameEngine;
  std::shared_ptr<Entity> m_playerEntity;

  void m_createPlayer(EntityManager& entityManager);

 public:
  WorldManager(GameEngine* gameEngine);

  std::shared_ptr<Entity> getPlayer() const;

  friend class SceneIntro;
};
