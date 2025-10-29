#include "Entity.h"
#include "Vec2.h"


bool Entity::isDead() const
{
  return !(m_active);
}

size_t Entity::id() const
{
  return m_id;
}

const std::string& Entity::tag() const
{
  return m_tag;
}

void Entity::destroy()
{
  m_active = false;
}

