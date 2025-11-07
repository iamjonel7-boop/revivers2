#pragma once

#include "Scene.h"
#include <memory>
#include <map>
#include <vector>

using entity = std::shared_ptr<Entity>;

class SceneMain : public Scene
{
protected:
  entity m_cursorEntity;

  enum class MapControlState
    {
      NAVIGATING,
      SENTENCING
    };

  MapControlState m_controlState;

  void createMap();

  void init();
  void update();
  void sDoAction(const Action& action);
  void onEnd();

public:
  SceneMain(GameEngine* gameEngine);
  void sRender() override;
};
