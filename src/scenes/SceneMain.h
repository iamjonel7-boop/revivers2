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
  sf::View m_mainView;

  int tileSize = 20;
  int mapWidth = 800;
  int mapHeight = 600;
  sf::RectangleShape m_tile;

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
