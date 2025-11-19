#include "SceneIntro.h"
#include "Components.h"
#include "GameEngine.h"
#include <iostream>
#include <fstream>
#include "SceneMain.h"

//TODO: add checks/warnings

SceneIntro::SceneIntro(GameEngine* gameEngine) :
  Scene(gameEngine)
{
  init();
  m_state = State::INTRO_DIALOGUE;
  m_currentLine = 0;

  registerAction(sf::Keyboard::D, static_cast<int>(ActionName::NEXT));
  registerAction(sf::Keyboard::A, static_cast<int>(ActionName::BACK));
  registerAction(sf::Keyboard::Enter, static_cast<int>(ActionName::ENTER));
}

void SceneIntro::init()
{
  m_game->getWorldManager()->m_createPlayer(m_entities);
  m_player = m_game->getWorldManager()->getPlayer();

  /*
  m_player = m_entities.addEntity("player"); //remove
  m_player->addComponent<CTransform>();
  */

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
  m_playerName = m_entities.addEntity("name"); //remove
  m_playerName->addComponent<CText>(m_font,
                                    "",
                                    24,
                                    sf::Vector2f(200.f, 200.f),
                                    sf::Color::White);

}

void SceneIntro::update()
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
      auto& t = m_playerName->getComponent<CText>().text;
      t.setString(getTextInputManager().getText());
      break;
    }
}

void SceneIntro::sDoAction(const Action& action) //maybe we can use switch here. it is prettier
{
  ActionName act = static_cast<ActionName>(action.name());
  std::string name;

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
                  std::cout << "End of intro dialogue. --------------" << std::endl;
                  m_currentLine = 0;
                  m_state = State::ELDER_DIALOGUE;
                  std::cout << "At elder_dialogue state." << std::endl;
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
                  std::cout << "End of elder dialogue.--------------------" << std::endl;
                  m_state = State::NAME_INPUT;
                  std::cout << "At name_input state." << std::endl;
                  getTextInputManager().start();
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
            case ActionName::ENTER:
              name = getTextInputManager().getText();
              m_player->getComponent<CProfile>().playerName = name;

              m_game->changeScene("main", std::make_shared<SceneMain>(m_game));
              std::cout << "Scene changed to main/map." << std::endl;
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
        renderText("name");
      break;

    default:
      break;
    }
}
