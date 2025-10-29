#pragma once

#include "Entity.h"
#include "Components.h"
#include <string>
#include <vector>
#include <functional>
#include <map>

class StageManager
{
public:
   enum class GameStage
   {
      STAGE_1_APPRENTICESHIP = 1,
      STAGE_2_RECRUITMENT = 2
      //STAGE_3_STRONGHOLDS = 3,
      //STAGE_4_LITERACY = 4,
      //STAGE_5_SCHOOLS = 5,
      //STAGE_6_WORKPLACES = 6,
      //STAGE_7_GOVERNMENT = 7,
      //STAGE_8_INSTITUTIONAL_POWER = 8
   };

   struct StageObjective
   {
      std::string description;
      bool isCompleted = false;
      std::function<bool()> completionCondition;

      StageObjective(const std::string& desc, std::function<bool()> condition)
         : description(desc), completionCondition(condition)
      {
      }
   };

   struct StageData
   {
      GameStage stage;
      std::string name;
      std::vector<StageObjective> objectives;
      std::vector<std::string> unlockedBuildings;
      std::vector<std::string> unlockedActions;
      int requiredRecruitedCivilians = 0;
      int requiredControlledBuildings = 0;
      float requiredLiteracyRate = 0.0f;
      float requiredLanguagePrestige = 0.0f;
   };

private:
   GameStage m_currentStage;
   std::vector<StageData> m_stages;
   std::shared_ptr<Entity> m_playerEntity;

   //progress tracking
   int m_recruitedCivilians = 0;
   int m_controlledBuildings = 0;
   float m_literacyRate = 0.0f;
   float m_languagePrestige = 0.0f;

public:
   StageManager();

   //stage management
   void initializeStage();
   void updateStageProgress();
   void advanceToNextStage();
   bool checkStageCompletion(GameStage stage) const;

   //progress tracking
   void incrementRecruitedCivilians(int amount = 1);
   void incrementControlledBuildings(int amount = 1);
   void updateLiteracyRate(float literacy);
   void updateLanguagePrestige(float prestige);

   //getters
   GameStage getCurrentStage() const;
   std::string getCurrentStageName() const;
   const StageData& getCurrentStageData() const;
   float getStageCompletionPercentage() const;
   int getRecruitedCivilians() const
   {
      return m_recruitedCivilians;
   }
   int getControlledBuildings() const
   {
      return m_controlledBuildings;
   }
   float getLiteracyRate() const
   {
      return m_literacyRate;
   }
   float getLanguagePrestige() const
   {
      return m_languagePrestige;
   }

   //player management
   void setPlayerEntity(std::shared_ptr<Entity> player);

   //event triggers
   void onBuildingCaptured(); //const std::string& buildingTyper
   void onCivilianRecruited();
   void onWordAddedToDictionary(); //const std::string& word, to be added to file

private:
   void setupStageObjectives();
   void unlockStageFeatures(GameStage stage);
   bool checkStage1Completion() const;
   bool checkStage2Completion() const;
   //bool checkStage3Completion() const;
   //bool checkStage4Completion() const;
   //bool checkStage5Completion() const;
   //bool checkStage6Completion() const;
   //bool checkStage7Completion() const;
   //bool checkStage8Completion() const;
};
