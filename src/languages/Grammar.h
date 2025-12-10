#pragma once

#include <string>
#include <vector>
#include <map>

enum class Aspect
  {
    PERFECTIVE,
    IMPERFECTIVE,
    CONTEMPLATIVE
  };

enum class Voice
  {
    ACTOR,
    PATIENT,
    LOCATIVE
  };

enum class Case
  {
    DIRECT,
    INDIRECT,
    OBLIQUE
  };

class Grammar
{
public:
  std::string m_language;
  std::map<std::string, Aspect> m_aspects;
  std::map<std::string, Voice> m_voices;
  std::map<std::string, Case> m_cases;

  //return the address of name but it should be read only
  const std::string& getLanguageName() const
  {
    return m_language;
  }

  virtual void printAspects() = 0;
  virtual void printVoices() = 0;
  virtual void printCases() = 0;

};
