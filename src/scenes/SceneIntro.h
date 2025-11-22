#pragma once

#include "Scene.h"

class SceneIntro : public Scene
{
 protected:
  enum class ActionName
    {
      NEXT,
      BACK,
      ENTER
    };

  enum class State
    {
      INTRO_DIALOGUE,
      ELDER_DIALOGUE,
      NAME_INPUT
    };

  State m_state;

  std::shared_ptr<Entity> m_player;
  sf::Text m_playerName;

  sf::Font m_font;
  std::vector<std::string> m_introLines;
  std::shared_ptr<Entity> m_introLine;

  std::vector<std::string> m_elderLines;
  std::shared_ptr<Entity> m_elderLine;
  size_t m_currentLine;

  void init();
  void onEnd() override;
  void readFile(std::string& filename);

  void sDoAction(const Action& action) override;
  void handleIntroDialogue(const Action& action, ActionName actionName);
  void handleElderDialogue(const Action& action, ActionName actionName);
  void handleNameInput(const Action& action, ActionName actionName, std::string& playerName);

  void update() override;

  void renderText(const std::string& entityTag);
  void renderPlayerName();

 public:
  SceneIntro(GameEngine* gameEngine);
  void sRender() override;
};
