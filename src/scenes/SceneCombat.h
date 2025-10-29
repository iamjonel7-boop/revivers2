#pragma once

#include "Scene.h"
#include "GameEngine.h"
#include "ManagerEntity.h"
#include "Components.h"
#include <memory>
#include <map>

class SceneCombat : public Scene
{
protected:
   sf::Font m_fontMain;
   sf::Color m_colorMain;
   bool m_paused = false;
   void setPaused(bool paused) { m_paused = paused;}

   //entities
   std::shared_ptr<Entity> m_playerEntity;
   std::shared_ptr<Entity> m_enemyEntity;
   std::shared_ptr<Entity> m_combatUIEntity;

   //states
   struct CombatState
   {
      int currentRound = 0;
      int maxRounds = 3;
      float timer = 30.0f;
      float maxTime = 30.0f;
      bool timerActive = false;


      // word challenge
      std::string currentEnglishWord;
      std::string currentNativeWord;
      int baseDamage = 10;
      int finalDamage = 10;

      //semantic chaining
      std::vector<std::string> semanticChain;
      float chainMultiplier = 1.2f;
      int maxChainLength = 5;

      //ui state
      std::string playerAnswer;
      std::string feedbackMessage;
      bool showInput = false;
      bool showChainUI =  false;
   };

   CombatState m_combatState;

   //word databases, make a file for this
   std::map<std::string, std::string> m_wordDatabase;
   std::map<std::string, std::vector<std::string>> m_semanticRelations;

   void init();//const std::string& config
   void update();
   void sDoAction(const Action& action);
   void onEnd();

   //combat system
   void sCombatUpdate();
   void sGenerateWordChallenge();
   void sProcessPlayerAnswer();
   void sApplyDamage(std::shared_ptr<Entity> target, int damage);
   void sEndCombat(bool playerWon);

   //chaining system
   void sStartSemanticChaining();
   void sAddWordToChain(const std::string& word);
   void sRemoveLastWordFromChain();
   void sSubmitChain();
   void sCalculateChainDamage();
   std::vector<std::string> sGetAvailableChainWords() const;
   bool sValidateSemanticChain(const std::string& newWord) const;
   bool sPlayerKnowsWord(const std::string& nativeWord) const;

   //rendering
   void sRenderCombatUI();
   void sRenderChainUI();
   void sRenderHealthBars();

   //helper
   void sLoadWordDatabase();
   void sLoadSemanticRelations();

public:
   SceneCombat(GameEngine* gameEngine, std::shared_ptr<Entity> player, std::shared_ptr<Entity> enemy);
   void sRender();
};
