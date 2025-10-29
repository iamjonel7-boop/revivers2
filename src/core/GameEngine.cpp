#include "GameEngine.h"
#include "SceneMenu.h"
#include <SFML/Graphics.hpp>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>

GameEngine::GameEngine()
{
   std::cout << "Game engine running.\n";

   m_window.create(sf::VideoMode(1280, 720), "Revivers");

   std::cout << "Window created.\n";

   init();

   changeScene("menu", std::make_shared<SceneMenu>(this));
}

void GameEngine::init()
{


   //grammr first
   m_grammarManager = std::make_unique<GrammarManager>();

   //lexicon depends on grammar for language specific paths
   std::string currentLanguage = m_grammarManager->getCurrentLanguageName();

   std::cout << "Native language set to " << currentLanguage << std::endl;

   std::string lexiconPath = "data/lexicon/" + currentLanguage + "_lexicon.json";

   //TODO: create file is does not exist
   std::string playerDictPath = "data/lexicon/" + currentLanguage + "_dictionary.json";

   m_lexiconManager = std::make_unique<LexiconManager>(lexiconPath, playerDictPath);

   std::cout << "Lexicon and player dictionary are loaded.\n";
}

//wont be using this for now, but this needs to be updated
//GameEngine::changeLanguage(const std::string &language)
//{
//   m_grammarManager->setLanguage(language);
//
//   init();
//}

GameEngine::GameEngine(int width, int height, const std::string& title)
   : m_window(sf::VideoMode(width, height), title)
{
   changeScene("menu", std::make_shared<SceneMenu>(this));
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

   while(m_window.pollEvent(event))
   {
      if(event.type == sf::Event::Closed)
      {
         quit();
      }

      //text input
      if(event.type == sf::Event::TextEntered && scene->isTyping())
      {
         if(event.text.unicode >= 32 && event.text.unicode <= 126)
            scene->appendInputChar(static_cast<char>(event.text.unicode));
         else if(event.text.unicode == 8)
            scene->removeLastInputChar();
      }

      if(event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Enter)
      {
         if(scene->isTyping())
         {
            scene->commitInput();
            scene->stopTyping();
         }
         else
            scene->startTyping();
      }

      if(event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)
      {
         if(scene->isTyping())
         {
            scene->stopTyping();
            scene->inputClear();
            continue;
         }
      }

      //mapped actions
      if(event.type == sf::Event::KeyPressed || event.type == sf::Event::KeyReleased)
      {
         if(actionMap.find(event.key.code) == actionMap.end())
            continue;

         const std::string actionType = (event.type == sf::Event::KeyPressed) ? "START": "END";
         auto actionName = actionMap.at(event.key.code);
         scene->doAction(Action(actionName, actionType));
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

   sf::Clock clock;
   //pause functionality here
   //systems that run when paused (rendering)
   //systems shouldnt (movement/input)

   while(isRunning())
   {
      m_deltaTime = clock.restart().asSeconds();

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
