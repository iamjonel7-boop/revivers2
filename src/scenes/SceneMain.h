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
