#include "SceneCombat.h"
#include <iostream>
#include <random>
#include <algorithm>
#include <cmath>

SceneCombat::SceneCombat(GameEngine* gameEngine, std::shared_ptr<Entity> player, std::shared_ptr<Entity> enemy)
   : Scene(gameEngine), m_playerEntity(player), m_enemyEntity(enemy)
{
   init();
}

void SceneCombat::init() //const std::string& config
{
   //todo read from file
   sLoadWordDatabase();
   sLoadSemanticRelations();

   std::cout << "words loaded\n.";

   //ui
   m_combatUIEntity = m_entities.addEntity("combat_ui");

   //start first round
   sGenerateWordChallenge();
   std::cout << "Combat scene initialised.\n";
}

void SceneCombat::update()
{
   if(!m_paused){
      sCombatUpdate();
   }
}

void SceneCombat::sCombatUpdate()
{
   //update timer
   if(m_combatState.timerActive && m_combatState.timer > 0)
   {
      m_combatState.timer -= m_game->getDeltaTime();

      if(m_combatState.timer == 0)
      {
         m_combatState.timer = 0;
         m_combatState.timerActive = false;

         //time's, player takes damage
         sApplyDamage(m_playerEntity, m_combatState.baseDamage);
         m_combatState.feedbackMessage = "Time's up! You take " + std::to_string(m_combatState.baseDamage) + " damage!";

         m_combatState.currentRound++;
         if(m_combatState.currentRound >= m_combatState.maxRounds)
         {
            sEndCombat(false);
         }
         else
         {
            sGenerateWordChallenge();
         }
      }
   }

   //check for combat end conditions
   if(m_playerEntity->hasComponent<CCombatant>())
   {
      auto& playerCombatant = m_playerEntity->getComponent<CCombatant>();
      if(playerCombatant.health <= 0)
      {
         sEndCombat(false);
         return;
      }
   }

   if(m_enemyEntity->hasComponent<CCombatant>())
   {
      auto& enemyCombatant = m_enemyEntity->getComponent<CCombatant>();
      if(enemyCombatant.health <= 0)
      {
         sEndCombat(true);
         return;
      }
   }
}

void SceneCombat::sDoAction(const Action& action)
{
   if(action.type() == "START")
   {
      if(action.name() == "PAUSE")
      {
         setPaused(!m_paused);
      }
      else if(m_combatState.showChainUI)
      {
         //chain input
         if(action.name() == "CHAIN_WORD_1")
         {
            auto words = sGetAvailableChainWords();
            if(words.size() > 0) sAddWordToChain(words[0]);
         }
         else if(action.name() == "CHAIN_WORD_2" && sGetAvailableChainWords().size() > 1)
         {
            auto words = sGetAvailableChainWords();
            sAddWordToChain(words[1]);
         }
         else if(action.name() == "CHAIN_WORD_3" && sGetAvailableChainWords().size() > 2)
         {
            auto words = sGetAvailableChainWords();
            sAddWordToChain(words[2]);
         }
         else if(action.name() == "REMOVE_LAST_WORD")
         {
            sRemoveLastWordFromChain();
         }
         else if(action.name() == "SUBMIT_CHAIN")
         {
            sSubmitChain();
         }
         else if(action.name() == "CANCEL_CHAIN")
         {
            m_combatState.showChainUI = false;
            m_combatState.showInput = true;
            m_combatState.semanticChain.clear();
            m_combatState.finalDamage = m_combatState.baseDamage;
         }
      }
      else if(action.name() == "START_CHAINING" && m_combatState.showInput)
      {
         sStartSemanticChaining();
      }
      else if(action.name() == "SUBMIT_ANSWER" && m_combatState.showInput)
      {
         sProcessPlayerAnswer();
      }
      else if(action.name() == "START_TYPING" && !m_combatState.showInput && !m_combatState.showChainUI)
      {
         m_combatState.showInput = true;
         startTyping();
      }
      else if(action.name() == "CANCEL_TYPING" && m_combatState.showInput)
      {
         m_combatState.showInput = false;
         stopTyping();
         inputClear();
      }
   }

   //handle text input
   if(m_isTyping && m_combatState.showInput)
   {
      if(action.type() == "TEXT_INPUT")
      {
         m_combatState.playerAnswer += action.name();
      }
   }
}

void SceneCombat::sGenerateWordChallenge()
{
   if(m_wordDatabase.empty()) return;

   std::random_device rd;
   std::mt19937 gen(rd());
   std::uniform_int_distribution<> dis(0, m_wordDatabase.size() - 1);

   //get random word
   auto it = m_wordDatabase.begin();
   std::advance(it, dis(gen));

   m_combatState.currentEnglishWord = it->first;
   m_combatState.currentNativeWord = it->second;
   m_combatState.baseDamage = 10 + (it->first.length()*2);
   m_combatState.finalDamage = m_combatState.baseDamage;

   //reset ui state
   m_combatState.playerAnswer.clear();
   m_combatState.feedbackMessage.clear();
   m_combatState.showInput = true;
   m_combatState.showChainUI = false;
   m_combatState.timer = m_combatState.maxTime;
   m_combatState.timerActive = true;
   m_combatState.semanticChain.clear();

   std::cout << "New challenege: " << m_combatState.currentEnglishWord << " -> " << m_combatState.currentNativeWord << std::endl;
}

void SceneCombat::sProcessPlayerAnswer()
{
   std::string normalizedAnswer = m_combatState.playerAnswer;
   std::transform(normalizedAnswer.begin(), normalizedAnswer.end(), normalizedAnswer.begin(), ::tolower);

   std::string correctAnswer = m_combatState.currentNativeWord;
   std::transform(correctAnswer.begin(), correctAnswer.end(), correctAnswer.begin(), ::tolower);

   bool isCorrect = (normalizedAnswer == correctAnswer);

   if(isCorrect)
   {
      sApplyDamage(m_enemyEntity, m_combatState.finalDamage);
      m_combatState.feedbackMessage = "Correct! Enemy takes " + std::to_string(m_combatState.finalDamage) + "damage!";
   }
   else
   {
      sApplyDamage(m_playerEntity, m_combatState.finalDamage);
      m_combatState.feedbackMessage = "Incorrect!, You take " + std::to_string(m_combatState.finalDamage) + " damage! Correct: " + m_combatState.currentNativeWord;
   }

   m_combatState.currentRound++;
   if(m_combatState.currentRound >= m_combatState.maxRounds)
   {
      sEndCombat(isCorrect);
   }
   else
   {
      sGenerateWordChallenge();
   }
}

void SceneCombat::sApplyDamage(std::shared_ptr<Entity> target, int damage)
{
   if(!target || !target->hasComponent<CCombatant>()) return;

   auto& combatant = target->getComponent<CCombatant>();
   combatant.health = std::max(0, combatant.health - damage);

   std::cout << target->tag() << " takes " << damage << " damage. Health: " << combatant.health << std::endl;
}

void SceneCombat::sEndCombat(bool playerWon)
{
   std::cout << "Combat ended. Player " << (playerWon ? "won" : "lost") << std::endl;

   //return to main scene
   m_game->changeScene("MAIN", nullptr, true);
}

//chaining implementation
void SceneCombat::sStartSemanticChaining()
{
   m_combatState.showChainUI = true;
   m_combatState.showInput = false;
}

void SceneCombat::sAddWordToChain(const std::string& word)
{
   if((!m_combatState.semanticChain.size()) >= m_combatState.maxChainLength)
   {
      m_combatState.feedbackMessage = "Chain is full! Maximum " + std::to_string(m_combatState.maxChainLength) + " words.";
      return;
   }

   if(!sPlayerKnowsWord(word))
   {
      m_combatState.feedbackMessage = "You don't know the word: " + word;
      return;
   }

   if(!sValidateSemanticChain(word))
   {
      m_combatState.feedbackMessage = word + " is not semantically related!";
      return;
   }

   m_combatState.semanticChain.push_back(word);
   sCalculateChainDamage();
}

void SceneCombat::sRemoveLastWordFromChain()
{
   if(!m_combatState.semanticChain.empty())
   {
      m_combatState.semanticChain.pop_back();
      sCalculateChainDamage();
   }
}

void SceneCombat::sSubmitChain()
{
   m_combatState.showChainUI = false;
   m_combatState.showInput = true;
}

void SceneCombat::sCalculateChainDamage()
{
   m_combatState.finalDamage = static_cast<int>(m_combatState.baseDamage * std::pow(m_combatState.chainMultiplier, m_combatState.semanticChain.size()));
}

std::vector<std::string> SceneCombat::sGetAvailableChainWords() const
{
   std::vector<std::string> availableWords;

   if(m_combatState.semanticChain.empty())
   {
      auto it = m_semanticRelations.find(m_combatState.currentNativeWord);
      if(it != m_semanticRelations.end())
      {
         for(const auto& word : it->second)
         {
            if(sPlayerKnowsWord(word))
            {
               availableWords.push_back(word);
            }
         }
      }
   }
   else
   {
      std::string lastWord = m_combatState.semanticChain.back();
      auto it = m_semanticRelations.find(lastWord);
      if(it != m_semanticRelations.end())
      {
         for(const auto& word : it->second)
         {
            if(sPlayerKnowsWord(word) && std::find(m_combatState.semanticChain.begin(), m_combatState.semanticChain.end(), word) == m_combatState.semanticChain.end())
            {
               availableWords.push_back(word);
            }
         }
      }
   }

   return availableWords;
}

bool SceneCombat::sValidateSemanticChain(const std::string& newWord) const
{
   if(m_combatState.semanticChain.empty())
   {
      auto it = m_semanticRelations.find(m_combatState.currentNativeWord);
      if(it != m_semanticRelations.end())
      {
         return std::find(it->second.begin(), it->second.end(), newWord) != it->second.end();
      }
   }
   else
   {
      std::string lastWord = m_combatState.semanticChain.back();
      auto it = m_semanticRelations.find(lastWord);
      if(it != m_semanticRelations.end())
      {
         return std::find(it->second.begin(), it->second.end(), newWord) != it->second.end();
      }
   }
   return false;
}

bool SceneCombat::sPlayerKnowsWord(const std::string& nativeWord) const
{
   if(!m_playerEntity->hasComponent<CPlayerDictionary>()) return false;
   const auto& dictionary = m_playerEntity->getComponent<CPlayerDictionary>();
   return dictionary.knownWords.find(nativeWord) != dictionary.knownWords.end();
}

void SceneCombat::sRender()
{
   m_game->window().clear(sf::Color(30, 30, 50)); //dark blue

   sRenderHealthBars();
   sRenderCombatUI();

   if(m_combatState.showChainUI)
   {
      sRenderChainUI();
   }

   m_game->window().display();
}

void SceneCombat::sRenderHealthBars()
{
   if(m_playerEntity->hasComponent<CCombatant>())
   {
      auto& player = m_playerEntity->getComponent<CCombatant>();

      sf::Text playerHealth;
      playerHealth.setFont(m_fontMain);
      playerHealth.setString("Player Health: " + std::to_string(player.health));
      playerHealth.setCharacterSize(24);
      playerHealth.setFillColor(sf::Color::Green);
      playerHealth.setPosition(50, 50);
      m_game->window().draw(playerHealth);
   }

   if(m_enemyEntity->hasComponent<CCombatant>())
   {
      auto& enemy = m_enemyEntity->getComponent<CCombatant>();

      sf::Text enemyHealth;
      enemyHealth.setFont(m_fontMain);
      enemyHealth.setString("Enemy Health: " + std::to_string(enemy.health));
      enemyHealth.setCharacterSize(24);
      enemyHealth.setFillColor(sf::Color::Red);
      enemyHealth.setPosition(50, 50);
      m_game->window().draw(enemyHealth);
   }
}

void SceneCombat::sRenderCombatUI()
{
   //render challenge
   sf::Text challengeText;
   challengeText.setFont(m_fontMain);
   challengeText.setString("Translate: " + m_combatState.currentEnglishWord);
   challengeText.setCharacterSize(36);
   challengeText.setFillColor(sf::Color::White);
   challengeText.setPosition(400, 200);
   m_game->window().draw(challengeText);

   //timer
   sf::Text timerText;
   timerText.setFont(m_fontMain);
   timerText.setString("Time: " + std::to_string(static_cast<int>(m_combatState.timer)));
   timerText.setCharacterSize(24);
   timerText.setFillColor(sf::Color::Yellow);
   timerText.setPosition(600, 150);
   m_game->window().draw(timerText);

   //playerInput
   if(m_combatState.showInput)
   {
      sf::Text inputText;
      inputText.setFont(m_fontMain);
      inputText.setString("Your answer: " + m_combatState.playerAnswer);
      inputText.setCharacterSize(24);
      inputText.setFillColor(sf::Color::Cyan);
      inputText.setPosition(400, 250);
      m_game->window().draw(inputText);
   }

   //feedback
   if(!m_combatState.feedbackMessage.empty())
   {
      sf::Text feedbackText;
      feedbackText.setFont(m_fontMain);
      feedbackText.setString(m_combatState.feedbackMessage);
      feedbackText.setCharacterSize(50);
      feedbackText.setFillColor(m_combatState.feedbackMessage.find("Correct") != std::string::npos ? sf::Color::Green : sf::Color::Red);
      feedbackText.setPosition(400, 300);
      m_game->window().draw(feedbackText);
   }

   //round info
   sf::Text roundText;
   roundText.setFont(m_fontMain);
   roundText.setString("Round: " + std::to_string(m_combatState.currentRound + 1) + "/" + std::to_string(m_combatState.maxRounds));
   roundText.setCharacterSize(20);
   roundText.setFillColor(sf::Color::White);
   roundText.setPosition(50, 150);
   m_game->window().draw(roundText);
}

void SceneCombat::sRenderChainUI()
{
   sf::RectangleShape background(sf::Vector2f(800, 400));
   background.setFillColor(sf::Color(50, 50, 80, 200));
   background.setPosition(240, 160);
   background.setOutlineColor(sf::Color::Yellow);
   background.setOutlineThickness(2.0f);
   m_game->window().draw(background);

   //chain title
   sf::Text titleText;
   titleText.setFont(m_fontMain);
   titleText.setString("SEMANTIC CHAINING");
   titleText.setCharacterSize(32);
   titleText.setFillColor(sf::Color::Yellow);
   titleText.setPosition(400, 180);
   m_game->window().draw(titleText);

   //current chain
   sf::Text chainText;
   chainText.setFont(m_fontMain);
   chainText.setString("Base: " + m_combatState.currentNativeWord + "\nChain: " + (m_combatState.semanticChain.empty() ? "None" : ""));
   chainText.setCharacterSize(24);
   chainText.setFillColor(sf::Color::White);
   chainText.setPosition(260, 230);
   m_game->window().draw(chainText);

   //Chain words
   if(!m_combatState.semanticChain.empty())
   {
      sf::Text wordsText;
      wordsText.setFont(m_fontMain);
      std::string wordsStr;
      for(size_t i = 0; i < m_combatState.semanticChain.size(); ++i)
      {
         if(i > 0) wordsStr += " -> ";
         wordsStr += m_combatState.semanticChain[i];
      }
      wordsText.setString(wordsStr);
      wordsText.setCharacterSize(20);
      wordsText.setFillColor(sf::Color::Green);
      wordsText.setPosition(260, 280);
      m_game->window().draw(wordsText);
   }

   //damage
   sf::Text damageText;
   damageText.setFont(m_fontMain);
   damageText.setString("Damage: " + std::to_string(m_combatState.finalDamage) + " (Base: " + std::to_string(m_combatState.baseDamage) + ")");
   damageText.setCharacterSize(20);
   damageText.setFillColor(sf::Color::Red);
   damageText.setPosition(260, 320);
   m_game->window().draw(damageText);

   //available words
}

void SceneCombat::sLoadWordDatabase()
{
}

void SceneCombat::sLoadSemanticRelations()
{
}
