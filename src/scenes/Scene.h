#pragma once

#include "ManagerEntity.h"
#include "ManagerTextInput.h"
#include "Actions.h"
#include <memory>
#include <map>

class GameEngine;

typedef std::map<int, std::string> ActionMap;

class Scene
{
protected:
   GameEngine* m_game = nullptr;

   //managers
   EntityManager m_entities;
   std::unique_ptr<SystemManager> m_systemManager;
   std::unique_ptr<TurnManager> m_turnManager;
   std::unique_ptr<StageManager> m_stageManager;
   std::unique_ptr<LexiconManager> m_lexiconManager;
   std::unique_ptr<ActionFormationManager> m_actionFormationManager;
   std::unique_ptr<ActionResolutionManager> m_actionResolutionManager;

   ActionMap m_actionMap;
   bool m_hasEnded = false;
   size_t m_currentFrame = 0;

   bool m_isTyping = false;
   std::string m_inputBuffer;
   std::vector<std::string> m_commands;

   virtual void onEnd() = 0;
   virtual void initializeManagers() = 0;

public:
   Scene();
   Scene(GameEngine* gameEngine);
   virtual ~Scene() = default;

   //core scene funtionality
   virtual void doAction(const Action& action);
   virtual void update() = 0;
   virtual void sDoAction(const Action& action) = 0;
   virtual void sRender() = 0;

   //simulation and action management
   void simulate(const size_t frames);
   void registerAction(int inputKey, const std::string& actionName);
   const ActionMap& getActionMap() const;

   //input handling
   bool isTyping() const;
   void startTyping();
   void stopTyping();
   void appendInputChar(char c);
   void removeLastInputChar();
   void commitInput();
   void inputClear();
   const std::string& getCurrentInput() const;
   const std::vector<std::string>& getCommands() const;

   //utility methods
   size_t width() const;
   size_t height() const;
   size_t currentFrame() const;
   bool hasEnded() const;
   void drawLine(const sf::Vector2f p1, const sf::Vector2f p2);

   //manager accessors
   EntityManager& getEntityManager();
   TurnManager* getTurnManager();
   StageManager* getStageManager();
   SystemManager* getSystemManager();
   ActionFormationManager* getActionFormationManager();
   ActionResolutionManager* getActionResolutionManager();
   GrammarManager& getGrammarManager();
   LexiconManager& getLexiconManager();

   //manager init helpers
   void initializeDefaultManagers();
   void initializeDefaultTurnManager();
   void initializeDefaultStageManager();
   void initializeDefaultSystemManager();
   void initializeDefaultLexiconManager();
   void initializeActionManagers();

   //common entity creation helpers
   std::shared_ptr<Entity> createPlayerEntity(const std::string& name = "Player");
   std::shared_ptr<Entity> createCursorEntity();
   std::shared_ptr<Entity> createTextEntity(const std::string& text, const sf::Font& font, unsigned size, const sf::Vector2f& position, const sf::Color& color, const std::string& tag = "text_ui");
};
