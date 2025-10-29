#pragma once

#include "Scene.h"
#include "GameEngine.h"
#include "ManagerEntity.h"
#include "ManagerTurn.h"
#include "Components.h"
#include <memory>
#include <vector>
#include <map>

class SceneEspionage : public Scene
{
public:
   enum class EspionageState
   {
      INFILTRATING,
      SOLVING_ANAGRAM,
      FINAL_TRANSLATION,
      COMPLETED,
      FAILED,
      DETECTED
   };

   struct KeyPoint
   {
      sf::Vector2i position;
      std::string anagram;
      std::string solution;
      bool isCompleted = false;
   };

protected:
   sf::Font m_fontMain;
   sf::Color m_colorMain;

   std::unique_ptr<TurnManager> m_turnManager;

   sf::Clock m_timer;
   bool m_isReturning = false;
   float m_returnDelay = 3.f;

   //espionage state
   EspionageState m_state;
   std::vector<KeyPoint> m_keyPoints;
   int m_currentKeyPoint;

   //entities
   std::shared_ptr<Entity> m_buildingEntity;
   std::shared_ptr<Entity> m_playerEntity;
   std::shared_ptr<Entity> m_cursorEntity;
   std::shared_ptr<Entity> m_guardEntity;

   //map data
   std::vector<std::vector<std::shared_ptr<Entity>>> m_grid;
   st::Vector2i m_gridSize;
   int m_tileSize;

   //game state
   std::string m_buildingEnglishName;
   std::string m_buildingNativeName;
   std::string m_currentAnagram;
   std::string m_playerSolution;
   std::string m_feedbackMessage;

   //ui
   std::shared_ptr<Entity> m_espionageUIEntity;

   void init();
   void update();
   void sDoAction(const Action& action);
   void onEnd();

   void sInitializeFloorPlan();
   void sMoveToNextPoint();
   void sGenerateAnagram();
   void sCheckAnagramSolution();
   void sCheckFinalTranslation();
   void sCompleteEspionage();
   void sFailEspionage();

   void sRenderFloorPlan();
   void sRenderAnagramUI();
   void sRenderTranslationUI();
   void sRenderPlayerMarker();

   std::string sScrambleWord(const std::string& word);
   bool sIsvalidTranslation(const std::string& translation);

public:
   SceneEspionage(GameEngine* gameEngine, std::shared_ptr<Entity> player, std::shared_ptr<Entity> building, const std::string& englishName);
   void sRender();
};
