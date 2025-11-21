#pragma once

#include <vector>
#include "Entity.h"
#include <SFML/Graphics.hpp>

class MapManager
{
  const float  m_tileSize = 20.f;
  const int m_mapWidth = 40;
  const int m_mapHeight = 30;
  size_t m_numBuildings = 5;

  struct Tile
  {
    std::vector<std::shared_ptr<Entity>> m_tileEntities;
    bool isClaimed;
  };

  std::vector<Tile> m_mapData;
  std::vector<sf::Vector2u> m_buildingCoords;
  std::vector<sf::Vector2u> m_pathCoords;

  std::shared_ptr<Entity> m_tile;

 public:
  MapManager();

  void locateBuildings();
  void locatePaths();
  void printBuildingCoords();
  void printPathCoords();

  std::vector<sf::Vector2u> getBuildingCoords() const;
  std::vector<sf::Vector2u> getPathCoords() const;

  int getWidth() const;
  int getHeight() const ;

};
