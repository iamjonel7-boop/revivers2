
#include "SceneMain.h"
#include "Components.h"
#include "GameEngine.h"
#include "ManagerMap.h"
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
		registerAction(sf::Keyboard::Space, static_cast<int>(ActionName::SELECT_TILE));
		registerAction(sf::Keyboard::Tab, static_cast<int>(ActionName::OPEN_INVENTORY));
		registerAction(sf::Keyboard::E, static_cast<int>(ActionName::SELECT_ENTITY));
		registerAction(sf::Keyboard::Backspace, static_cast<int>(ActionName::RETURN));
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

		std::string help1, help2;

		const auto& func = m_updateTable[static_cast<size_t>(m_controlState)];
		if(func != nullptr) (this->*func)(help1, help2);

		m_helpMsg.setString(help1);
		m_helpMsg2.setString(help2);
}

void SceneMain::updateSentencing(std::string& help1, std::string& help2)
{
		help1 = "Press Backspace to return.";
		help2 = "";
}

void SceneMain::updateNavigating(std::string& help1, std::string& help2)
{
		help1 = "Use the arrow keys to navigate\nthe map.";
		help2 = "Press Space to select a tile.";
		updateCursorPos();
}
void SceneMain::createPopulation()
{
		auto& pathEntities = m_entities.getEntities("path");

		std::random_device rd;
		std::mt19937 gen(rd());

		std::cout << "[POPULATION] Spawning "
				  << WorldManager::m_nativeEthnicity
				  << " civilians..." << std::endl;

		if (pathEntities.empty())
		{
				std::cout << "[POPULATION] ERROR: No path tiles exist!" << std::endl;
				return;
		}

		for(int i = 0; i < WorldManager::m_nativeEthnicity; i++)
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

		for(int i = 0; i < WorldManager::m_imperialEthnicity; i++)
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

void SMain::onEnd()
{
}

void SMain::sRender()
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
}

void SceneMain::handleSentencing()
{
void SceneMain::getTileAtCursorPosition(sf::Vector2f pos)
{
		const float tileSize = 20.f;
		int tileX = static_cast<int>(pos.x/tileSize);
		int tileY = static_cast<int>(pos.y/tileSize);

		auto& pathEnt = m_entities.getEntities("path");
		for(auto& path : pathEnt)
		{
				if(path->getComponent<CTransform>())
				{
						auto& pathPos = path->getComponent<CTransform>.position;
						int pathXpos = static_cast<int>(pathPos.x/tileSize);
						int pathYpos = static_cast<int>(pathPos.y/tileSize);

						if(tileX == pathXpos && tileY == pathYpos)
								return path;
				}
		}

		auto& buildEnt = m_entities.getEntities("building");
		for(auto& build : buildEnt)
		{
				if(build->getComponent<CTransform>())
				{
						auto& buildPos = path->getComponent<CTransform>.position;
						int buildXpos = static_cast<int>(buildPos.x/tileSize);
						int buildYpos = static_cast<int>(buildPos.y/tileSize);

						if(tileX == buildXpos && tileY == buildYpos)
								return build;
				}
		}
		return nullptr;
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
  transform.position = sf::Vector2f(100.f, 100.f);

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
