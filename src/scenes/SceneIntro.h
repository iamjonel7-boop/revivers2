#pragma once

#include "Scene.h"

using entity = std::shared_ptr<Entity>;

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

  entity m_player;

  sf::Font m_font;
  std::vector<std::string> m_introLines;
  entity m_introLine;
  std::vector<std::string> m_elderLines;
  entity m_elderLine;
  size_t m_currentLine;
  entity m_playerName;

  void init();
  void update() override;
  void sDoAction(const Action& action) override;
  void onEnd() override;

  void renderText(const std::string& entityTag);

 public:
  SceneIntro(GameEngine* gameEngine);
  void sRender() override;
};
