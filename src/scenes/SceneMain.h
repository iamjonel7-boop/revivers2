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
      SELECT_TILE,
      OPEN_INVENTORY
    };

  enum class MapControlState
    {
      NAVIGATING,
      SENTENCING,
    };

  sf::View m_mainView;

  std::shared_ptr<Entity> m_cursorEntity;
  std::shared_ptr<Entity> m_tileOutlineShape;

  MapControlState m_controlState;

  void init();
  void update() override;
  void sDoAction(const Action& action) override;
  void onEnd() override;

  void handleCursorNavigation(const Action& action, ActionName act, CInput& cinput);
  void handleSentencing();

  void makeMapView();
  void makeMapGrid();
  void makeCursor();
  void makeBuildings();
  void makePaths();

  void updateCursorPos();

  void renderTileGrid();
  void renderCursor();
  void renderBuildings();
  void renderPaths();

public:
  SceneMain(GameEngine* gameEngine);
  void sRender() override;
};
