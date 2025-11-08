#pragma once

#include <string>

class TextInputManager
{
  bool m_active = false;
  std::string m_text;

 public:
  void start();
  void stop();
  bool isActive() const;
  void clear();

  void addCharacter(char c);

  const std::string& getText() const;
};
