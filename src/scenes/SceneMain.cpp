#include "SceneMain.h"
#include "Components.h"
#include <iostream>
#include <fstream>

using SMain = SceneMain;

SMain::SceneMain(GameEngine* gameEngine) :
   Scene(gameEngine)
{
   init();

   m_controlState = MapControlState::NAVIGATING;

   switch (MapControlState)
     {
     case NAVIGATING:
       registerAction(sf::Keyboard::W, "UP");
       registerAction(sf::Keyboard::A, "LEFT");
       registerAction(sf::Keyboard::S, "DOWN");
       registerAction(sf::Keyboard::D, "RIGHT");
       registerAction(sf::Keyboard::Space, "SELECT_TILE")
       break;
     case SENTENCING:
       break;
     }
}

void SMain::init()
{
  m_cursorEntity = m_entities.addEntity("cursor");
  m_cursorEntity->addComponent<CTransform>();
  m_cursorEntity->addComponent<CInput>();
}

void SMain::createMap()
{


}

void SMain::sDoAction(const Action& ation)
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
