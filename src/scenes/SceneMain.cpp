
#include "SceneMain.h"
#include "Components.h"
#include "GameEngine.h"
#include "ManagerMap.h"
#include <iostream>
#include <fstream>

using SMain = SceneMain;

SMain::SceneMain(GameEngine* gameEngine) :
  Scene(gameEngine)
{
  init();
  showMapGrid();

  m_controlState = MapControlState::NAVIGATING;
  std::cout << "Navigating state.";

  registerAction(sf::Keyboard::W, static_cast<int>(ActionName::MOVE_UP));
  registerAction(sf::Keyboard::A, static_cast<int>(ActionName::MOVE_LEFT));
  registerAction(sf::Keyboard::S, static_cast<int>(ActionName::MOVE_DOWN));
  registerAction(sf::Keyboard::D, static_cast<int>(ActionName::MOVE_RIGHT));
  registerAction(sf::Keyboard::Space, static_cast<int>(ActionName::SELECT_TILE));
}

void SMain::init()
{

  auto  m_player = m_game->getWorldManager()->getPlayer();

  // DEBUG: Check if same player exists
  std::cout << "=== SCENE MAIN ===" << std::endl;
  if (m_player) {
    std::cout << "Player memory address: " << m_player.get() << std::endl;
    std::cout << "Player name: " << m_player->getComponent<CProfile>().playerName << std::endl;
    std::cout << "Has CTransform: " << m_player->hasComponent<CTransform>() << std::endl;
    std::cout << "Has CProfile: " << m_player->hasComponent<CProfile>() << std::endl;
  } else {
    std::cout << "ERROR: Player is null!" << std::endl;
  }

  // cursor entity
  m_cursorEntity = m_entities.addEntity("cursor");

  m_cursorEntity->addComponent<CTransform>();
  auto& transform = m_cursorEntity->getComponent<CTransform>();
  transform.position = sf::Vector2f(100.f, 100.f);

  m_cursorEntity->addComponent<CInput>();

  auto shape = std::make_shared<sf::RectangleShape>(sf::Vector2f(20.f, 20.f));
  shape->setFillColor(sf::Color::Yellow);
  m_cursorEntity->addComponent<CShape>(shape);

  // camera view
  m_mainView.setSize({800.f, 400.f});
  m_mainView.setCenter({400.f, 200.f});
  m_mainView.setViewport(sf::FloatRect({0.0f, 0.05f}, {1.f, 2.f/3.f}));
}

void SMain::showMapGrid()
{
  m_mapManager.createMapGrid(m_entities);
  /*
  m_tile = m_entities.addEntity("shape");
  auto shape = std::make_shared<sf::RectangleShape>(sf::Vector2f(20.f, 20.f));
  shape->setFillColor(sf::Color::Transparent);
  shape->setOutlineThickness(1);
  shape->setOutlineColor(sf::Color(255, 255, 255, 100));
  m_tile->addComponent<CShape>(shape);
  */
}

void SMain::sDoAction(const Action& action)
{
  ActionName act = static_cast<ActionName>(action.name());
  auto& cinput = m_cursorEntity->getComponent<CInput>();
  switch(m_controlState)
    {
    case MapControlState::NAVIGATING:
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
      break;
    case MapControlState::SENTENCING:
      switch(action.type())
        {
        case ActionType::START:
        case ActionType::END:
        case ActionType::NONE:
          break;
        }
    }
  /*
  if(m_controlState == MapControlState::NAVIGATING)
    {
      auto& cinput = m_cursorEntity->getComponent<CInput>();

      if(action.type() == "START")
        {
          if(action.name() == "UP")
            cinput.up = true;
          else if(action.name() == "DOWN")
            cinput.down = true;
          else if(action.name() == "LEFT")
            cinput.left = true;
          else if(action.name() == "RIGHT")
            cinput.right = true;

          else if(action.name() == "SELECT_TILE")
            {
              m_controlState = MapControlState::SENTENCING;
              std::cout << "Sentencing state.\n";
            }
        }
      else if(action.type() == "END")
        {
          if(action.name() == "UP")
            cinput.up = false;
          else if(action.name() == "DOWN")
            cinput.down = false;
          else if(action.name() == "LEFT")
            cinput.left = false;
          else if(action.name() == "RIGHT")
            cinput.right = false;
        }
    }
  else if(m_controlState == MapControlState::SENTENCING)
    {
      if(action.type() == "START")
        {
          if(action.name() == "SELECT_TILE")
            {
              m_controlState = MapControlState::NAVIGATING;
              std::cout << "Navigating state.\n";
            }
        }
    }
  */
}

void SMain::update()
{
  m_entities.update();

  //move cursor
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

void SMain::onEnd()
{
}

void SMain::sRender()
{
  m_game->window().setView(m_mainView);

  m_mapManager.renderGrid(m_game->window());

  /*
  for(int y = 0; y < 600; ++y)
    {
      for(int x = 0; x < 800; ++x)
        {
          auto& tileShape = m_tile->getComponent<CShape>().shape;
          tileShape->setPosition(x*20.f, y*20.f);
          m_game->window().draw(*tileShape);
        }
    }
  */

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
}
