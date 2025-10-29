#pragma once

#include "ManagerLexicon.h"
#include "ManagerSystem.h"
#include "ManagerActionFormation.h"
#include "ManagerActionResolution.h"
#include <memory>
#include <vector>
#include <iostream>

struct TurnState
{
   int currentTurn = 0;
   bool turnInProgress = false;
   int playerActionPoints = 1;
   int maxPlayerActionPoints = 1;

   enum TurnPhase
   {
      PlayerAction,
      NPCAction,
      EventResolution,
      TurnEnd
   }
      currentPhase = PlayerAction;
};

class TurnManager
{
   TurnState m_state;
   std::vector<std::shared_ptr<Entity>> m_entitiesWithTurns;

   //delegated to specialized managers
   ActionFormationManager* m_formationManager;
   ActionResolutionManager* m_resolutionManager;
   SystemManager* m_systemManager;
   LexiconManager* m_lexiconManager;

public:
   TurnManager(SystemManager* systemManager, LexiconManager* lexiconManager, ActionFormationManager* formationManager, ActionResolutionManager* resolutionManager);

   //turn management
   void startTurn();
   void endTurn();
   void processTurnPhase();
   //  void consumePlayerActionPoint();
   //void resetPlayerActionPoints();
   void advanceToNextPhase();

   //player action interface
   void executePlayerAction();
   void cancelPlayerAction();

   //processing
   void processPlayerPhase();
   void processNPCPhase();
   void processEventPhase();
   void processTurnEnd();

   //entity registration,
   void registerEntityForTurns(std::shared_ptr<Entity> entity);
   void unregisterEntityForTurns(std::shared_ptr<Entity> entity);

   //action processing
   void processOngoingActions();
   void processFutureActions();

   //state queries
   bool isPlayerTurn() const;
   bool hasPlayerActionPoints() const;
   int getCurrentTurn() const;
   TurnState::TurnPhase getCurrentPhase() const;
   int getPlayerActionPoints() const;
   bool isTurnInProgress() const;

   //player entity management
   void setPlayerEntity(std::shared_ptr<Entity> player);
   void setLexiconManager(LexiconManager* LexiconManager);

   //action formation delegation
   void startActionFormation();
   void selectEntity(std::shared_ptr<Entity> entity);
   void selectMarker(const std::string& marker);
   void selectVerbRoot(const std::string& verbRoot);
   void selectVoice(const std::string& voice);
   void selectAspect(const std::string& aspect);

   std::string getSelectionPrompt() const;
   bool isActionFormationInProgress() const;
   bool isActionReadyToExecute() const;

private:
   void consumePlayerActionPoint();
   void resetPlayerActionPoints();
};
