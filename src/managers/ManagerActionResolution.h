#pragma once

#include "ManagerActionFormation.h"
#include "ManagerLexicon.h"
#include "ManagerSystem.h"
#include <vector>
#include <memory>

class ActionResolutionManager
{
private:
   SystemManager* m_systemManager;
   LexiconManager* m_lexiconManager;

   std::vector<LinguisticAction> m_ongoingActions; //imperfective actions
   std::vector<LinguisticAction> m_futureActions; //future/contemplative

public:
   ActionResolutionManager(SystemManager* systemManager, LexiconManager* lexiconManager)
      : m_systemManager(systemManager), m_lexiconManager(lexiconManager)
   {
   }

   void executeAction(const LinguisticAction& action);
   void processOngoingActions(); //called each turn for imperfective acions
   void processFutureActions(); //called each turn for future actions

private:
   void applyPerfectiveEffects(const LinguisticAction& action);
   void scheduleImperfectiveAction(const LinguisticAction& action);
   void scheduleFutureAction(const LinguisticAction& action);

   std::shared_ptr<Entity> getActorByVoice(const LinguisticAction& action);
   std::shared_ptr<Entity> getPatientByVoice(const LinguisticAction& action);
   std::shared_ptr<Entity> getLocationByVoice(const LinguisticAction& action);

   bool validateAction(const LinguisticAction& action);
};
