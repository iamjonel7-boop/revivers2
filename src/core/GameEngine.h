#pragma once

#include "Scene.h"
#include "ManagerGrammar.h"
#include "ManagerLexicon.h"
#include <SFML/Graphics.hpp>
#include <memory>
#include <string>
#include <map>

class Scene;

typedef std::map<std::string, std::shared_ptr<Scene>> SceneMap;

class GameEngine
{
   //core managers
   std::unique_ptr<GrammarManager> m_grammarManager;
   std::unique_ptr<LexiconManager> m_lexiconManager;

   sf::RenderWindow m_window;
   std::string m_currentScene;
   SceneMap m_sceneMap;

   bool m_deltaTime = 0.0f;

   size_t m_simulationSpeed = 1;
   bool m_running = true;

   void init(); //config
   void update();
   void sUserInput();

   std::shared_ptr<Scene> currentScene();

public:
   GameEngine();
   GameEngine(int width, int height, const std::string& title);

   void changeScene(const std::string& sceneName,
   std::shared_ptr<Scene> scene,
   bool endCurrentScene = false);

   GrammarManager& getGrammarManager()
   {
      return *m_grammarManager;
   }

   LexiconManager& getLexiconManager()
   {
      return *m_lexiconManager;
   }

   void changeLanguage(const std::string& language);

   void quit();
   void run();
   void sRender();

   sf::RenderWindow& window();
   bool isRunning();

   float getDeltaTime() const { return m_deltaTime; }

   size_t windowWidth() const
   {
      return m_window.getSize().x;
   }
   size_t windowHeight() const
   {
      return m_window.getSize().y;
   }
};
