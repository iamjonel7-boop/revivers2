#include "SceneMenu.h"
#include "SceneMain.h"
#include "GameEngine.h"
#include "SceneApprenticeship.h"
#include <SFML/Graphics.hpp>
#include <iostream>

SceneMenu::SceneMenu(GameEngine* gameEngine)
   : Scene(gameEngine)
{
   init();
}

SceneMenu::~SceneMenu()
{
}

void SceneMenu::init()
{
   registerMenuActions();

   if(!m_fontTitle.loadFromFile("/usr/share/fonts/TTF/Amiri-Bold.ttf"))
   {
      std::cout << "failed to load font\n";
   }

   m_colorTitle = sf::Color::White;

   auto title = m_entities.addEntity("text_ui");
   title->addComponent<CText>(m_fontTitle, "Revivers", 100, sf::Vector2f{100, 100}, m_colorTitle);
   auto play = m_entities.addEntity("text_ui");
   play->addComponent<CText>(m_fontTitle, "PLAY", 50, sf::Vector2f{100, 250}, m_colorTitle);
}

void SceneMenu::registerMenuActions()
{
   registerAction(sf::Keyboard::Space, "PLAY");
   registerAction(sf::Keyboard::Escape, "EXIT");
   // registerAction(sf::Keyboard::Backspace, "DELETE_CHAR");

   //testing
   registerAction(sf::Keyboard::Up, "Next");

   /*
   //console printing
   for(int key = sf::Keyboard::A; key <= sf::Keyboard::Z; ++key)
   {
      char c = 'A' + (key - sf::Keyboard::A);
      std::string actionName(1, c);
      registerAction(static_cast<sf::Keyboard::Key>(key), actionName);
   }

   for(int key = sf::Keyboard::Num0; key <= sf::Keyboard::Num9; ++key)
   {
      char c = '0' + (key - sf::Keyboard::Num0);
      std::string actionName(1, c);
      registerAction(static_cast<sf::Keyboard::Key>(key), actionName);
   }
   */
}

void SceneMenu::sDoAction(const Action& action)
{
   std::cout << "SceneMenu received action: " << action.name()
             << " type: " << action.type() << std::endl;

   if(action.type() == "START")
   {
      if(action.name() == "PLAY")
      {
         std::cout << "PLAY action - starting typing mode" << std::endl;
         startTyping();
      }
      else if(action.name() == "EXIT")
      {
         std::cout << "EXIT action - quitting game" << std::endl;
         onEnd();
      }
      else if(action.name() == "Next") // For testing
      {
         auto newScene = std::make_shared<SceneApprenticeship>(m_game);
         m_game->changeScene("apprenticeship", newScene);
         std::cout << "Next action received" << std::endl;
      }
   }
}

void SceneMenu::sRender()
{
   auto& window = m_game->window();
   window.clear();

   for(auto& e : m_entities.getEntities("text_ui"))
   {
      if(e->hasComponent<CText>())
      {
         window.draw(e->getComponent<CText>().text);
      }
   }

   if (isTyping())
   {
      sf::Text typingText;
      typingText.setFont(m_fontTitle);
      typingText.setString("Typing mode active... Press Enter to confirm");
      typingText.setCharacterSize(30);
      typingText.setPosition(100, 350);
      typingText.setFillColor(sf::Color::Yellow);
      window.draw(typingText);

      // Show current input
      if (!getCurrentInput().empty())
      {
         sf::Text inputText;
         inputText.setFont(m_fontTitle);
         inputText.setString("Input: " + getCurrentInput());
         inputText.setCharacterSize(30);
         inputText.setPosition(100, 400);
         inputText.setFillColor(sf::Color::Green);
         window.draw(inputText);
      }
   }

   window.display();
}

void SceneMenu::update()
{
   m_entities.update();
}


void SceneMenu::onEnd()
{
   m_game->quit();
}






/*
void SceneMenu::sDoAction(const Action& action)
{
   if(action.type() == "START")
   {
      if(!m_enteringName)
      {
         if(action.name() == "PLAY")
         {
            m_enteringName = true;

            std::cout << "Please enter your name...\n";
         }
         else if (action.name() == "QUIT")
         {
            onEnd();
         }
      }
      else //if(m_enteringName)
      {
         if(action.name() == "DELETE_CHAR" && !m_playerName.empty())
         {
            m_playerName.pop_back();
         }
         else if(action.name().size() == 1)
         {
            m_playerName += action.name();
         }
         else if(action.name() == "PLAY")
         {
            std::cout << "Player name confirmed: "
                      << m_playerName
                      << "\n";

            auto newScene = std::make_shared<SceneApprenticeship>(m_game);
            m_game->changeScene("apprenticeship", newScene);
         }
      }
   }
}
*/
