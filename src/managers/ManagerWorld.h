#pragma once

#include "Entity.h"
#include "ManagerEntity.h"
#include "GameEngine.h"
#include <memory>
#include <random>

class GameEngine;

class WorldManager
{
		GameEngine* m_gameEngine;
		std::shared_ptr<Entity> m_playerEntity;

		void m_createPlayer(EntityManager& entityManager);

		enum class Stage
		{
				FIRST,
				SECOND,
				THIRD,
				FOURTH,
				FIFTH,
				SIXTH,
				SEVENTH,
				EIGHTH
		};

		Stage currentStage;

 public:
  WorldManager(GameEngine* gameEngine);

  std::shared_ptr<Entity> getPlayer() const;

public:
		WorldManager(GameEngine* gameEngine);

		std::shared_ptr<Entity> getPlayer() const;


		friend class SceneIntro;
};
