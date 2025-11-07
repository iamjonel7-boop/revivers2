#include "GameEngine.h"
//#include "SceneMain.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

GameEngine::GameEngine()
{
  init();
  changeScene("main", std::make_shared<dummyScene>(this));
}

void GameEngine::init()
{
  std::cout << "Game engine running.\n";
  m_window.create(sf::VideoMode(WIDTH, HEIGHT), "Revivers");
  std::cout << "Window created.\n";
}

void GameEngine::update()
{
  if(auto scene = currentScene())
    {
      scene->update();
    }
}

void GameEngine::sUserInput()
{
  sf::Event event;
  auto scene = currentScene();
  auto& actionMap = scene->getActionMap();
  auto& textInputManager = scene->getTextInputManager();

  while(m_window.pollEvent(event))
    {
      switch(event.type)
        {
        case sf::Event::Closed:
          quit();
          break;

        case sf::Event::KeyPressed:
        case sf::Event::KeyReleased:
          {
            if(actionMap.find(event.key.code) == actionMap.end())
              continue;

            const std::string actionType = (event.type == sf::Event::KeyPressed) ? "START" : "END";
            auto actionName = actionMap.at(event.key.code);
            scene->doAction(Action(actionName, actionType));
            break;
          }

        case sf::Event::TextEntered:
          if(event.text.unicode < 128)
            {
              textInputManager.addCharacter(static_cast<char>(event.text));
            }
          break;

        default:
          break;
        }
    }
}

void GameEngine::sRender()
{
  auto scene = currentScene();
  if(scene)
    scene->sRender();
}

void GameEngine::changeScene(const std::string& sceneName, std::shared_ptr<Scene> scene, bool endCurrentScene)
{
  if(endCurrentScene && m_sceneMap.count(m_currentScene))
    {
      m_sceneMap.erase(m_currentScene);
    }

  m_sceneMap[sceneName] = scene;
  m_currentScene = sceneName;

  std::cout << "Changing scene to " << m_currentScene << std::endl;
}

void GameEngine::run()
{
  std::cout << "Game engine running.\n";

  while(isRunning())
    {
      sUserInput();
      update();
      sRender();
    }
}

void GameEngine::quit()
{
  m_running = false;
  m_window.close();
}

sf::RenderWindow& GameEngine::window()
{
  return m_window;
}

std::shared_ptr<Scene> GameEngine::currentScene()
{
  auto it = m_sceneMap.find(m_currentScene);
  if (it != m_sceneMap.end()) {
    return it->second;
  }
  return nullptr;
}


bool GameEngine::isRunning()
{
  return m_running;
}
