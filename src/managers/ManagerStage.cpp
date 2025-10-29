#include "ManagerStage.h"
#include <iostream>

StageManager::StageManager()
   : m_currentStage(GameStage::STAGE_1_APPRENTICESHIP)
{
   initializeStage();
}

void StageManager::initializeStage()
{
   m_stages.clear();

   //stage 1
   StageData stage1;
   stage1.stage = GameStage::STAGE_1_APPRENTICESHIP;
   stage1.name = "Apprenticeship";
   stage1.objectives.push_back(StageObjective("Visit the Elder", [this]() { return checkStage1Completion(); }));
   stage1.unlockedBuildings = {};
   stage1.unlockedActions = {"move", "greet"};
   m_stages.push_back(stage1);

   //stage2

}

void StageManager::updateStageProgress()
{
   // const auto& currentStageData = getCurrentStageData();

   bool stageCompleted = checkStageCompletion(m_currentStage);
   if(stageCompleted)
   {
      std::cout << "Stage " << static_cast<int>(m_currentStage) << " completed!" << std::endl;
      advanceToNextStage();
   }
}

void StageManager::advanceToNextStage()
{
   int currentStageInt = static_cast<int>(m_currentStage);
   if(currentStageInt < 8)
   {
      m_currentStage = static_cast<GameStage>(currentStageInt + 1);
      unlockStageFeatures(m_currentStage);

      std::cout << "Advance to stage: " << getCurrentStageName() << std::endl;
   }
   else
   {
      std::cout << "Game completed! All stages finished." << std::endl;
   }
}

bool StageManager::checkStageCompletion(GameStage stage) const
{
   switch(stage)
   {
   case GameStage::STAGE_1_APPRENTICESHIP:
      return checkStage1Completion();
   case GameStage::STAGE_2_RECRUITMENT:
      return checkStage2Completion();
   }
   return false;
}

void StageManager::incrementRecruitedCivilians(int amount)
{
   m_recruitedCivilians += amount;
   updateStageProgress();
}

void StageManager::incrementControlledBuildings(int amount)
{
   m_controlledBuildings += amount;
   updateStageProgress();
}

void StageManager::updateLiteracyRate(float literacyRate)
{
   m_literacyRate = literacyRate;
   updateStageProgress();
}

void StageManager::updateLanguagePrestige(float prestige)
{
   m_languagePrestige = prestige;
   updateStageProgress();
}

StageManager::GameStage StageManager::getCurrentStage() const
{
   return m_currentStage;
}

std::string StageManager::getCurrentStageName() const
{
   return getCurrentStageData().name;
}

const StageManager::StageData& StageManager::getCurrentStageData() const
{
   for(const auto& stage : m_stages)
   {
      if(stage.stage == m_currentStage){
         return stage;
      }
   }
   static StageData defaultStage;
   return defaultStage;
}

float StageManager::getStageCompletionPercentage() const
{
   const auto& stageData = getCurrentStageData();
   if(stageData.objectives.empty()) return 0.0f;
   int completed = 0;
   for(const auto& objective : stageData.objectives)
   {
      if(objective.isCompleted) completed++;
   }
   return static_cast<float>(completed)/stageData.objectives.size();
}

void StageManager::setPlayerEntity(std::shared_ptr<Entity> player)
{
   m_playerEntity = player;
}

void StageManager::onBuildingCaptured()//const std::string& buildingType
{
   incrementControlledBuildings(1);
}

void StageManager::onCivilianRecruited()
{
   incrementRecruitedCivilians(1);
}

void StageManager::onWordAddedToDictionary()//const std::string& word
{
   m_languagePrestige =+ 0.1f;
   updateStageProgress();
}

void StageManager::unlockStageFeatures(GameStage stage)
{
   for(const auto& stageData : m_stages)
   {
      if(stageData.stage == stage)
      {
         std::cout << "Unlocked features for stage " << stageData.name << ":" << std::endl;
         for(const auto& building :stageData.unlockedBuildings)
         {
            std::cout << " - Building: " << building << std::endl;
         }
         for(const auto& action : stageData.unlockedActions)
         {
            std::cout << " - Action: " << action << std::endl;
         }
         break;
      }
   }
}

bool StageManager::checkStage1Completion() const
{
   return true;
}

bool StageManager::checkStage2Completion() const
{
   return m_recruitedCivilians >= 5;
}
//add for the other stages
