#pragma once

#include "ManagerEntity.h"
#include <string>
#include <map>
#include <vector>
#include <memory>
#include <random>
#include <nlohmann/json.hpp>
#include <unordered_set>

using json = nlohmann::json;

class LexiconManager
{
public:
   enum class WordType
   {
      NOUN,
      VERB,
      ADJECTIVE,
      PHRASE,
      BUILDING,
      PROFESSION
   };

   enum class VerbCategory
   {
      MOVEMENT,
      SOCIAL,
      COMBAT,
      ESPIONAGE,
      RESOURCE
   };

   enum class GameSystem
   {
      MOVEMENT_SYSTEM,
      RECRUITMENT_SYSTEM,
      COMBAT_SYSTEM,
      RESOURCE_SYSTEM,
      BUILDING_SYSTEM,
      ESPIONAGE_SYSTEM,
      DIALOGUE_SYSTEM,
      CRAFTING_SYSTEM,
      HEALING_SYSTEM,
      STEALTH_SYSTEM
   };

   struct SystemTrigger
   {
      GameSystem system;
      std::map<std::string, std::string> parameters;
      int priority = 0; //order of execution if multiple systems
      SystemTrigger(GameSystem sys, const std::map<std::string, std::string>& params = {}, int prio = 0)
         : system(sys), parameters(params), priority(prio)
      {
      }
   };

   struct WordData
   {
      std::string native;
      std::string english;
      WordType type;
      int baseDamage = 0;
      float duration = 0.0f; //for verbs with imperfective aspect
      int insightCost = 0; //cost to learn
      int prestigeValue = 0; //prestige gained when learnt
      std::vector<std::string> synonyms;
      std::vector<std::string> relatedWords; //for semantic chaining
      std::string category;
      int difficulty = 1;

      //verb specific properties
      VerbCategory verbCategory = VerbCategory::MOVEMENT;
      std::vector<std::string> requiredArguments;

      bool isTransitive = false;

      //building specific prop
      int defenseBonus = 0; //defense when building is claimed
      std::vector<std::string> unlockableWords; //words unlocked when building claimed

      std::vector<SystemTrigger> systemTriggers;


   };

private:

   std::map<std::string, WordData> m_gameLexicon; //all game words
   std::map<std::string, WordData> m_playerDictionary; //player's learned words

   std::unordered_set<std::string> m_unlockedSystems; //systems player can use

  //system registry
   std::map<GameSystem, std::function<void(
   const std::string& verb,
   const std::string& aspect,
   const std::map<std::string, std::string>& params,
   std::shared_ptr<Entity> focus,
   std::shared_ptr<Entity> nonFocus)>>  m_systemCallbacks;

   //filepaths
   std::string m_gameLexiconPath;
   std::string m_playerDictionaryPath;

   //random num gen
   std::random_device m_rd;
   std::mt19937 m_gen;

   void loadGameLexicon();
   void loadPlayerDictionary();
   void savePlayerDictionary();
   void initializeSemanticChains();

   //system trigger parsing
   SystemTrigger parseSystemTrigger(const json& triggerData);
   std::vector<SystemTrigger> parseSystemTriggers(const json& triggersArray);

public:
   LexiconManager(const std::string& gameLexiconPath, const std::string& playerDictPath);

   //word access and management
   const WordData* getWordData(const std::string& nativeWord) const;
   bool playerKnowsWord(const std::string& nativeWord) const;
   bool addWordToPlayerDictionary(const std::string& nativeWord);
   void removeWordFromPlayerDictionary(const std::string& nativeWord);

   std::vector<SystemTrigger> getSystemTriggers(const std::string& verb, const std::string& aspect = "") const;

   //word lookup by properties
   std::string getSystemForVerb(const std::string& verb) const;
   bool verbExists(const std::string& verb) const;

   //random word selection
   std::string getRandomEnglishWord(int maxDifficulty = 3) const;
   std::string getRandomWordbyType(WordType type, int maxDifficulty = 3) const;
   std::string getRandomWordbyCategory(const std::string& category, int maxDifficulty = 3) const;
   std::vector<std::string> getRandomWords(int count, int maxDifficulty = 3) const;

   //building and combat util
   std::vector<std::string> getBuildingNames() const;
   std::vector<std::string> getMovementVerbs() const;
   std::vector<std::string> getCombatWords(int maxDifficulty = 3) const;
   std::vector<std::string> getAnagramCandidates(int wordLength = 6) const;

   //semantic chaining
   std::vector<std::string> getRelatedWords(const std::string& baseWord) const;
   bool areWordsRelated(const std::string& word1, const std::string& word2) const;

   //dict management
   const std::map<std::string, WordData>& getPlayerDictionary() const
   {
      return m_playerDictionary;
   }
   int getPlayerVocabularySize() const
   {
      return m_playerDictionary.size();
   }
   void clearPlayerDictionary();

   //search and filtering
   std::vector<WordData> searchWords(const std::string& query, WordType type = WordType::NOUN) const;
   std::vector<std::string> getWordsByVerbCategory(VerbCategory category) const;

   //analytics
   int calculatePlayerPrestige() const;
   std::map<WordType, int> getVocabularyBreakdown() const;
};
