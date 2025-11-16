#pragma once

#include "Scene.h"
#include <memory>
#include <map>
#include <vector>

using entity = std::shared_ptr<Entity>;

class SceneMain : public Scene
{
protected:
  enum class ActionName
    {
      MOVE_UP,
      MOVE_DOWN,
      MOVE_LEFT,
      MOVE_RIGHT,
      SELECT_TILE
    };

  entity m_cursorEntity;
  sf::View m_mainView;

  entity m_tile;
  entity m_topRect; //stats at the top
  entity m_building;

  std::vector<entity> m_buildings;

  enum class MapControlState
    {
      NAVIGATING,
      SENTENCING
    };

  MapControlState m_controlState;

  void createMap();

  void init();
  void update() override;
  void sDoAction(const Action& action) override;
  void onEnd() override;

public:
  SceneMain(GameEngine* gameEngine);
  void sRender() override;
};
