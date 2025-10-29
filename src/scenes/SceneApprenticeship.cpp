#include "SceneApprenticeship.h"
#include "nlohmann/json.hpp"
#include "EntityManager.h"
#include "GameEngine.h"
#include <SFML/Window/Keyboard.hpp>
#include <fstream>
#include <iostream>
#include <stdexcept>

using json = nlohmann::json;

SceneApprenticeship::SceneApprenticeship(GameEngine* gameEngine)
   : Scene(gameEngine)
{
   init();
}

SceneApprenticeship::~SceneApprenticeship()
{
}

void SceneApprenticeship::init()
{
   registerApprenticeshipActions();
   loadDialogue("assets/dialogue.json");

   m_currentNode = &m_dialogue.nodes[m_dialogue.startNode];
   m_selectedOption = 0;

   spawnPlayer();
}

void SceneApprenticeship::spawnPlayer()
{
   m_playerEntity = m_entities.addEntity("player");
}

void SceneApprenticeship::registerApprenticeshipActions()
{
   registerAction(sf::Keyboard::Up, "UP");
   registerAction(sf::Keyboard::Down, "DOWN");
   registerAction(sf::Keyboard::Left, "LEFT");
   registerAction(sf::Keyboard::Right, "RIGHT");
   registerAction(sf::Keyboard::Enter, "SELECT");
}

Gift SceneApprenticeship::stringToGift(const std::string& s)
{
   if(s == "Kind") return Gift::Kind;
   if(s == "Clever") return Gift::Clever;
   if(s == "Cautious") return Gift::Cautious;
   return Gift::Courageous;
}

void SceneApprenticeship::sDoAction(const Action& action)
{
   if(action.name() == "UP")
   {
      if(m_selectedOption > 0) m_selectedOption--;
   }
   else if(action.name() == "DOWN")
   {
      if(m_selectedOption + 1 < m_currentNode->options.size())
         m_selectedOption++;
   }
   else if(action.name() == "SELECT")
   {
      if(!m_currentNode->options.empty())
      {
         DialogueOption& selected = m_currentNode->options[m_selectedOption];

         if(!selected.traitChanges.empty())
         {
            const auto& [traitStr, _] = *selected.traitChanges.begin();
            if(m_playerEntity)
               m_playerEntity->getComponent<CGift>().gift = stringToGift(traitStr);
         }

         m_currentNode = &m_dialogue.nodes[selected.nextNode];
         m_selectedOption = 0;
      }
   }
}

void SceneApprenticeship::loadDialogue(const std::string& path)
{
   try{
      std::ifstream file(path);
      if(!file.is_open())
      {
         throw std::runtime_error("could not open dialogue file");
      }
      json j;
      file >> j;

      m_dialogue.startNode = j["startNode"].get<std::string>();
      m_dialogue.nodes.clear();

      for(auto& nodeJson : j["nodes"])
      {
         DialogueNode node;
         node.id = nodeJson["id"].get<std::string>();
         node.speaker = nodeJson["speaker"].get<std::string>();
         node.text = nodeJson["text"].get<std::string>();

         for(auto& optJson : nodeJson["options"])
         {
            DialogueOption option;
            option.text = optJson["text"].get<std::string>();
            option.nextNode = optJson["next"].get<std::string>();

            if(optJson.contains("traitChanges"))
            {
               for(auto& item : optJson["traitChanges"].items())
               {
                  const std::string& trait = item.key();
                  int value = item.value().get<int>();
                  option.traitChanges[trait] = value;
               }
               node.options.push_back(option);
            }

            m_dialogue.nodes[node.id] = node;
         }
      }
   } catch (const std::runtime_error& e)
   {
      std::cerr << "dialogue error: " << e.what() << std::endl;
   }
}

void SceneApprenticeship::sRender()
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

   //test
   sf::Text text;
   sf::Font font;
   font.loadFromFile("/usr/share/fonts/TTF/Amiri-BoldItalic.ttf");
   text.setFont(font);
   text.setString("this is the apprenticeship scenee");
   text.setCharacterSize(30);
   text.setFillColor(sf::Color::White);
   text.setPosition(100,250);
   window.draw(text);

   window.display();
}

void SceneApprenticeship::update()
{

}

void SceneApprenticeship::onEnd()
{
}
