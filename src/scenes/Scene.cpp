#include "Scene.h"
#include "GameEngine.h"
#include <SFML/Graphics.hpp>
#include <iostream>

Scene::Scene()
   : m_game(nullptr)
   , m_hasEnded(false)
   , m_currentFrame(0)
{
}

Scene::Scene(GameEngine* gameEngine)
   : m_game(gameEngine)
   , m_hasEnded(false)
   , m_currentFrame(0)
{
}

void Scene::doAction(const Action& action)
{
   sDoAction(action);
}

void Scene::simulate(const size_t frames)
{
   for(size_t i = 0; i < frames && !m_hasEnded; i++)
   {
      update();
      m_currentFrame++;
   }
}

void Scene::registerAction(int inputKey, int actionName)
{
   m_actionMap[inputKey] = actionName;
}

size_t Scene::currentFrame() const
{
   return m_currentFrame;
}

bool Scene::hasEnded() const
{
   return m_hasEnded;
}

const ActionMap& Scene::getActionMap() const
{
   return m_actionMap;
}

TextInputManager& Scene::getTextInputManager()
{
  return m_textInputManager;
}

MapManager& Scene::getMapManager()
{
  return m_mapManager;
}

LexiconManager& Scene::getLexiconManager()
{
  return m_lexiconManager;
}

sf::Text Scene::createText(const std::string& content, const sf::Font& font, sf::Vector2f pos, float charSize, sf::Color color) const
{
  sf::Text text;
  text.setString(content);
  text.setFont(font);
  text.setPosition(pos);
  text.setCharacterSize(charSize);
  text.setFillColor(color);
  return text;
}

sf::RectangleShape Scene::createBox(sf::Vector2f pos, sf::Vector2f size, sf::Color color) const
{
  sf::RectangleShape box;
  box.setPosition(pos);
  box.setSize(size);
  box.setFillColor(color);
  return box;
}

int Scene::wrapIndex(int current, int delta, int size)
{
		return (current + delta + size) % size;
}
