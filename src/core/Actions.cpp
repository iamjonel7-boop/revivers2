#include "Actions.h"
#include <sstream>

Action::Action()
  : m_type(ActionType::NONE) //START or END

  , m_name(-1) //moveLeft, right, up, etc
{
}

Action::Action(int name, ActionType type)
  : m_name(name)
  , m_type(type)
{
}

int Action::name() const
{
  return m_name;
}

ActionType Action::type() const
{
  return m_type;
}
