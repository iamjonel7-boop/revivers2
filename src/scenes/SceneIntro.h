#pragma once

#include "Scene.h"

using entity = std::shared_ptr<Entity>;

class SceneIntro : public Scene
{
 protected:
  entity m_player;

  sf::Font m_elderFont;
  std::vector<std::string> m_elderWords;
  entity m_elderDialogue;
  size_t m_currentDialogue;

  void init();
  void update() override;
  void sDoAction(const Action& action) override;
  void onEnd() override;

 public:
  SceneIntro(GameEngine* gameEngine);
  void sRender() override;
};
