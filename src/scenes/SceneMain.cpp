#include "SceneMain.h"
#include "Components.h"
#include "GameEngine.h"
#include <iostream>
#include <fstream>

using SMain = SceneMain;

SMain::SceneMain(GameEngine* gameEngine) :
   Scene(gameEngine)
{
   init();
   createMap();

   m_controlState = MapControlState::NAVIGATING;
   std::cout << "Navigating state.";

   switch (m_controlState)
     {
     case MapControlState::NAVIGATING:
       registerAction(sf::Keyboard::W, "UP");
       registerAction(sf::Keyboard::A, "LEFT");
       registerAction(sf::Keyboard::S, "DOWN");
       registerAction(sf::Keyboard::D, "RIGHT");
       registerAction(sf::Keyboard::Space, "SELECT_TILE");
       break;
     case MapControlState::SENTENCING:
       break;
     }
}

void SMain::init()
{
  m_cursorEntity = m_entities.addEntity("cursor");
  m_cursorEntity->addComponent<CTransform>();
  m_cursorEntity->addComponent<CInput>();

  m_mainView.setSize({800.f, 400.f});
  m_mainView.setCenter({400.f, 200.f});
  m_mainView.setViewport(sf::FloatRect({0.0f, 0.0f}, {1.f, 2.f/3.f}));
}

void SMain::createMap()
{
  m_tile.setSize(sf::Vector2f(tileSize, tileSize));
  m_tile.setFillColor(sf::Color::Transparent);
  m_tile.setOutlineThickness(1);
}

void SMain::sDoAction(const Action& action)
{
  if(action.type() == "START")
    {
      if(action.name() == "UP")
        {
          std::cout << "Move up.\n";
          //set cinput values to true for the cursor entity
        }
    }

}

void SMain::update()
{

}

void SMain::onEnd()
{
}

void SMain::sRender()
{
  m_game->window().setView(m_mainView);

  for(int y = 0; y < mapHeight; ++y)
    {
      for(int x = 0; x < mapWidth; ++x)
        {
          //int id = tileMap[y*mapWidth+x];
          m_tile.setOutlineColor(sf::Color(255, 255, 255, 100));
          m_tile.setPosition(x*tileSize, y*tileSize);
          m_game->window().draw(m_tile);
        }
    }
}
