#pragma once

#include <string>
#include <vector>
#include <map>

enum class Aspects
  {
    PERFECTIVE,
    IMPERFECTIVE,
    CONTEMPLATIVE
  };

enum class Voices
  {
    ACTOR,
    PATIENT,
    LOCATIVE
  };

enum class Cases
  {
    DIRECT,
    INDIRECT,
    OBLIQUE
  };

class Grammar
{
 protected:
  std::string m_language;
  std::map<std::string, Aspects> m_aspects;
  std::map<std::string, Voices> m_voices;
  std::map<std::string, Cases> m_cases;

 public:
  //return the address of name but it should be read only
  const std::string& getLanguageName() const
  {
    return m_language;
  }

  virtual void printAspects() = 0;
  virtual void printVoices() = 0;
  virtual void printCases() = 0;

};
