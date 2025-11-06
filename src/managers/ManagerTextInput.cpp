#include "ManagerTextInput.h"

using TIM = TextInputManager;

void TIM::start()
{
  m_active = true;
}

void TIM::stop()
{
  m_active = false;
}

void TIM::clear()
{
  m_text.clear();
}

bool TIM::isActive() const
{
  return m_active;
}

void TIM::addCharacter(char c)
{
  if(!m_active) return;

  if(c == '\b')
    {
      if(!m_text.empty())
        m_text.pop_back();
    }
  else if(c == '\r' || c == '\n')
    {
      stop();
    }
  else if(c >= 32 && c < 127)
    {
      m_text += c;
    }
}

const std::string& TIM::getText() const
{
  return m_text;
}
