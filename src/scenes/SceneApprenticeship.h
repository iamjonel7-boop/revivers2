#pragma once

#include "Actions.h"
#include "Entity.h"
#include "ManagerEntity.h"
#include "Scene.h"
#include "StructsDialogue.h"
#include "Components.h"
#include <SFML/Graphics.hpp>

class GameEngine;

class SceneApprenticeship : public Scene
{
protected:
   std::shared_ptr<Entity> m_playerEntity;

   std::string m_title;
   DialogueData m_dialogue;
   DialogueNode* m_currentNode = nullptr; //node being shown
   size_t m_selectedOption = 0; //selected option

   Gift stringToGift(const std::string& s);

   void init();
   void update();
   void sDoAction(const Action& action);
   void onEnd();

   void loadDialogue(const std::string& path);
   void registerApprenticeshipActions();

   void spawnPlayer();


public:
   SceneApprenticeship(GameEngine* gameEngine);
   virtual ~SceneApprenticeship();
   void sRender();
};

//make cleaning /unloading resources
