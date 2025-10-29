#pragma once

#include "Scene.h"
#include "GameEngine.h"
#include "SceneApprenticeship.h"
#include "Components.h"
#include <SFML/Graphics.hpp>
#include <map>
#include <memory>

class SceneMenu : public Scene
{
protected:
   sf::Font m_fontTitle;
   sf::Font m_fontMenu;
   sf::Color m_colorTitle;
   sf::Color m_colorMenu;
   sf::Color m_colorSelected;

   enum class MenuState
   {
      MAIN_MENU,
      ENTERING_NAME,
      CONFIRMING_NAME
   };

   //menu items
   std::vector<std::string> m_menuItems;
   int m_selectedItem;

   //player name input
   std::string m_playerName;
   sf::Text m_namePromptText;
   sf::Text m_nameInputText;
   sf::Text m_nameConfirmText;

   void init();
   void registerMenuActions();

   void navigateUp();
   void navigateDown();
   void selectMenuItem();

   void startNameInput();
   void handleNameInput(const std::string& input);
   void confirmName();
   void cancelNameInput();

   void renderMainMenu();
   void renderNameInput();
   void renderMenuItems();

   void sDoAction(const Action& action);
   void update();
   void onEnd();

   void initializeManagers(){}

public:
   SceneMenu(GameEngine* gameEngine);
   virtual ~SceneMenu();
   void sRender();
};
