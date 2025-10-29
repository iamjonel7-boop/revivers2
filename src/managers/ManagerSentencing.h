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
    std::map<std::shared_ptr<Entity>, int> m_MarkedEntities;
    // mapped to index of the markers
    int
  };

  std::shared_pointer<Entity> m_entity;
  std::vector<Entity> m_entities;

 public:

};
