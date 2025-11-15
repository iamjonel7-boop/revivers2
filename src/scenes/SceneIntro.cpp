#include "SceneIntro.h"
#include "Components.h"
#include "GameEngine.h"
#include <iostream>
#include <fstream>

using SIntro = SceneIntro;

SIntro::SceneIntro(GameEngine* gameEngine) :
  Scene(gameEngine)
{
  init();

  registerAction(sf::Keyboard::Enter, "ENTER");
}

void SIntro::init()
{
  m_player = m_entities.addEntity("player");
  m_player->addComponent<CTransform>();

  //setup elder dialogue. redo to read from file
  m_elderWords = {
    "Maray ta nagbalik ka.",
    "Maluyahon ako"
  };
  m_elderFont.loadFromFile("/usr/local/share/fonts/Liberation/LiberationMono-Regular.ttf");
  m_elderDialogue = m_entities.addEntity("text");
  m_elderDialogue->addComponent<CText>(m_elderFont, m_elderWords[m_currentDialogue], 24, sf::Vector2f(200.f, 200.f), sf::Color::White);
}

void SIntro::update()
{
  m_entities.update();

  if(m_currentDialogue < m_elderWords.size())
    {
      auto& dialogue = m_elderDialogue->getComponent<CText>().text;
      dialogue.setString(m_elderWords[m_currentDialogue]);
    }
}

void SIntro::sDoAction(const Action& action)
{
  if(action.type() == "START")
    {
      if(m_currentDialogue < m_elderWords.size())
        {
          if(action.name() == "ENTER")
            {
              std::cout << m_elderWords.size();
              m_currentDialogue++;
            }
        }
    }
  else if(action.type() == "END")
    {
    }
}

void SIntro::onEnd()
{
}

void SIntro::sRender()
{
  auto& window = m_game->window();

  for(auto& e : m_entities.getEntities("text"))
    {
      if(e->hasComponent<CText>())
        window.draw(e->getComponent<CText>().text);
    }
}
