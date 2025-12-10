#include "ManagerWorld.h"
#include "Components.h"
#include <iostream>
#include <string>

float WorldManager::timeScale = 1.0f;
float WorldManager::monthsPassed = 0.0f;
int WorldManager::birthRate = 10; //births per year per 100 population
int WorldManager::deathRate = 8; //dethas per yer per 100 population

int WorldManager::population = 100;
int WorldManager::imperialEthnicity = 40;
int WorldManager::nativEthnicity = 60;
int WorldManager::currentYear = 0;
int WorldManager::imperialSpeakers = 70;
int WorldManager::nativeSpeakers = 30;

WorldManager::WorldManager(GameEngine* gameEngine) :
		m_gameEngine(gameEngine)
{
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

void WorldManager::updateTime(float deltaTime)
{
		//	std::cout << "deltaTime = " << deltaTime
		//         << ", timeScale = " << timeScale << std::endl;

		if(timeScale <= 0.0f)
				return;

		//timescale = 1.0 -> 1 year per second -> 12 months per second
		//so monthsPerSecond = 12.0f*timeScale
		float monthsPerSecond = 12.0f*timeScale;
		monthsPassed += monthsPerSecond*deltaTime;

		if(monthsPassed >= 12) //ervy 12 months = 1 year
		{
				currentYear++;
				monthsPassed -=12;
				simulatePopulationChanges();
				checkGameOver();
		}
}

void WorldManager::simulatePopulationChanges()
{
		int oldNativeEthnicity = nativEthnicity;
		int oldImperialEthnicity = imperialEthnicity;
		int oldNativeSpeakers = nativeSpeakers;
		int oldImperialSpeakers = imperialSpeakers;

		std::random_device rd;
		std::mt19937 gen(rd());

		//death----------------
		int deathsThisYear = population*deathRate/100;


		// Calculate deaths with rounding
		int nativeDeaths = 0;
		int imperialDeaths = 0;

		if (nativEthnicity > 0)
		{
				float exactNativeDeaths = static_cast<float>(deathsThisYear * nativEthnicity) / population;
				nativeDeaths = static_cast<int>(exactNativeDeaths);

				// Handle fractional part with probability
				float fractionalPart = exactNativeDeaths - nativeDeaths;
				std::uniform_real_distribution<> probDist(0.0f, 1.0f);
				if (probDist(gen) < fractionalPart)
				{
						nativeDeaths++;
				}
		}

		if (imperialEthnicity > 0)
		{
				float exactImperialDeaths = static_cast<float>(deathsThisYear * imperialEthnicity) / population;
				imperialDeaths = static_cast<int>(exactImperialDeaths);

				// Handle fractional part with probability
				float fractionalPart = exactImperialDeaths - imperialDeaths;
				std::uniform_real_distribution<> probDist(0.0f, 1.0f);
				if (probDist(gen) < fractionalPart)
				{
						imperialDeaths++;
				}
		}

		// Ensure we don't kill more than exist
		nativeDeaths = std::min(nativeDeaths, nativEthnicity);
		imperialDeaths = std::min(imperialDeaths, imperialEthnicity);

		nativEthnicity -= nativeDeaths;
		imperialEthnicity -= imperialDeaths;


		if (oldNativeEthnicity > 0 && nativeDeaths > 0)
		{
				int nativeSpeakersLost = (oldNativeSpeakers * nativeDeaths) / oldNativeEthnicity;
				nativeSpeakers -= nativeSpeakersLost;
		}

		if (oldImperialEthnicity > 0 && imperialDeaths > 0)
		{
				// Imperial deaths reduce Imperial speakers
				int imperialSpeakersLost = (oldImperialSpeakers * imperialDeaths) / oldImperialEthnicity;
				imperialSpeakers -= imperialSpeakersLost;
		}

		nativeSpeakers = std::max(0, nativeSpeakers);
		imperialSpeakers = std::max(0, imperialSpeakers);

		m_nativeDeaths = nativeDeaths;
		m_imperialDeaths = imperialDeaths;

		//birth--------------
		int birthsThisYear = population*birthRate/100;

		for(int i = 0; i < birthsThisYear; i++)
		{
				if (nativEthnicity <= 0)
				{
						// Only imperial ethnicity left
						imperialEthnicity++;
						imperialSpeakers++;
						m_imperialBirths++;
						continue;
				}

				std::uniform_real_distribution<> ethDist(0.0f, 100.f);
				float roll = ethDist(gen);
				float nativeEthnicityPercent = (static_cast<float>(nativEthnicity) * 100.0f) / population;

				bool parentIsNative = roll <= nativeEthnicityPercent;

				// Determine child's language based on DOMINANT language in society
				bool learnNativeLanguage = nativeSpeakers >= imperialSpeakers;

				// BONUS: If parent is native AND native language is still strong (>40%),
				// higher chance child learns native
				if (parentIsNative)
				{
						float nativeSpeakerPercent = (static_cast<float>(nativeSpeakers) * 100.0f) / population;
						if (nativeSpeakerPercent > 40.0f)
						{
								// Strong native language presence - native parents teach native
								learnNativeLanguage = true;
						}
						else if (nativeSpeakerPercent > 20.0f)
						{
								// Moderate presence - 70% chance to learn native from native parent
								std::uniform_real_distribution<> languageDist(0.0f, 100.0f);
								learnNativeLanguage = languageDist(gen) < 70.0f;
						}
						// else: weak presence, follows dominant language
				}

				// Handle native parent births
				if(parentIsNative)
				{
						nativEthnicity++;  // Child inherits native ethnicity

						if(learnNativeLanguage)
						{
								nativeSpeakers++;
								m_nativeBirths++;
						}
						else
						{
								// Native ethnicity but learns imperial language (assimilation)
								imperialSpeakers++;
								m_imperialBirths++;
						}
				}
				else  // Handle imperial parent births - THIS WAS MISSING!
				{
						imperialEthnicity++;  // Child inherits imperial ethnicity

						// Imperial parents almost always teach imperial
						// Only if native language is VERY dominant (>75%) might they learn native
						if (learnNativeLanguage && nativeSpeakers > imperialSpeakers * 3)
						{
								// Rare case: native language so dominant even imperial kids learn it
								nativeSpeakers++;
								m_nativeBirths++;
						}
						else
						{
								imperialSpeakers++;
								m_imperialBirths++;
						}
				}
		}

// Update total population
		population = nativEthnicity + imperialEthnicity;

// Ensure speaker counts are valid
		if (nativeSpeakers <= 0)
		{
				imperialSpeakers = population;
				nativeSpeakers = 0;
		}

// Ensure speaker counts don't exceed population
		nativeSpeakers = std::min(nativeSpeakers, population);
		imperialSpeakers = std::min(imperialSpeakers, population);

		int totalSpeakers = nativeSpeakers + imperialSpeakers;
		if (totalSpeakers < population)
		{
				imperialSpeakers += (population - totalSpeakers);
		}

		std::cout << "\n=== YEAR " << currentYear << " ===" << std::endl;
		std::cout << "Population: " << population << std::endl;
		std::cout << "Native Ethnicity: " << nativEthnicity
				  << " (Deaths: " << m_nativeDeaths
				  << ", Births: " << m_nativeBirths << ")" << std::endl;
		std::cout << "Imperial Ethnicity: " << imperialEthnicity
				  << " (Deaths: " << m_imperialDeaths
				  << ", Births: " << m_imperialBirths << ")" << std::endl;
		std::cout << "Native Speakers: " << nativeSpeakers << std::endl;
		std::cout << "Imperial Speakers: " << imperialSpeakers << std::endl;
		std::cout << "Total Deaths: " << (m_nativeDeaths + m_imperialDeaths) << std::endl;
		std::cout << "Total Births: " << birthsThisYear << std::endl;
		std::cout << "=================\n" << std::endl;

}

bool WorldManager::isVictory() const
{
		return m_isVictory;
}

void WorldManager::checkGameOver()
{
		// Game over if native speakers disappear
		/*	if(nativEthnicity <= 0)
			{
			m_gameOver = true;
			m_gameOverReason = "Native language has gone extinct!";
			std::cout << "GAME OVER: " << m_gameOverReason << std::endl;
			return;
			}

			// Game over if population collapses
			if(population <= 10)
			{
			m_gameOver = true;
			m_gameOverReason = "Population collapsed!";
			std::cout << "GAME OVER: " << m_gameOverReason << std::endl;
			return;
			}

			// Game over if imperial dominance is too high
			if(nativeSpeakers > 0)
			{
			float ratio = static_cast<float>(imperialSpeakers) / nativeSpeakers;
			if(ratio >= 20.0f)  // Adjust this threshold as needed
			{
			m_gameOver = true;
			m_gameOverReason = "Imperial language dominance too high!";
			std::cout << "GAME OVER: " << m_gameOverReason << std::endl;
			return;
			}
			}*/
		// === WIN CONDITION ===
		// Win if native speakers are 90% or more of population
		if (nativeSpeakers > 0 && population > 0)
		{
				float nativeSpeakerRatio = static_cast<float>(nativeSpeakers) / population;
				if (nativeSpeakerRatio >= 0.90f)  // 90% native speakers = victory!
				{
						m_gameOver = true;
						m_isVictory = true;
						m_gameOverReason = "The native language thrives! 90% of the population speaks it!";
						std::cout << "VICTORY: " << m_gameOverReason << std::endl;
						return;
				}
		}

		// === LOSS CONDITIONS ===

		// Loss 1: Native ethnicity extinct
		if (nativEthnicity <= 0)
		{
				m_gameOver = true;
				m_isVictory = false;
				m_gameOverReason = "Native population extinct - language lost forever";
				std::cout << "GAME OVER: " << m_gameOverReason << std::endl;
				return;
		}

		// Loss 2: Population collapse
		if (population <= 10)
		{
				m_gameOver = true;
				m_isVictory = false;
				m_gameOverReason = "Population collapsed - too few to sustain the language";
				std::cout << "GAME OVER: " << m_gameOverReason << std::endl;
				return;
		}

		// Loss 3: Native speakers nearly extinct (less than 10% of population)
		if (nativeSpeakers > 0 && population > 0)
		{
				float nativeSpeakerRatio = static_cast<float>(nativeSpeakers) / population;
				if (nativeSpeakerRatio < 0.10f)  // Less than 10% = defeat
				{
						m_gameOver = true;
						m_isVictory = false;
						m_gameOverReason = "Native language speakers fell below 10% - language dying out";
						std::cout << "GAME OVER: " << m_gameOverReason << std::endl;
						return;
				}
		}

		// Loss 4: No native speakers left at all
		if (nativeSpeakers <= 0)
		{
				m_gameOver = true;
				m_isVictory = false;
				m_gameOverReason = "No native speakers remain - language extinct";
				std::cout << "GAME OVER: " << m_gameOverReason << std::endl;
				return;
		}
}

bool WorldManager::isGameOver() const
{
		return m_gameOver;
}


int WorldManager::getNativeBirths() const {return m_nativeBirths;}
int WorldManager::getImperialBirths() const {return m_imperialBirths;}
int WorldManager::getNativeDeaths() const {return m_nativeDeaths;}
int WorldManager::getImperialDeaths() const {return m_imperialDeaths;}

void WorldManager::resetEntityChanges()
{
		m_nativeBirths = m_imperialBirths = m_nativeDeaths = m_imperialDeaths = 0;
}

void WorldManager::updatePopulationEntities(std::vector<std::shared_ptr<Entity>>& nativEntities, std::vector<std::shared_ptr<Entity>>& imperialEntities, EntityManager& entityMgr)
{
		removeDeadEntities(nativEntities, imperialEntities, entityMgr);
		addNewBornEntities(nativEntities, imperialEntities, entityMgr);
		resetEntityChanges();
}

void WorldManager::removeFromTileEntities(EntityManager& entityMgr, std::shared_ptr<Entity>& entityTobeRemoved)
{
		auto& paths = entityMgr.getEntities("path");
		for(auto& path: paths)
		{
				if(path->hasComponent<CPath>())
				{
						auto& tileEntities = path->getComponent<CPath>().entities;
						tileEntities.erase(std::remove(tileEntities.begin(),
													   tileEntities.end(), entityTobeRemoved),
										   tileEntities.end());
				}
		}
}

void WorldManager::removeDeadEntities(std::vector<std::shared_ptr<Entity>>& nativEntities, std::vector<std::shared_ptr<Entity>>& imperialEntities, EntityManager& entityMgr)
{
		int nativeToRemove = std::min(m_nativeDeaths, (int)nativEntities.size());
		for(int i = 0; i < nativeToRemove; i++)
		{
				if(!nativEntities.empty())
				{
						auto entity = nativEntities.back();
						nativEntities.pop_back();
						removeFromTileEntities(entityMgr, entity);
						entity->destroy();
				}
		}

		int imperialToRemove = std::min(m_imperialDeaths, (int)imperialEntities.size());
		for(int i = 0; i < imperialToRemove; i++)
		{
				if(!imperialEntities.empty())
				{
						auto entity = imperialEntities.back();
						imperialEntities.pop_back();
						removeFromTileEntities(entityMgr, entity);
						entity->destroy();
				}
		}
}

bool WorldManager::m_overSpawned(std::vector<std::shared_ptr<Entity>>& nativEntities, std::vector<std::shared_ptr<Entity>>& imperialEntities)
{
		return nativEntities.size() + imperialEntities.size() >= static_cast<size_t>(MAX_VISIBLE_ENTITIES);
}

void WorldManager::addToTileEntities(EntityManager& entityMgr, std::shared_ptr<Entity>& randomPath, std::vector<std::shared_ptr<Entity>>& civilians, size_t randomIndex)
{
		if(randomPath->hasComponent<CPath>())
		{
				auto civ = entityMgr.addEntity("nat_ethnic");
				civ->addComponent<CPopulation>(Ethnicity::NATIVE_CIV, Language::NATIVE);

				auto& tileEntityList = randomPath->getComponent<CPath>().entities;
				tileEntityList.push_back(civ);
				civilians.push_back(civ);

				auto& pos = randomPath->getComponent<CTransform>();
				std::cout << "[POPULATION] Native Civilian spawned on path at ("
						  << pos.position.x << ", "
						  << pos.position.y << ") — index "
						  << randomIndex << std::endl;
		}
}

void WorldManager::addNewBornEntities(std::vector<std::shared_ptr<Entity>>& nativEntities, std::vector<std::shared_ptr<Entity>>& imperialEntities, EntityManager& entityMgr)
{
		auto& pathEntities = entityMgr.getEntities("path");
		if(pathEntities.empty()) return;

		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<size_t> dist(0, pathEntities.size() - 1);

		if(m_overSpawned(nativEntities, imperialEntities)) return;

		for(int i = 0; i < m_nativeBirths; i++)
		{
				size_t randomIndex = dist(gen);
				auto& pathTile = pathEntities[randomIndex];
				addToTileEntities(entityMgr, pathTile, nativEntities, randomIndex);
		}

		for(int i = 0; i < m_imperialBirths; i++)
		{
				size_t randomIndex = dist(gen);
				auto& pathTile = pathEntities[randomIndex];
				addToTileEntities(entityMgr, pathTile, imperialEntities, randomIndex);
		}
}

void WorldManager::createPopulation(std::vector<std::shared_ptr<Entity>>& nativEntities, std::vector<std::shared_ptr<Entity>>& imperialEntities, EntityManager& entityMgr)
{
		nativEntities.clear(); imperialEntities.clear();

		auto& pathEntities = entityMgr.getEntities("path");

		if(pathEntities.empty())
		{
				std::cout << "[POPULATION] ERROR: No path tiles exist!" << std::endl; return;
		}

		std::random_device rd; std::mt19937 gen(rd());
		std::uniform_int_distribution<size_t> dist(0, pathEntities.size() - 1);

		int natCivToCreate = static_cast<int>(NATIVE_TO_IMPERIAL_RATIO * MAX_VISIBLE_ENTITIES);
		int impCivToCreate = MAX_VISIBLE_ENTITIES - natCivToCreate;

		std::cout << "[POPULATION] Spawning " << nativEthnicity << " civilians..." << std::endl;

		for(int i = 0; i < natCivToCreate; i++)
		{
				std::uniform_int_distribution<size_t> dist(0, pathEntities.size() - 1);
				size_t randomIndex = dist(gen);

				auto& pathTile = pathEntities[randomIndex];
				addToTileEntities(entityMgr, pathTile, nativEntities, randomIndex);
		}

		for(int i = 0; i < impCivToCreate; i++)
		{
				std::uniform_int_distribution<size_t> dist(0, pathEntities.size() - 1);
				size_t randomIndex = dist(gen);

				auto& pathTile = pathEntities[randomIndex];
				addToTileEntities(entityMgr, pathTile, imperialEntities, randomIndex);
		}
}

void WorldManager::updateWorld(std::vector<std::shared_ptr<Entity>>& nativEntities, std::vector<std::shared_ptr<Entity>>& imperialEntities, EntityManager& entityMgr)
{
		static int lastYear = -1;
		if(currentYear != lastYear)
		{
				updatePopulationEntities(nativEntities, imperialEntities, entityMgr);
				lastYear = currentYear;
		}
}
