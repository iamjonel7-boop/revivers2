#pragma once

#include <vector>
#include "Entity.h"
#include <SFML/Graphics.hpp>

class MapManager
{
  const float  m_tileSize = 20.f;
  const int m_mapWidth = 600;
  const int m_mapHeight = 800;

  struct Tile
  {
    std::vector<std::shared_ptr<Entity>> m_tileEntities;
    bool isClaimed;
  };

  std::vector<Tile> mapData;

  std::shared_ptr<Entity> m_tile;

 public:
  MapManager();

  void createMapGrid(EntityManager& entityManager);
  void renderGrid(sf::RenderWindow& window);
};
