#pragma once

#include "Entity.h"
#include "ManagerEntity.h"
#include "GameEngine.h"
#include <memory>
#include <random>

class GameEngine;

class WorldManager
{
public:
		WorldManager(GameEngine* gameEngine);
		GameEngine* m_gameEngine;

		std::shared_ptr<Entity> m_playerEntity;

		std::vector<std::shared_ptr<Entity>> m_population;

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

		static float timeScale; //0.0 paused, 1.0 normal
		static float monthsPassed;
		static int birthRate;
		static int deathRate;

		static void togglePause(){timeScale = (timeScale > 0.0f) ? 0.0f : 1.0f;}
		static void setPaused(bool paused){timeScale = paused ? 0.0f : 1.0f;}
		static bool isPause(){return timeScale == 0.0f;}

		void updateTime(float deltaTime);
		void simulatePopulationChanges();
		void checkGameOver();
		bool isGameOver() const;

		static int population;
		static int imperialEthnicity;
		static int nativEthnicity;
		static int currentYear;
		static int nativeSpeakers;
		static int imperialSpeakers;

		int m_nativeBirths = 0;
		int m_imperialBirths = 0;
		int m_nativeDeaths = 0;
		int m_imperialDeaths = 0;

		std::shared_ptr<Entity> getPlayer() const;
		std::vector<std::shared_ptr<Entity>> getPopulation();

		int getNativeBirths() const;
		int getImperialBirths() const;
		int getNativeDeaths() const;
		int getImperialDeaths() const;

		void resetEntityChanges();
		void updatePopulationEntities(std::vector<std::shared_ptr<Entity>>& nativEntities, std::vector<std::shared_ptr<Entity>>& imperialEntities, EntityManager& entityMgr);
		void removeDeadEntities(std::vector<std::shared_ptr<Entity>>& nativEntities, std::vector<std::shared_ptr<Entity>>& imperialEntities, EntityManager& entityMgr);
		void addNewBornEntities(std::vector<std::shared_ptr<Entity>>& nativEntities, std::vector<std::shared_ptr<Entity>>& imperialEntities, EntityManager& entityMgr);
		void removeFromTileEntities(EntityManager& entityMgr, std::shared_ptr<Entity>& entityTobeRemoved);
		void addToTileEntities(EntityManager& entityMgr, std::shared_ptr<Entity>& randomPath, std::vector<std::shared_ptr<Entity>>& civilians, size_t randomIndex);
		void createPopulation(std::vector<std::shared_ptr<Entity>>& nativEntities, std::vector<std::shared_ptr<Entity>>& imperialEntities, EntityManager& entityMgr);
		void updateWorld(std::vector<std::shared_ptr<Entity>>& nativEntities, std::vector<std::shared_ptr<Entity>>& imperialEntities, EntityManager& entityMgr);

private:
		const int MAX_VISIBLE_ENTITIES = 100;
		const float NATIVE_TO_IMPERIAL_RATIO = 0.6f;
		bool m_gameOver = false;
		std::string m_gameOverReason;
		bool m_overSpawned(std::vector<std::shared_ptr<Entity>>& nativEntities, std::vector<std::shared_ptr<Entity>>& imperialEntities);
};
