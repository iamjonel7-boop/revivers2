#include "Scene.h"
#include <iostream>

class DummyScene : public Scene
{
public:
  using Scene::Scene;
  void update() override { std::cout << "Dummy Scene\n" }
  void sRender() override {}
};
