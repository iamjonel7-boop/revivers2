#include "SceneIntro.h"
#include "Components.h"
#include "GameEngine.h"
#include <iostream>
#include <fstream>
#include "SceneMain.h"

SceneIntro::SceneIntro(GameEngine* gameEngine) :
  Scene(gameEngine)
{
  init();
  m_state = State::INTRO_DIALOGUE;

  registerAction(sf::Keyboard::D, static_cast<int>(ActionName::NEXT));
  registerAction(sf::Keyboard::A, static_cast<int>(ActionName::BACK));
  registerAction(sf::Keyboard::Enter, static_cast<int>(ActionName::ENTER));
}

void SceneIntro::init()
{
  m_game->getWorldManager()->m_createPlayer(m_entities);
  m_player = m_game->getWorldManager()->getPlayer();

  m_currentLine = 0;

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

}

void SceneIntro::update()
{
  m_entities.update();

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
      auto& t = m_player->getComponent<CProfile>().playerName;
      t = getTextInputManager().getText();
      m_playerName.setString(t);
      break;
    }
}

void SceneIntro::handleIntroDialogue(const Action& action, ActionName actionName)
{
  switch(action.type())
    {
    case ActionType::START:
      switch(actionName)
        {
        case ActionName::NEXT:
          if(m_currentLine < m_introLines.size()-1)
            {
              m_currentLine++;
            }
          else if(m_currentLine == m_introLines.size()-1)
            {
              std::cout << "===  End of intro dialogue. ===" << std::endl;
              m_currentLine = 0;
              m_state = State::ELDER_DIALOGUE;
              std::cout << "=== Start elder dialogue. ===" << std::endl;
            }
          break;
        case ActionName::BACK:
          if(m_currentLine > 0)
            {
              m_currentLine--;
            }
          break;

        default:
          break;
        }

    default:
      break;
    }
}

void SceneIntro::handleElderDialogue(const Action& action, ActionName actionName)
{
  switch(action.type())
    {
    case ActionType::START:
      switch(actionName)
        {
        case ActionName::NEXT:
          if(m_currentLine < m_elderLines.size()-1)
            {
              m_currentLine++;
            }
          else if(m_currentLine == m_elderLines.size()-1)
            {
              std::cout << "=== End of elder dialogue. ===" << std::endl;
              m_state = State::NAME_INPUT;
              std::cout << "=== Start name_input state. ===" << std::endl;
              getTextInputManager().start();
            }
          break;

        default:
          break;
        }

    default:
      break;
    }
}

void SceneIntro::handleNameInput(const Action& action, ActionName actionName, std::string& playerName)
{
  switch(action.type())
    {
    case ActionType::START:
      switch(actionName)
        {
        case ActionName::ENTER:
          playerName = getTextInputManager().getText();
          m_player->getComponent<CProfile>().playerName = playerName;

          m_game->changeScene("main", std::make_shared<SceneMain>(m_game));
          std::cout << "Scene changed to main/map." << std::endl;
          break;

        default:
          break;
        }

    default:
      break;
    }
}

void SceneIntro::sDoAction(const Action& action)
{
  ActionName actionName = static_cast<ActionName>(action.name());
  std::string playerName;

  switch(m_state)
    {
    case State::INTRO_DIALOGUE:
      handleIntroDialogue(action, actionName);
      break;

    case State::ELDER_DIALOGUE:
      handleElderDialogue(action, actionName);
      break;

    case State::NAME_INPUT:
      handleNameInput(action, actionName, playerName);
      break;
    }
}

void SceneIntro::onEnd()
{
}

void SceneIntro::renderText(const std::string& entityTag)
{
  for(auto& e : m_entities.getEntities(entityTag))
    {
      if(e->hasComponent<CText>())
        m_game->window().draw(e->getComponent<CText>().text);
    }
}

void SceneIntro::renderPlayerName()
{
  m_playerName.setFont(m_font);
  m_playerName.setCharacterSize(25);
  m_playerName.setFillColor(sf::Color::Red);
  m_playerName.setPosition(200.f, 200.f);

  for(auto& e : m_entities.getEntities("player"))
    {
      if(e->hasComponent<CProfile>())
        {
          auto& name = e->getComponent<CProfile>().playerName;
          m_playerName.setString(name);
          m_game->window().draw(m_playerName);
        }
    }
}

void SceneIntro::sRender()
{
  switch(m_state)
    {
    case State::INTRO_DIALOGUE:
      renderText("textIntro");
      break;

    case State::ELDER_DIALOGUE:
      renderText("textElder");
      break;

    case State::NAME_INPUT:
      renderPlayerName();
      break;

    default:
      break;
    }
}
