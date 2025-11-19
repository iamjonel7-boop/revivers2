#pragma once

#include "Scene.h"
#include <memory>
#include <map>
#include <vector>

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

  std::shared_ptr<Entity> m_cursorEntity;
  sf::View m_mainView;

  std::shared_ptr<Entity> m_tile;

  std::vector<std::shared_ptr<Entity>> m_buildings;

  enum class MapControlState
    {
      NAVIGATING,
      SENTENCING
    };

  MapControlState m_controlState;

  void showMapGrid();

  void init();
  void update() override;
  void sDoAction(const Action& action) override;
  void onEnd() override;

  void makeBuildings();
  void renderTileGrid();

public:
  SceneMain(GameEngine* gameEngine);
  void sRender() override;
};
