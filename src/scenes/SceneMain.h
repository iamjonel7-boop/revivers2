#pragma once

#include "Scene.h"
#include "SceneCombat.h"
#include "SceneEspionage.h"
#include "StructsConfig.h"
#include <memory>
#include <map>

class SceneMain : public Scene
{
protected:
   //main entities
   std::shared_ptr<Entity> m_mapEntity;
   std::shared_ptr<Entity> m_playerEntity;
   std::shared_ptr<Entity> m_cursorEntity;

   //configs
   PlayerConfig m_playerConfig;
   ImperialPatrolConfig m_imperialPatrolConfig;
   CivilianConfig m_civilianConfig;

   //map data
   std::vector<std::vector<std::shared_ptr<Entity>>> m_grid;
   sf::Vector2i m_gridSize;
   sf::Vector2i m_cursorPos;
   sf::Vector2i m_cameraOffset;

   //control state
   enum class MapControlState
   {
      NAVIGATING,
      SELECTING_ENTITY,
      FORMING_ACTION
   };

   MapControlState m_controlState;
   int m_selectedEntityIndex;

   //action formation states
   enum class SelectionState
   {
      SELECTING_FOCUS,
      SELECTING_VERB,
      SELECTING_NONFOCUS,
      READY_TO_EXCUTE
   };




   SelectionState m_selectionState;
   std::string m_currentVerb;
   std::string m_currentAspect;
   std::string m_currentRole;
   std::shared_ptr<Entity> m_focusEntity;
   std::shared_ptr<Entity> m_nonFocusEntity;

   //espionage tracking
   bool m_imperfectiveUsedRecently = false;
   int m_imperfectiveTimer = 0;

   void init(); //const std::string& config
   void update();
   void sDoAction(const Action& action);
   void onEnd();

   //initialisation
   void initializeManagers() override;
   void sInitializeMap();
   void sLoadWordDatabase();
   void sLoadSemanticRelations();

   //entity creation
   void sCreatePlayerEntity();
   void sCreateCursorEntity();
   void sSpawnImperialGuard();
   void sSpawnCivilian();
   void sSpawnBuilding(int x, int y, const std::string& type);

   //core game systems
   void sTurnManagement();
   void sInputHandling();
   void sMovement();
   void sMapRendering();
   void sCursorManagement();
   void sActionFormation();
   void sCombatCollisionDectection();
   void sBuildingInteractionDetection();
   void sRenderUI;
   void sRenderActionFormationUI();

   //action formation helpers
   void sSelectEntityForAction(std::shared_ptr<Entity> entity);
   void sSelectVerbForAction(const std::String& verbRoot, const std::string& aspect, const std::string& role);
   void sExecuteCurrentAction();
   void sResetActionFormation();
   std::string sFormSentence() const;

   //espionage systems
   void sImperiumEspionage();
   void sUpdateIdleTracker();
   void sStealPlayerWords(int wordCount);
   void sTriggerImperiumEspionage(float chanceMultiplier = 1.0f);
   bool sShouldTriggerEspionage(float chanceMultiplier = 1.0f) const;
   std::vector<std::string> sSelectWordsToSteal(int count) const;
   void sShowAmbiguousEspionageWarning(int wordsStolen, int originalWordCount);
   void sTriggerEspionageVisualEffects();
   void sApplyEspionagePenalties(int wordsStolen);
   void sApplyCivilianMoralePenalty(int wordsStolen);
   void sApplyCombatPenalties(int wordsStolen);
   void sUpdateSuspicionSystem();
   void sIsPlayerUsingDiverseVerbs() const;
   void sImplementWordDiscoveryMechanics();
   void sRediscoverStolenWord();
   void sRenderEspionageAwarenessUI();
   void sHasPlayerUsedImperfectiveRecently() const;

   //ai and npc systems
   void sAIMovement();
   void sGuardAI();
   void sDetectionCheck();
   void sPlayerDetected();

   //pathfinding and vision
   void sPathFinding();
   void sVision();
   std::vector<sf::Vector2i> sFindPath(std::shared_ptr<Entity> entity, const sf::Vector2i& target);
   bool sHasLineOfSight(sf::Vector2i start, sf::Vector2i end) const;

   //helper methods
   std::shared_ptr<Entity> sGetPlayerEntity();
   std::shared_ptr<Entity> sGetCursorEntity();
   std::shared_ptr<Entity> sGetTileAt(int x, int y);
   std::vector<std::shared_ptr<Entity>> sGetEntitiesAtTile(int x, int y);
   bool sIsValidGridPos(int x, int y) const;
   bool sIsTileWalkable(int x, int y) const;
   void sCursorMove(int dx, int dy);
   void sCursorCycleEntity();
   void sCameraUpdate();
   int sCalculateDistance(sf::Vector2i a, sf::Vector2i b) const;

   //scene transitions
   void sStartCombat(std::shared_ptr<Entity> enemy);
   void sStartEspionage(std::shared_ptr<Entity> building);

public:
   SceneMain(GameEngine* gameEngine);
   void sRender() override;
};
