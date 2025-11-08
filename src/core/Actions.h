#pragma once

class Action
{
  std::string m_name = "none";
  std::string m_type = "none"; //start or end (released)

public:
  Action(const std::string& name, const std::string& type);

  const std::string& name() const;
  const std::string& type() const;
};
