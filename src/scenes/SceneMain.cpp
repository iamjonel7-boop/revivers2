#include "SceneMain.h"
#include <iostream>
#include <fstream>


SceneMain::SceneMain(GameEngine* gameEngine) :
   Scene(gameEngine)
{
   init();

   m_gridSize = sf::Vector2i(50, 50);
   m_cursorPos = sf::Vector2i(0, 0);
   m_cameraOffset = sf::Vector2i(0, 0);
   m_controlState = MapControlState::Navigating;
   m_selectedEntityIndex = 0;

   m_currentPhase = TurnPhase::PlayerAction;
   m_currentTurn = 1;
   m_turnInProgress = false;
   m_selectionState = SelectionState::SelectingFocus;

   sMapInit();

   sTurnStart();
}

void SceneMain::sMapInit()
{
   m_grid.resize(m_gridSize.y, std::vector<Tile>(m_gridSize.x));

   for(int y = 0; y < m_gridSize.y; y++)
   {
      for(int x = 0; x < m_gridSize.x; x++)
      {
         m_grid[y][x].gridPos = sf::Vector2i(x, y);
         m_grid[y][x].wordPos = sf::Vector2i(x*64.0f, y*64.0f);
         m_grid[y][x].type = TileType::Grass;
         m_grid[y][x].isImperialControlled = true;
         m_grid[y][x].isPlayerControlled =false;
      }
   }

   //todo: load from file
   m_grid[25][25].type = TileType::Building;
   m_grid[25][25].buildingName = "City Hall";

   for(int i = 20; i < 23; i++)
   {
      for(int j = 20; j < 23; j++)
      {
         m_grid[i][j].type = TileType::Housing;
         m_grid[i][j].buildingName = "Harong";
      }
   }

   for(int i = 5; i < 15; i++)
   {
      for(int j = 5; j < 15; j++)
      {
         m_grid[i][j].type = TileType::Forest;
      }
   }
}
