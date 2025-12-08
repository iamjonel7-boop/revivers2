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
		GameEngine* m_gameEngine;

		using Entity = std::shared_ptr<Entity>;
		Entity m_playerEntity;

		std::vector<Entity> m_population;

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

		int m_nativeBirths = 0; //i am still confuse sometimes if i should use static or not
		int m_imperialBirths = 0;
		int m_nativeDeaths = 0;
		int m_imperialDeaths = 0;

		WorldManager(GameEngine* gameEngine);

		Entity getPlayer() const;
		Entity getPopulation();

		int getNativeBirths() const;
		int getImperialBirths() const;
		int getNativeDeaths() const;
		int getImperialDeaths() const;

		void resetEntityChanges();
		//	friend class SceneIntro;
private:
		bool m_gameOver = false;
		std::string m_gameOverReason;
};
