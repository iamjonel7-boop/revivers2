#pragma once

#include "Entity.h"
#include "ManagerGrammar.h"
#include <memory>
#include <map>
#include <vector>
#include <string>
#include <sstream>

struct ActionArgument
{
   std::shared_ptr<Entity> entity;
   std::string marker;
   ActionArgument(std::shared_ptr<Entity> e)
      : entity(e)
   {
   }
};

struct LinguisticAction
{
   std::map<std::string, std::shared_ptr<Entity>> markedEntities; //marker, entity
   std::string verbRoot;
   std::string voice;
   std::string aspect;
   std::string formedSentence;
   bool isFutureAction = false;
   int turnScheduled = 0;
   LinguisticAction()
   {
   }
};

enum class FormationState
{
   IDLE,
   SELECTING_ENTITY_AND_MARKER,
   SELECTING_VERB_ROOT,
   SELECTING_VOICE,
   SELECTING_ASPECT,
   READY_TO_EXECUTE
};

class ActionFormationManager
{
   FormationState m_state = FormationState::IDLE;
   LinguisticAction m_currentAction;
   std::vector<std::string> m_availableMarkers;
   std::vector<std::string> m_usedMarkers;
   std::string m_selectionPrompt;

   GrammarManager* m_grammarManager;

public:
   ActionFormationManager(GrammarManager* grammarManager)
      : m_grammarManager(grammarManager)
   {
   }

   //action formation
   void startFormation();
   void selectEntityWithMarker(std::shared_ptr<Entity> entity, const std::string& marker);
   void selectVerbRoot(const std::string& verbRoot);
   void selectVoice(const std::string& voice);
   void selectAspect(const std::string& aspect);
   void cancelFormation();
   bool isFormationComplete() const
   {
      return m_state == FormationState::READY_TO_EXECUTE;
   }
   bool isFormationInProgress() const
   {
      return m_state != FormationState::IDLE;
   }
   const LinguisticAction& getCurrentAction() const
   {
      return m_currentAction;
   }
   std::string getSelectionPrompt() const
   {
      return m_selectionPrompt;
   }
   FormationState getCurrentState() const
   {
      return m_state;
   }

   void renderUI();

private:
   void buildSentence();
   bool isEntityValidForMarker(std::shared_ptr<Entity> entity, const std::string& marker) const;
   void proceedToVerbSelection();
};
