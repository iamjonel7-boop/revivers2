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
  m_state = State::INTRO_DIALOGUE;
  m_currentLine = 0;

  registerAction(sf::Keyboard::D, static_cast<int>(ActionName::NEXT));
  registerAction(sf::Keyboard::A, static_cast<int>(ActionName::BACK));
}

void SIntro::init()
{
  m_player = m_entities.addEntity("player");
  m_player->addComponent<CTransform>();

  m_font.loadFromFile("/usr/local/share/fonts/Liberation/LiberationMono-Regular.ttf");

  // redo to read from file
  m_introLines = {
    "Itaram mo ang pangaran mo",
    "Feeling ko ang pangit ng code ko"
  };
  m_introLine = m_entities.addEntity("textIntro");
  m_introLine->addComponent<CText>(m_font,
                                   m_introLines[m_currentLine],
                                   24,
                                   sf::Vector2f(200.f, 200.f),
                                   sf::Color::White);

  m_elderLines = {
    "Maray ta nagbalik ka.",
    "Maluyahon ako"
  };
  m_elderLine = m_entities.addEntity("textElder");
  m_elderLine->addComponent<CText>(m_font,
                                   m_elderLines[m_currentLine],
                                   24,
                                   sf::Vector2f(200.f, 200.f),
                                   sf::Color::White);

  //input name part

}

void SIntro::update()
{
  m_entities.update();

  /*
  if (!m_introLine) { std::cerr << "m_introLine is null\n"; return; }
  if (!m_elderLine) { std::cerr << "m_elderLine is null\n"; } // may be created later

  if (!m_introLine->hasComponent<CText>()) {
    std::cerr << "m_introLine missing CText\n";
    return;
  }
  if (!m_elderLine->hasComponent<CText>()) {
    std::cerr << "m_elderLine missing CText\n";
  }

  std::cerr << "m_currentLine=" << m_currentLine
            << " introLines=" << m_introLines.size()
            << " elderLines=" << m_elderLines.size() << "\n";
  */

  switch(m_state)
    {
    case State::INTRO_DIALOGUE:
      if(m_currentLine < m_introLines.size())
        {
          auto& t = m_introLine->getComponent<CText>().text;
          t.setString(m_introLines[m_currentLine]);
        }
      break;
    case State::ELDER_DIALOGUE:
      if(m_currentLine < m_elderLines.size())
        {
          auto& t = m_elderLine->getComponent<CText>().text;
          t.setString(m_elderLines[m_currentLine]);
        }
      break;
    case State::NAME_INPUT:
      break;
    }
}

void SIntro::sDoAction(const Action& action) //maybe we can use switch here. it is prettier
{
  ActionName act = static_cast<ActionName>(action.name());

  switch(m_state)
    {
    case State::INTRO_DIALOGUE:
      switch(action.type())
        {
        case ActionType::START:
          switch(act)
            {
            case ActionName::NEXT:
              if(m_currentLine < m_introLines.size()-1)
                {
                  m_currentLine++;
                }
              else if(m_currentLine == m_introLines.size()-1)
                {
                  std::cout << "currentline equal to elderlines" << std::endl;
                  m_currentLine = 0;
                  m_state = State::ELDER_DIALOGUE;
                }
              break;
            case ActionName::BACK:
              if(m_currentLine > 0)
                {
                  m_currentLine--;
                }
              break;
            }
          break;
        default:
          break;
        }
      break;

    case State::ELDER_DIALOGUE:
      switch(action.type())
        {
        case ActionType::START:
          switch(act)
            {
            case ActionName::NEXT:
              if(m_currentLine < m_elderLines.size()-1)
                {
                  m_currentLine++;
                }
              else if(m_currentLine == m_elderLines.size()-1)
                {
                  std::cout << "end of elder dialogue." << std::endl;
                }
              break;
            default:
              break;
            }
          break;
        default:
          break;
        }
      break;

    case State::NAME_INPUT:
      switch(action.type())
        {
        case ActionType::START:
          switch(act)
            {
            case ActionName::NEXT:
              m_textInputManager.start();
              std::cout << m_textInputManager.getText() << std::endl;
              break;
            default:
              break;
            }
        default:
          break;
        }
      break;
    }
}

void SIntro::onEnd()
{
}

void SIntro::sRender()
{
  auto& window = m_game->window();

  switch(m_state)
    {
    case State::INTRO_DIALOGUE:
      for(auto& e : m_entities.getEntities("textIntro"))
        {
          if(e->hasComponent<CText>())
            window.draw(e->getComponent<CText>().text);
        }
      break;

    case State::ELDER_DIALOGUE:
      for(auto& e : m_entities.getEntities("textElder"))
        {
          if(e->hasComponent<CText>())
            window.draw(e->getComponent<CText>().text);
        }
      break;

    default:
      break;
    }
}
