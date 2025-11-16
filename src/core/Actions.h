#pragma once

#include <string>

enum class ActionType
  {
    NONE,
    START, //when key pressed
    END, //when key released
  };

class Action
{
  ActionType m_type;
  int m_name;
  //std::string m_name = "none";
  //std::string m_type = "none"; //start or end (released)

public:
  Action();
  Action(int name, ActionType type);

  int name() const;
  ActionType type() const;
};
