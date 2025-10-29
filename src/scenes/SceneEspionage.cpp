#include "SceneEspionage.h"
#include <iostream>
#include <algorithm>
#include <random>

SceneEspionage::SceneEspionage(GameEngine* gameEngine, std::shared_ptr<Entity> player, std::shared_ptr<Entity> building, const std::string& englishName)
   : Scene(gameEngine), m_playerEntity(player), m_buildingEntity(building), m_buildingEnglishName(englishName)
{
   init();
}

void SceneEspionage::init()
{
   m_state = EspionageState::INFILTRATING;

   //this should be loaded form a file, and the game choses random words

   if(m_buildingEnglishName == "school")
   {
      m_buildingNativeName = "eskuwela";
   }
   else m_buildingNativeName = "unknwn";

   std::cout << "Espionage scene initialized for: " << m_buildingEnglishName << std::endl;
}

void SceneEspionage::update()
{
   if(!m_paused)
   {
      //update based on current state
      switch(m_state)
      {
      case EspionageState::INFILTRATING:
         //timer, if the player becomes idle, building will be locked down, espionage fail, reduce reputation of player
         break;
      case EspionageState::SOLVING_ANAGRAM:
         //there is a timer for player input
         break;
      case EspionageState::FINAL_TRANSLATION:
         //same condition
         break;
      case EspionageState::COMPLETED:
         //add reputation
         break;
      case EspionageState::Failed:
         //reduce reputation
         //end scene after delay
         break;
      }
   }
}

void SceneEspionage::sDoAction(const Action& action)
{
   if(action.type() == "START")
   {
      if(action.name() == "PAUSE")
      {
         setPaused(!m_paused);
      }
      else if(m_state == EspionageState::INFILTRATING && action.name() == "SELECT")
      {
         sMoveToNextPoint();
      }
      else if((m_state == EspionageState::SOLVING_ANAGRAM || m_state == EspionageState::FINAL_TRANSLATION) && action.name() == "SUBMIT ANSWER")
      {
         if(m_state == EspionageState::SOLVING_ANAGRAM)
         {
            sCheckAnagramSolution();
         }
         else
         {
            sCheckFinalTranslation();
         }
      }
      else if(action.name() == "START_TYPING" && (m_state == EspionageState::SOLVING_ANAGRAM || m_state == EspionageState::FINAL_TRANSLATION))
      {
         startTyping();
      }
      else if(action.name() == "CANCEL_TYPING")
      {
         stopTyping();
         inputClear();
      }
   }

   //text input
   if(m_isTyping && (m_state == EspionageState::SOLVING_ANAGRAM || m_state == EspionageState::FINAL_TRANSLATION))
   {
      if(action.type() == "TEXT_INPUT")
         {
            m_playerSolution += action.name();
         }
   }
}

void SceneEspionage::sInitializeFloorPlan();
{
   m_floorPlan = FloorPlan();

   //create three key points
   m_floorPlan.keyPoints = {
      sf::Vector2f(200, 300),
      sf::Vector2f(500, 200),
      sf::Vector2f(800, 400)
   };

   //player should move by using verbs

   //generate anagrams, again, this should be loaded from file, anagrams should be random
   std::vector<std::string> buildingWords;
   if(m_buildingEnglishName == "school")
   {
      buildingWords = {"teacher", "student", "classroom"};
   }

   for(const auto& word : buildingWords)
   {
      m_floorPlan.anagrams.push_back(sScrambleWord(word));
   }

   m_floorPlan.completedPoints.resize(3, false);
   m_floorPlan.currentPoint = 0;
}

void SceneEspionage::sMoveToNextPoint()
{
   if(m_floorPlan.)
