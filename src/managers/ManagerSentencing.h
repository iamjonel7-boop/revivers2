#pragma once

#include <vector>
#include <memory>

//the player types the option he wants to select

class EntityManager;

class SentencingManager
{
  enum states
    {
      SELECTING_ENTITY,
      MARKING_ENTITY,
      SELECTING_VERB,
      SELECTING_ASPECT,
      SELECTING_VOICE,
      SELECTING_PERIOD
    };

  struct sentence
  {
    std::map<int, std::shared_ptr<Entity>> m_MarkedEntities;
    // mapped to index of the markers
    std::string verb; //the player typed the verb
  };

  std::shared_pointer<Entity> m_entity;
  std::vector<Entity> m_entities;

 public:
  SentencingManager()
  {
  }

  void displayEntities();
  void selectEntity();
  void selectMarker();
  void selectVerb();
  void selectVoice();
  void selectAspect();

};
