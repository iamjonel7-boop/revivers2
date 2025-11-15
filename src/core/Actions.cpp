#include "Actions.h"
#include <sstream>

Action::Action()
        : m_name("none") //moveLeft, right, up, etc
        , m_type("none") //START or END
{
}

Action::Action(const std::string& name, const std::string& type)
        : m_name(name)
        , m_type(type)
{
}

const std::string& Action::name() const
{
  return m_name;
}

const std::string& Action::type() const
{
  return m_type;
}
