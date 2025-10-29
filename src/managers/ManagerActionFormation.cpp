#include "ManagerActionFormation.h"
#include <algorithm>
#include <iostream>

void ActionFormationManager::startFormation()
{
   m_state = FormationState::SELECTING_ENTITY_AND_MARKER;
   m_currentAction = LinguisticAction();
   m_availableMarkers = {"si", "ni", "ki", "sa", "kan", "an"};
   m_usedMarkers.clear();
   m_selectionPrompt = "Select entity and assign marker";
}

void ActionFormationManager::selectEntityWithMarker(std::shared_ptr<Entity> entity, const std::string &marker)
{
   if(m_state != FormationState::SELECTING_ENTITY_AND_MARKER) return;



   //validate marker is available and valid for entity
   if(std::find(m_availableMarkers.begin(), m_availableMarkers.end(), marker) == m_availableMarkers.end())
   {
      m_selectionPrompt = "Invalid marker selection";
      return;
   }

   if(!isEntityValidForMarker(entity, marker))
   {
      m_selectionPrompt = "Invalid marker for this enity type";
      return;
   }

   //add to marked entities
   m_currentAction.markedEntities[marker] = entity;

   //remove used marker form available options
   m_availableMarkers.erase(std::remove(m_availableMarkers.begin(), m_availableMarkers.end(), marker), m_availableMarkers.end());

   //update prompt, player can add more entities or proceed to verb
   bool hasFocusMarker = m_currentAction.markedEntities.count("si") || m_currentAction.markedEntities.count("an");

   if(hasFocusMarker)
   {
      m_selectionPrompt = "Entity marked. [A]dd another entity or [P]roceed to verb";
   }
   else
   {
      m_selectionPrompt = "Select another Entity (need si/an)";
   }
}

void ActionFormationManager::proceed
