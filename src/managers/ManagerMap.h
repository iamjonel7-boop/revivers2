#pragma once

#include <vector>
#include "Entity.h"
#include <SFML/Graphics.hpp>

class MapManager
{
  const float  m_tileSize = 20.f;
  const int m_mapWidth = 600;
  const int m_mapHeight = 800;
  size_t m_numBuildings = 4;

  struct Tile
  {
    std::vector<std::shared_ptr<Entity>> m_tileEntities;
    bool isClaimed;
  };

  std::vector<Tile> m_mapData;
  std::vector<sf::Vector2f> m_buildingCoords;
  std::vector<sf::Vector2f> m_pathCoords;

  std::shared_ptr<Entity> m_tile;

 public:
  MapManager();

  void createMapGrid(EntityManager& entityManager);
  void renderGrid(sf::RenderWindow& window);

  void locateBuilding();
  void locatePath();
  void printBuildingCoords();

  std::vector<sf::Vector2f> getBuildingCoords() const;
  std::vector<sf::Vector2f> getPathCoords() const;

  int getWidth() const;
  int getHeight() const ;

};
