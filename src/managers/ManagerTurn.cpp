#include "ManagerTurn.h"
#include <iostream>
#include <algorithm>
#include <random>

TurnManager::TurnManager(SystemManager* systemManager, LexiconManager* lexiconManager, ActionFormationManager* formationManager, ActionResolutionManager* resolutionManager)
   : m_systemManager(systemManager), m_lexiconManager(lexiconManager), m_formationManager(formationManager), m_resolutionManager(resolutionManager)
{
   m_state = TurnState();
}


//turn management
void TurnManager::startTurn()
{
   m_state.turnInProgress = true;
   m_state.currentPhase = TurnPhase::PlayerAction;
   m_state.playerActionPoints = m_state.maxPlayerActionPoints;

   //reset action points for all registered entities
   for(auto& entity : m_entitiesWithTurns)
   {
      if(entity->hasComponent<CTurn>())
      {
         entity->getComponent<CTurn>().hasTurn = true;
         entity->getComponent<CTurn>().actionPoints = entity->getComponent<CTurn>().maxActionPoints;
      }
   }

   //process any future actions from previous turns
   if(m_resolutionManager)
   {
      m_resolutionManager->processFutureActions();
   }

   std::cout << "Starting turn " << m_state.currentTurn << std::endl;
}

void TurnManager::endTurn()
{
   m_state.turnInProgress = false;
   m_state.currentTurn++;

   std::cout << "Ending turn " << m_state.currentTurn - 1 << std::endl;

   //cancel any ongoing action formation
   if(m_formationManager && m_formationManager->isFormationInProgress())
   {
      m_formationManager->cancelFormation();
   }

   startTurn();
}

void TurnManager::processTurnPhase()
{
   if(!m_state.turnInProgress) return;

   switch(m_state.currentPhase)
   {
   case TurnPhase::PlayerAction:
      processPlayerPhase();
      break;
   case TurnPhase::NPCAction:
      processNPCPhase();
      break;
   case TurnPhase::EventResolution:
      processEventPhase();
      break;
   case TurnPhase::TurnEnd:
      processTurnEnd();
      break;
   }
}

void TurnManager::processPlayerPhase()
{
   //process imperfective


   //if there are ongoing actions, dotn advanceToNextPhase
   if(m_resolutionManager && m_resolutionManager->hasOngoingActions())
   {
      std::cout << "Waiting for ongoing actions to complete..." << std::endl;
      if(m_worldManager)
      {
         m_worldManager->processDuringPlayerAction();
      }
      m_resolutionManager->processOngoingActions();
      return; //stay in playerphase
   }
}

//void TurnManager::processPlayerPhase()
//{
//   if(m_state.playerActionPoints <= 0)
//   {
//      advanceToNextPhase();
//   }
//   std::cout << "Player phase done. Advancing to next phase.\n";
//}

void TurnManager::processNPCPhase() {
   // Normal NPC turns (when no imperfective action)
   if (m_worldManager) {
      m_worldManager->patrolImperialPolice();
      m_worldManager->spawnAndWanderCivilians();
      m_worldManager->guardBuildings();
   }
   advanceToNextPhase();
}

void TurnManager::processEventPhase() {
   // Events and world updates
   if (m_worldManager) {
      m_worldManager->triggerRandomEvents();
      m_worldManager->updateWorldState();
   }
   advanceToNextPhase();
}

void TurnManager::processTurnEnd() {
   // Cleanup and preparation for next turn
   advanceToNextPhase();
}

void TurnManager::advanceToNextPhase()
{
   switch(m_state.currentPhase)
   {
   case TurnPhase::PlayerAction:
      m_state.currentPhase = TurnPhase::NPCAction;
      break;
   case TurnPhase::NPCAction:
      m_state.currentPhase = TurnPhase::EventResolution;
      break;
   case TurnPhase::EventResolution:
      m_state.currentPhase = TurnPhase::TurnEnd;
      break;
   case TurnPhase::TurnEnd:
      endTurn();
      startTurn();
      break;
   }
}


void TurnManager::consumePlayerActionPoint()
{
   if(m_state.playerActionPoints > 0)
   {
      m_state.playerActionPoints--;
   }
}


void TurnManager::cancelActionFormations()
{
   m_formationState = FormationState::IDLE;
   m_currentAction = LinguisticAction();
   m_availableMarkers.clear();
   m_usedMarkers.clear();
   m_selectedPrompt = "Action formation cancelled";
}

void TurnManager::resetPlayerActionPoints()
{
   m_state.playerActionPoints = m_state.maxPlayerActionPoints;
}

void TurnManager::registerEntityForTurns(std::shared_ptr<Entity> entity)
{
   if(entity->hasComponent<CTurn>())
   {
      m_entitiesWithTurns.push_back(entity);
   }
}

void TurnManager::unregisterEntityForTurns(std::shared_ptr<Entity> entity)
{
   m_entitiesWithTurns.erase(std::remove(m_entitiesWithTurns.begin(), m_entitiesWithTurns.end(), entity), m_entitiesWithTurns.end());
}

bool TurnManager::isPlayerTurn() const
{
   return m_state.turnInProgress && m_state.currentPhase == TurnPhase::PlayerAction;
}

bool TurnManager::hasPlayerActionPoints() const
{
   return m_state.playerActionPoints > 0;
}

int TurnManager::getCurrentTurn() const
{
   return m_state.currentTurn;
}

TurnManager::TurnPhase TurnManager::getCurrentPhase() const
{
   return m_state.currentPhase;
}

int TurnManager::getPlayerActionPoints() const
{
   return m_state.playerActionPoints;
}

bool TurnManager::isTurnInProgress() const
{
   return m_state.turnInProgress;
}

void TurnManager::executePlayerAction() {
   if (!m_formationManager || !m_resolutionManager) return;

   if (!m_formationManager->isFormationComplete()) {
      return;
   }

   if (!isPlayerTurn() || !hasPlayerActionPoints()) {
      return;
   }

   // Get the formed action and execute it
   auto action = m_formationManager->getCurrentAction();
   m_resolutionManager->executeAction(action);
   consumePlayerActionPoint();

   // Ready for next action or return to navigation
   if (hasPlayerActionPoints()) {
      m_formationManager->startFormation(); // Ready for next action
   }
}
