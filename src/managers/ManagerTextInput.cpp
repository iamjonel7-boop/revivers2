#include "ManagerTextInput.h"
#include <iostream>

void TextInputManager::start()
{
  clear();
  m_active = true;
  std::cout << "textinputmanager turned active." << std::endl;
}

void TextInputManager::stop()
{
  m_active = false;
  std::cout << "textinputmanager turned inactive." << std::endl;
}

void TextInputManager::clear()
{
  m_text.clear();
  std::cout << "textinputmanager cleared buffer." << std::endl;
  std::cout << "text: " << m_text << std::endl;
}

bool TextInputManager::isActive() const
{
  std::cout << "textinputmanager is active." << std::endl;
  return m_active;
}

void TextInputManager::addCharacter(char c)
{
  if(!m_active) return;

  if(c == '\b')
    {
      if(!m_text.empty())
        m_text.pop_back();
      std::cout << c << " deleted." << std::endl;
    }
  else if(c == '\r' || c == '\n')
    {
      stop();
      std::cout << "final text: " << m_text << std::endl;
    }
  else if(c >= 32 && c < 127)
    {
      m_text += c;
      std::cout << "current text processed: " << m_text << std::endl;
    }
}

const std::string& TextInputManager::getText() const
{
  return m_text;
}
