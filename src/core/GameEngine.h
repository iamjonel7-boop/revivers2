#pragma once

#include <SFML/Graphics.hpp>
#include <memory>
#include <string>
#include <map>
#include "ManagerWorld.h"

class Scene;
class WorldManager;

typedef std::map<std::string, std::shared_ptr<Scene>> SceneMap;

class GameEngine
{
		sf::Clock m_clock;
		float m_deltaTime;

		sf::RenderWindow m_window;

		std::string m_currentScene;
		SceneMap m_sceneMap;

		bool m_running = true;

		void init();
		void update();
		void sUserInput();

		std::shared_ptr<Scene> currentScene();
		std::unique_ptr<WorldManager> m_world;

public:
		GameEngine();
		WorldManager* getWorldManager();
		const WorldManager* getWorldManager() const;

		void changeScene(const std::string& sceneName,
						 std::shared_ptr<Scene> scene,
						 bool endCurrentScene = false);

		void quit();
		void run();
		void sRender();

		float getDeltaTime() const
				{
						return m_deltaTime;
				}

		sf::RenderWindow& window();
		bool isRunning();

		size_t windowWidth() const
				{
						return m_window.getSize().x;
				}
		size_t windowHeight() const
				{
						return m_window.getSize().y;
				}
};
