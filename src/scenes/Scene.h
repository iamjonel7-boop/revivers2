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

  EntityManager m_entities;
  TextInputManager m_textInputManger;

  ActionMap m_actionMap;

  bool m_hasEnded = false;
  size_t m_currentFrame = 0;

  virtual void onEnd() = 0;

public:
  Scene();
  Scene(GameEngine* gameEngine);
  virtual ~Scene() = default;

  //core scene funtionality
  virtual void doAction(const Action& action);
  virtual void update() = 0;
  virtual void sDoAction(const Action& action) = 0;
  virtual void sRender() = 0;

  void simulate(const size_t frames);
  void registerAction(int inputKey, const std::string& actionName);

  const auto& getActionMap() const;
  const auto& getTextInputManager() const;

  //manager accessors
  EntityManager& getEntityManager();
};
