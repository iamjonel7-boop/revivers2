#include "SceneMain.h"
#include "Components.h"
#include "GameEngine.h"
#include "ManagerMap.h"
#include "ManagerWorld.h"
#include "SceneInventory.h"
#include <iostream>
#include <fstream>

using SMain = SceneMain;

SMain::SceneMain(GameEngine* gameEngine) :
		Scene(gameEngine)
{
		init();

		m_controlState = MapControlState::NAVIGATING;
		std::cout << "Navigating state.";

		registerAction(sf::Keyboard::W, static_cast<int>(ActionName::MOVE_UP));
		registerAction(sf::Keyboard::A, static_cast<int>(ActionName::MOVE_LEFT));
		registerAction(sf::Keyboard::S, static_cast<int>(ActionName::MOVE_DOWN));
		registerAction(sf::Keyboard::D, static_cast<int>(ActionName::MOVE_RIGHT));
		//	registerAction(sf::Keyboard::Space, static_cast<int>(ActionName::SELECT_TILE));
		registerAction(sf::Keyboard::Tab, static_cast<int>(ActionName::OPEN_INVENTORY));
		registerAction(sf::Keyboard::E, static_cast<int>(ActionName::SELECT_TILE));
		registerAction(sf::Keyboard::Q, static_cast<int>(ActionName::RETURN));
}

void SMain::init()
{
		makeMapView();
		makeMapGrid();

		m_mapManager.locateBuildings();
		m_mapManager.printBuildingCoords();
		makeBuildings();

		m_mapManager.locatePaths();
		m_mapManager.printPathCoords();
		makePaths();
		makeStatistics();

		m_entities.update(); //solve paths not exist

		makeCursor();

		createPopulation();

		auto  m_player = m_game->getWorldManager()->getPlayer();

		std::cout << "=== SCENE MAIN ===" << std::endl;
		if (m_player)
		{
				std::cout << "Player memory address: " << m_player.get() << std::endl;
				std::cout << "Player name: " << m_player->getComponent<CProfile>().playerName << std::endl;
				std::cout << "Has CTransform: " << m_player->hasComponent<CTransform>() << std::endl;
				std::cout << "Has CProfile: " << m_player->hasComponent<CProfile>() << std::endl;
		} else
		{
				std::cout << "ERROR: Player is null!" << std::endl;
		}

		m_sentencePanel = createBox({0.f, 450.f}, {800.f, 40.f}, sf::Color::Black);
		m_sentencingPanel = createBox({0.f, 480.f}, {600.f, 120.f}, sf::Color::Black);
		m_sentencingPanel.setOutlineThickness(1.f);
		m_sentencingPanel.setOutlineColor(sf::Color::White);

		m_font.loadFromFile("/usr/local/share/fonts/Liberation/LiberationMono-Regular.ttf");

		m_helpMsg = createText("", m_font, {610.f, 388.f}, 10, sf::Color::White);
		m_helpMsg2 = createText("", m_font, {610.f, 415.f}, 10, sf::Color::White);
}

void SMain::update()
{
		m_entities.update();

		float deltaTime = m_game->getDeltaTime();

		m_game->getWorldManager()->updateTime(deltaTime);

		updateStatistics();

		std::string help1, help2;

		const auto& func = m_updateTable[static_cast<size_t>(m_controlState)];
		if(func != nullptr) (this->*func)(help1, help2);

		m_helpMsg.setString(help1);
		m_helpMsg2.setString(help2);

		if(m_game->getWorldManager()->isGameOver())
		{
				// Handle game over - change scene or show message
				std::cout << "Game Over detected in SceneMain!" << std::endl;
				// m_game->changeScene("gameover", ...);
		}
}

void SceneMain::makeStatistics()
{
		std::string a = "Population: " + std::to_string(WorldManager::population);
		m_totalPopulation = createText(a, m_font, {7.f, 6.f}, 10, sf::Color::White);

		std::string b = "Year: " + std::to_string(WorldManager::currentYear);
		m_year = createText(b, m_font, {7.f, 23.f}, 10, sf::Color::White);

		std::string c = "Native Ethnicity: " + std::to_string(WorldManager::nativEthnicity);
		m_nativEthnicity = createText(c, m_font, {142.f, 6.f}, 10, sf::Color::White);

		std::string d = "Imperial Ethnicity: " + std::to_string(WorldManager::imperialEthnicity);
		m_imperialEthnicity = createText(d, m_font, {142.f, 23.f}, 10, sf::Color::White);

		std::string e = "Native Language Speakers: " + std::to_string(WorldManager::nativeSpeakers);
		m_nativeSpeakers = createText(e, m_font, {300.f, 6.f}, 10, sf::Color::White);

		std::string f = "Imperial Language Speakers: " + std::to_string(WorldManager::imperialSpeakers);
		m_imperialSpeakers = createText(f, m_font, {300.f, 23.f}, 10, sf::Color::White);
}

void SceneMain::updateStatistics()
{
		std::string a = "Population: " + std::to_string(WorldManager::population);
		m_totalPopulation.setString(a);

		std::string b = "Year: " + std::to_string(WorldManager::currentYear);
		m_year.setString(b);

		std::string c = "Native Ethnicity: " + std::to_string(WorldManager::nativEthnicity);
		m_nativEthnicity.setString(c);

		std::string d = "Imperial Ethnicity: " + std::to_string(WorldManager::imperialEthnicity);
		m_imperialEthnicity.setString(d);

		std::string e = "Native Language Speakers: " + std::to_string(WorldManager::nativeSpeakers);
		m_nativeSpeakers.setString(e);

		std::string f = "Imperial Language Speakers: " + std::to_string(WorldManager::imperialSpeakers);
		m_imperialSpeakers.setString(f);
}

void SceneMain::updateSentencing(std::string& help1, std::string& help2)
{
		help1 = "Press Q to return.";
		help2 = "";
}

void SceneMain::updateNavigating(std::string& help1, std::string& help2)
{
		help1 = "Use the WASD keys to navigate\nthe map.";
		help2 = "Press Space to select a tile.";
		updateCursorPos();
}

void SceneMain::removeDeadEntities()
{
}

void SceneMain::addNewBornEntities()
{
}

void SceneMain::redistributeEntities()
{
}

void SceneMain::createPopulation()
{
		auto& pathEntities = m_entities.getEntities("path");

		std::random_device rd;
		std::mt19937 gen(rd());

		std::cout << "[POPULATION] Spawning "
				  << WorldManager::nativEthnicity
				  << " civilians..." << std::endl;

		if (pathEntities.empty())
		{
				std::cout << "[POPULATION] ERROR: No path tiles exist!" << std::endl;
				return;
		}

		for(int i = 0; i < WorldManager::nativEthnicity; i++)
		{
				std::uniform_int_distribution<size_t> dist(0, pathEntities.size() - 1);
				size_t randomIndex = dist(gen);

				auto& pathTile = pathEntities[randomIndex];

				if(pathTile->hasComponent<CPath>())
				{
						auto civ = m_entities.addEntity("nat_ethnic");
						civ->addComponent<CPopulation>(Ethnicity::NATIVE_CIV, Language::IMPERIAL);

						auto& tileEntityList = pathTile->getComponent<CPath>().entities;
						tileEntityList.push_back(civ);

						auto& pos = pathTile->getComponent<CTransform>();
						std::cout << "[POPULATION] Native Civilian spawned on path at ("
								  << pos.position.x << ", "
								  << pos.position.y << ") — index "
								  << randomIndex << std::endl;
			    }

		}

		for(int i = 0; i < WorldManager::imperialEthnicity; i++)
		{
				std::uniform_int_distribution<size_t> dist(0, pathEntities.size() - 1);
				size_t randomIndex = dist(gen);

				auto& pathTile = pathEntities[randomIndex];

				if(pathTile->hasComponent<CPath>())
				{
						auto civ = m_entities.addEntity("imp_ethnic");
						civ->addComponent<CPopulation>(Ethnicity::IMPERIAL_CIV, Language::IMPERIAL);

						auto& tileEntityList = pathTile->getComponent<CPath>().entities;
						tileEntityList.push_back(civ);

						auto& pos = pathTile->getComponent<CTransform>();
						std::cout << "[POPULATION] Imperial Civilian spawned on path at ("
								  << pos.position.x << ", "
								  << pos.position.y << ") — index "
								  << randomIndex << std::endl;
			    }
		}
}

void SMain::sDoAction(const Action& action)
{
		ActionName act = static_cast<ActionName>(action.name());
		auto& cinput = m_cursorEntity->getComponent<CInput>();
		switch(m_controlState)
		{
		case MapControlState::NAVIGATING:
				handleCursorNavigation(action, act, cinput);
				break;
		case MapControlState::SENTENCING:
				handleSentencing(act);
				break;
		default:
				break;
		}
}

void SceneMain::onEnd()
{
		m_game->quit();
}

void SceneMain::sRender()
{
		m_game->window().setView(m_mainView);

		for (auto& e : m_entities.getEntities())
		{
				if (e->hasComponent<CShape>() && e->hasComponent<CTransform>())
				{
						auto& shape = e->getComponent<CShape>().shape;
						auto& transform = e->getComponent<CTransform>();

						shape->setPosition(transform.position.x, transform.position.y);
						m_game->window().draw(*shape);
				}
		}

		renderTileGrid();
		renderBuildings();
		renderCursor();

		m_game->window().setView(m_game->window().getDefaultView());
		m_game->window().draw(m_sentencePanel);
		m_game->window().draw(m_sentencingPanel);

		m_game->window().draw(m_helpMsg);
		m_game->window().draw(m_helpMsg2);

		m_game->window().draw(m_totalPopulation);
		m_game->window().draw(m_year);
		m_game->window().draw(m_nativEthnicity);
		m_game->window().draw(m_imperialEthnicity);
		m_game->window().draw(m_imperialSpeakers);
		m_game->window().draw(m_nativeSpeakers);
		renderTileEntities(m_tileEntities);
}

void SceneMain::renderTileEntities(std::vector<std::shared_ptr<Entity>> tileEntities)
{
		sf::Text a;
		float space = 200.f;
		int b = 0;
		int n = 0;
		int i = 0;
		if(tileEntities.empty())
		{
				a =  createText("Nothing is in this tile", m_font, {58.f, 533.f}, 12, sf::Color::White);
				//m_game->window().draw(a); //fix this
		}
		else
		{
				for(const auto& e : tileEntities)
				{
						if(e->hasComponent<CPopulation>())
						{
								auto& ethnicity = e->getComponent<CPopulation>().ethnicity;
								std::string e;
								switch(ethnicity)
								{
								case Ethnicity::NATIVE_CIV:
										e = "Native Civilian";
										a = createText(e, m_font, {20.f+(space*i), 505.f}, 12, sf::Color::White);
										n++;
										break;
								case Ethnicity::IMPERIAL_CIV:
										e = "Imperial Civilian";
										a = createText(e, m_font, {20.f+(space*i), 535.f}, 12, sf::Color::White);
										i++;
										break;
								}
								m_game->window().draw(a);
						}
						else if(e->hasComponent<CBuilding>())
						{
								auto& building = e->getComponent<CBuilding>();
								std::string buildingName;
								switch(building.type)
								{
								case Building::HOME: buildingName = "Home"; break;
								case Building::PLAZA: buildingName = "Plaza"; break;
								case Building::SCHOOL: buildingName = "School"; break;
								case Building::WORKPLACE: buildingName = "Workplace"; break;
								case Building::GOVERNMENT: buildingName = "Government"; break;
								default: buildingName = "Building"; break;
								}
								a = createText(buildingName, m_font, {20.f+(space*i), 565.f}, 12, sf::Color::White);
								m_game->window().draw(a);
								b++;
						}
				}
		}
}

void SceneMain::handleSentencing(ActionName act)
{
		std::cout << "sentencing" << std::endl;

		const auto& cursor = m_cursorEntity->getComponent<CTransform>();
		sf::Vector2f cursorPos = cursor.position;

		auto tileEntities = getTileEntities(cursorPos);

		if(!tileEntities.empty())
		{
				std::cout << "found " << tileEntities.size() << " entities "
						  << "at position (" << cursorPos.x << ", "
						  << cursorPos.y << ")." << std::endl;
		}
		else
				std::cout << "no entities found at current tile." << std::endl;

		switch(act)
		{
		case ActionName::SELECT_ENTITY:
				break;
		case ActionName::RETURN:
				m_tileEntities.clear();
				m_controlState = MapControlState::NAVIGATING;
				break;
		default:
				break;
		}
}

std::shared_ptr<Entity> SceneMain::getTileAtCursorPosition(const sf::Vector2f& pos)
{
		auto checkTile = [&](const std::shared_ptr<Entity>& entity) -> bool
				{
						if(entity->hasComponent<CTransform>())
						{
								auto& entityPos = entity->getComponent<CTransform>().position;
								return entityPos.x == pos.x && entityPos.y == pos.y;
						}
						return false;
				};

		auto& pathEnt = m_entities.getEntities("path");
		for(auto& path : pathEnt)
		{
				if(checkTile(path))
						return path;
		}

		auto& buildEnt = m_entities.getEntities("building");
		for(auto& build : buildEnt)
		{
				if(checkTile(build))
						return build;
		}
		return nullptr;
}

std::vector<std::shared_ptr<Entity>> SceneMain::getTileEntities(const sf::Vector2f& pos)
{
		std::vector<std::shared_ptr<Entity>> tileEntities;

		auto tile = getTileAtCursorPosition(pos);
		if(!tile)
		{
				return tileEntities;
		}

		if(tile->hasComponent<CPath>())
		{
				auto& pathEnt = tile->getComponent<CPath>().entities;
				tileEntities.insert(tileEntities.end(), pathEnt.begin(), pathEnt.end());
		}

		if(tile->hasComponent<CBuilding>())
		{
				tileEntities.push_back(tile);
		}

		m_tileEntities = tileEntities;
		return tileEntities;
}

void SceneMain::handleCursorNavigation(const Action& action, ActionName act, CInput& cinput)
{
		switch(action.type())
		{
		case ActionType::START:
				switch(act)
				{
				case ActionName::MOVE_UP:
						cinput.up = true;
						break;
				case ActionName::MOVE_DOWN:
						cinput.down = true;
						break;
				case ActionName::MOVE_LEFT:
						cinput.left = true;
						break;
				case ActionName::MOVE_RIGHT:
						cinput.right = true;
						break;
				case ActionName::SELECT_TILE:
						m_controlState = MapControlState::SENTENCING;
						break;
				case ActionName::OPEN_INVENTORY:
						m_game->changeScene("inventory", std::make_shared<SceneInventory>(m_game));
				default:
						break;
				}
				break;
		case ActionType::END:
				switch(act)
				{
				case ActionName::MOVE_UP:
						cinput.up = false;
						break;
				case ActionName::MOVE_DOWN:
						cinput.down = false;
						break;
				case ActionName::MOVE_LEFT:
						cinput.left = false;
						break;
				case ActionName::MOVE_RIGHT:
						cinput.right = false;
						break;
				default:
						break;
				}
				break;
		case ActionType::NONE:
				break;
		}
}

void SceneMain::renderCursor()
{
		for(auto& e : m_entities.getEntities("cursor"))
		{
				if(e->hasComponent<CTransform>() && e->hasComponent<CShape>() && e->hasComponent<CInput>())
				{
						auto& shape = e->getComponent<CShape>().shape;
						auto& transform = e->getComponent<CTransform>();
						shape->setPosition(transform.position.x, transform.position.y);
						m_game->window().draw(*shape);
				}
		}
}

void SceneMain::renderBuildings()
{
		for(auto& e : m_entities.getEntities("building"))
		{
				if(e->hasComponent<CBuilding>() && e->hasComponent<CTransform>() && e->hasComponent<CShape>())
				{
						auto& shape = e->getComponent<CShape>().shape;
						auto& transform = e->getComponent<CTransform>();
						shape->setPosition(transform.position.x, transform.position.y);
						m_game->window().draw(*shape);
				}
		}
}

void SceneMain::makeCursor()
{
		m_cursorEntity = m_entities.addEntity("cursor");
		m_cursorEntity->addComponent<CTransform>();
		auto& transform = m_cursorEntity->getComponent<CTransform>();

		const auto& buildings = m_entities.getEntities("building");
		if (!buildings.empty())
		{
				auto& bpos = buildings[0]->getComponent<CTransform>().position;
				transform.position = bpos;
		}
		else
		{
				transform.position = sf::Vector2f(100.f, 100.f);
		}

		m_cursorEntity->addComponent<CInput>();

		auto shape = std::make_shared<sf::RectangleShape>(sf::Vector2f(20.f, 20.f));
		shape->setFillColor(sf::Color::Yellow);
		m_cursorEntity->addComponent<CShape>(shape);
}

void SceneMain::makeMapView()
{
		m_mainView.setSize({800.f, 600.f});
		m_mainView.setCenter({400.f, 300.f});
		m_mainView.setViewport(sf::FloatRect({0.0f, 0.08f}, {0.75f, 2.f/3.f}));
}

void SMain::makeMapGrid()
{
		m_tileOutlineShape = m_entities.addEntity("shape");
		auto shape = std::make_shared<sf::RectangleShape>(sf::Vector2f(20.f, 20.f));
		shape->setFillColor(sf::Color::Transparent);
		shape->setOutlineThickness(1);
		shape->setOutlineColor(sf::Color(255, 255, 255, 100));
		m_tileOutlineShape->addComponent<CShape>(shape);
}

void SceneMain::updateCursorPos()
{
		auto& transform = m_cursorEntity->getComponent<CTransform>();
		auto& input = m_cursorEntity->getComponent<CInput>();

		const float tileSize = 20.f;

		if(input.up)
		{
				transform.position.y -= tileSize;
				input.up = false;
		}
		if(input.down)
		{
				transform.position.y += tileSize;
				input.down = false;
		}
		if(input.left)
		{
				transform.position.x -= tileSize;
				input.left = false;
		}
		if(input.right)
		{
				transform.position.x += tileSize;
				input.right = false;
		}
}

void SceneMain::makeBuildings()
{
		const auto& buildingCoords = m_mapManager.getBuildingCoords();

		for(size_t i = 0; i < buildingCoords.size(); i++)
		{
				std::shared_ptr<Entity> building = m_entities.addEntity("building");
				building->addComponent<CTransform>();
				auto& transform = building->getComponent<CTransform>();
				transform.position.x = static_cast<float>(buildingCoords[i].x);
				transform.position.y = static_cast<float>(buildingCoords[i].y);

				auto shape = std::make_shared<sf::RectangleShape>(sf::Vector2f(20.f, 20.f));
				shape->setFillColor(sf::Color::Red);
				building->addComponent<CShape>(shape);

				building->addComponent<CBuilding>();
				auto type = static_cast<Building>(i % (int)Building::COUNT);

				auto& buildingType = building->getComponent<CBuilding>().type;
				buildingType = type;
		}
}



void SMain::renderTileGrid()
{
		for(int y = 0; y < m_mapManager.getHeight(); y++)
		{
				for(int x = 0; x < m_mapManager.getWidth(); x++)
				{
						auto& tileShape = m_tileOutlineShape->getComponent<CShape>().shape;
						tileShape->setPosition(x*20.f, y*20.f);
						m_game->window().draw(*tileShape);
				}
		}
}

void SceneMain::makePaths()
{
		const auto& pathCoords = m_mapManager.getPathCoords();

		std::cout << "Creating " << pathCoords.size() << " path entities" << std::endl;

		for(size_t i = 0; i < pathCoords.size(); i++)
		{
				std::shared_ptr<Entity> path = m_entities.addEntity("path");
				path->addComponent<CTransform>();
				auto& transform = path->getComponent<CTransform>();
				transform.position.x = static_cast<float>(pathCoords[i].x);
				transform.position.y = static_cast<float>(pathCoords[i].y);

				auto shape = std::make_shared<sf::RectangleShape>(sf::Vector2f(20.f, 20.f));
				shape->setFillColor(sf::Color::Green);
				path->addComponent<CShape>(shape);

				path->addComponent<CPath>();
		}
}

void SceneMain::renderPaths()
{
		for(auto& e : m_entities.getEntities("path"))
		{
				if(e->hasComponent<CTransform>() && e->hasComponent<CShape>() && e->hasComponent<CPath>())
				{
						auto& shape = e->getComponent<CShape>().shape;
						auto& transform = e->getComponent<CTransform>();

						shape->setPosition(transform.position.x, transform.position.y);
						m_game->window().draw(*shape);
				}
		}
}
