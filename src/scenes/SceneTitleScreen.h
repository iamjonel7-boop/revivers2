#pragma once

#include "Scene.h"
#include <string>
#include <vector>

class TitleScreen : public Scene
{
  enum class ActionName
  {
    LEFT,
    RIGHT,
    ENTER
  };

  enum class Menu
  {
    ABOUT,
    EXIT,
    PLAY
  };

  std::string m_gameTitle;
  std::string m_gameDescription;
  std::string m_gameTagline;

  sf::Font m_screenTitleFont;

  sf::Text m_title;
  sf::Text m_tag;

  sf::Text m_play;
  sf::Text m_about;
  sf::Text m_exit;

  std::vector<sf::Text> m_menus;
  size_t m_currentMenuIndex;

  void init();
  void readFile(const std::string& filename);

  void makeTitle();
  void makeMenu();

  void sDoAction(const Action& action) override;
  void handleMenus();

  void update() override;
  void updateMenus();

  void onEnd() override;

 public:
  TitleScreen(GameEngine* gameEngine);
  void sRender() override;
};
