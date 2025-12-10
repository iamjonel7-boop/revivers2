#include "ManagerMap.h"
#include "Components.h"
#include "ManagerEntity.h"
#include <iostream>
#include <algorithm>
#include <random>

MapManager::MapManager()
{
}

void  MapManager::locateBuildings()
{
		std::random_device rd;
		std::mt19937 rng(rd());

		std::vector<unsigned int> xs;
		while(xs.size() < m_numBuildings)
		{
				unsigned int x = (rng() % (int)m_mapWidth) * m_tileSize;
				if(std::find(xs.begin(), xs.end(), x) == xs.end())
				{
						xs.push_back(x);
				}
		}

		std::vector<unsigned int> ys;
		while(ys.size() < m_numBuildings)
		{
				unsigned int y = (rng() % (int)m_mapHeight) * m_tileSize;
				if(std::find(ys.begin(), ys.end(), y) == ys.end())
				{
						ys.push_back(y);
				}
		}

		std::shuffle(ys.begin(), ys.end(), rng);

		for(size_t i = 0; i < m_numBuildings; i++)
		{
				m_buildingCoords.emplace_back(xs[i], ys[i]);
		}
}

void MapManager::printBuildingCoords()
{
		std::cout << "Building coordinates:" << std::endl;

		for(const auto& p : m_buildingCoords)
		{
				std::cout << "(" << p.x << "," << p.y << ") ";
		}
		std::cout << std::endl;
}

void MapManager::printPathCoords()
{
		std::cout << "Path coordinates:" << std::endl;
		for(const auto& p : m_pathCoords)
		{
				std::cout << "(" << p.x << "," << p.y << ") ";
		}
		std::cout << std::endl;
}

void MapManager::locatePaths()
{
		for(size_t i = 0; i < m_buildingCoords.size()-1; i++)
		{
				unsigned int x1 = m_buildingCoords[i].x;
				unsigned int y1 = m_buildingCoords[i].y;
				unsigned int x2 = m_buildingCoords[i+1].x;
				unsigned int y2 = m_buildingCoords[i+1].y;

				unsigned int xDiff = std::abs(static_cast<int>(x2) - static_cast<int>(x1)) / m_tileSize;
				unsigned int yDiff = std::abs(static_cast<int>(y2) - static_cast<int>(y1)) / m_tileSize;

				// horizontal movement
				if (x1 < x2) {
						for (unsigned int step = 1; step <= xDiff; step++) {
								unsigned int newX = x1 + (step * m_tileSize);
								m_pathCoords.emplace_back(newX, y1);
						}
				} else if (x1 > x2) {
						for (unsigned int step = 1; step <= xDiff; step++) {
								unsigned int newX = x1 - (step * m_tileSize);
								m_pathCoords.emplace_back(newX, y1);
						}
				}

				// then vertical
				unsigned int currentX = x2; // After horizontal movement, we're at tile X

				if (y1 < y2) {
						for (unsigned int step = 1; step <= yDiff; step++) {
								unsigned int newY = y1 + (step * m_tileSize);
								m_pathCoords.emplace_back(currentX, newY);
						}
				} else if (y1 > y2) {
						for (unsigned int step = 1; step <= yDiff; step++) {
								unsigned int newY = y1 - (step * m_tileSize);
								m_pathCoords.emplace_back(currentX, newY);
						}
				}
		}
}

std::vector<sf::Vector2u> MapManager::getBuildingCoords() const
{
		return m_buildingCoords;
}

std::vector<sf::Vector2u> MapManager::getPathCoords() const
{
		return m_pathCoords;
}

int MapManager::getWidth() const
{
		return m_mapWidth;
}

int MapManager::getHeight() const
{
		return m_mapHeight;
}

void MapManager::makeBuildings(EntityManager& entityMgr)
{
		for(size_t i = 0; i < m_buildingCoords.size(); i++)
		{
				std::shared_ptr<Entity> building = entityMgr.addEntity("building");
				building->addComponent<CTransform>();
				auto& transform = building->getComponent<CTransform>();
				transform.position.x = static_cast<float>(m_buildingCoords[i].x);
				transform.position.y = static_cast<float>(m_buildingCoords[i].y);

				auto shape = std::make_shared<sf::RectangleShape>(sf::Vector2f(m_tileSize, m_tileSize));
				shape->setFillColor(sf::Color::Red);
				building->addComponent<CShape>(shape);

				building->addComponent<CBuilding>();
				auto type = static_cast<Building>(i % (int)Building::COUNT);
				auto& buildingType = building->getComponent<CBuilding>().type;
				buildingType = type;
		}
}

void MapManager::makePaths(EntityManager& entityMgr)
{
		for(size_t i = 0; i < m_pathCoords.size(); i++)
		{
				std::shared_ptr<Entity> path = entityMgr.addEntity("path");
				path->addComponent<CTransform>();
				auto& transform = path->getComponent<CTransform>();
				transform.position.x = static_cast<float>(m_pathCoords[i].x);
				transform.position.y = static_cast<float>(m_pathCoords[i].y);

				auto shape = std::make_shared<sf::RectangleShape>(sf::Vector2f(20.f, 20.f));
				shape->setFillColor(sf::Color::Green);
				path->addComponent<CShape>(shape);

				path->addComponent<CPath>();
		}
}

void MapManager::makeMapView(sf::View& view)
{
		view.setSize({800.f, 600.f});
		view.setCenter({400.f, 300.f});
		view.setViewport(sf::FloatRect({0.0f, 0.08f}, {0.75f, 2.f/3.f}));
}

void MapManager::makeMapGrid(EntityManager& entityMgr, std::shared_ptr<Entity>& outlineShape)
{
		outlineShape = entityMgr.addEntity("shape");
		auto shape = std::make_shared<sf::RectangleShape>(sf::Vector2f(m_tileSize, m_tileSize));
		shape->setFillColor(sf::Color::Transparent);
		shape->setOutlineThickness(1);
		shape->setOutlineColor(sf::Color(255, 255, 255, 100));
		outlineShape->addComponent<CShape>(shape);
}
