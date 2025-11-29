#include "SceneTitleScreen.h"
#include "GameEngine.h"
#include "SceneIntro.h"
#include <iostream>
#include <fstream>

TitleScreen::TitleScreen(GameEngine* gameEngine) :
  Scene(gameEngine)
{
  init();

  registerAction(sf::Keyboard::Left, static_cast<int>(ActionName::LEFT));
  registerAction(sf::Keyboard::Right, static_cast<int>(ActionName::RIGHT));
  registerAction(sf::Keyboard::Enter, static_cast<int>(ActionName::ENTER));

}

void TitleScreen::init()
{
  readFile("../file/titleScreen.txt");

  makeMenu();
  makeTitle();

  m_menus = {
    m_play,
    m_about,
    m_exit
  };

  m_currentMenuIndex = 0;

  m_screenTitleFont.loadFromFile("/usr/local/share/fonts/Liberation/LiberationMono-Bold.ttf");

}

void TitleScreen::makeTitle()
{
  m_title.setFont(m_screenTitleFont);
  m_title.setString(m_gameTitle);
  m_title.setCharacterSize(100);
  m_title.setFillColor(sf::Color::White);
  m_title.setPosition(100.f, 100.f);

  m_tag.setFont(m_screenTitleFont);
  m_tag.setString(m_gameTagline);
  m_tag.setCharacterSize(30);
  m_tag.setFillColor(sf::Color::White);
  m_tag.setPosition(100.f, 225.f);
}

void TitleScreen::makeMenu()
{
  m_play.setFont(m_screenTitleFont);
  m_play.setString("PLAY");
  m_play.setCharacterSize(20);
  m_play.setPosition(100.f, 300.f);

  m_about.setFont(m_screenTitleFont);
  m_about.setString("ABOUT");
  m_about.setCharacterSize(20);
  m_about.setPosition(200.f, 300.f);

  m_exit.setFont(m_screenTitleFont);
  m_exit.setString("EXIT");
  m_exit.setCharacterSize(20);
  m_exit.setPosition(300.f, 300.f);
}

void TitleScreen::readFile(const std::string& filename)
{
  std::fstream fileIn(filename, std::ios::in);
  std::string line;

  if(!fileIn.is_open())
    {
      std::cerr << "Could not open file.\n";
    }

  if(!std::getline(fileIn, m_gameTitle))
    {
      std::cerr << "File missing title.\n";
    }
  else
    {
      std::cout << m_gameTitle << std::endl;
    }

  if(!std::getline(fileIn, m_gameTagline))
    {
      std::cerr << "File missing tagline.\n";
    }
  else
    {
      std::cout << m_gameTagline << std::endl;
    }

  if(!std::getline(fileIn, m_gameDescription))
    {
      std::cerr << "File missing tagline.\n";
    }
  else
    {
      std::cout << m_gameDescription << std::endl;
    }
}

void TitleScreen::handleMenus()
{
  switch(m_currentMenuIndex)
    {
    case 0:
      m_game->changeScene("intro", std::make_shared<SceneIntro>(m_game));
      break;

    case 1:
      break;

    case 2:
      onEnd();
      break;
    }
}

void TitleScreen::sDoAction(const Action& action)
{
  ActionName actionName = static_cast<ActionName>(action.name());

  switch(action.type())
    {
    case ActionType::START:
      switch(actionName)
        {
        case ActionName::LEFT:
          m_currentMenuIndex = (m_currentMenuIndex + 1) % m_menus.size();
          std::cout << m_currentMenuIndex << std::endl;
          break;

        case ActionName::RIGHT:
          m_currentMenuIndex = (m_currentMenuIndex + m_menus.size() -1) % m_menus.size();
          std::cout << m_currentMenuIndex << std::endl;
          break;

        case ActionName::ENTER:
          handleMenus();
          break;
        }
      break;

    default:
      break;
    }
}

void TitleScreen::updateMenus()
{
  m_play.setFillColor(sf::Color::White);
  m_about.setFillColor(sf::Color::White);
  m_exit.setFillColor(sf::Color::White);

  switch(m_currentMenuIndex)
    {
    case 0: m_play.setFillColor(sf::Color::Red); break;
    case 1: m_about.setFillColor(sf::Color::Red); break;
    case 2: m_exit.setFillColor(sf::Color::Red); break;
    }
}

void TitleScreen::update()
{
  updateMenus();
}

void TitleScreen::onEnd()
{
  m_game->quit();
}

void TitleScreen::sRender()
{
  m_game->window().draw(m_title);
  m_game->window().draw(m_tag);
  m_game->window().draw(m_play);
  m_game->window().draw(m_about);
  m_game->window().draw(m_exit);
}
