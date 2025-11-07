#pragma once

#include "Grammar.h"
#include <iostream>
#include <map>

class BicolGrammar : public Grammar
{
  enum class NominalMarkers
    {
      //direct
      SI, AN,
      //indirect
      NI, KAN,
      //oblique
      SA, KI
    };

  /*  enum PronominalMarkers
    {
      //direct
      AKO, IKA, SIYA,
      //indirect
      KO, MO, NIYA,
      //oblique
      SAKO, SAIMO, SAIYA
    };
  */

  std::map<Cases, std::map<std::string, NominalMarkers>> m_NomMarkers;

public:
  BicolGrammar()
  {
    m_language = "Bicol";
    m_aspects = {
      {"imperfective", Aspects::IMPERFECTIVE},
      {"perfective", Aspects::PERFECTIVE},
      {"contemplative", Aspects::CONTEMPLATIVE}
    };
    m_voices = {
      {"actor", Voices::ACTOR},
      {"patient", Voices::PATIENT},
      {"voices", Voices::LOCATIVE}
    };
    m_cases = {
      {"direct", Cases::DIRECT},
      {"indirect", Cases::INDIRECT},
      {"oblique", Cases::OBLIQUE}
    };
    m_NomMarkers = {
      {Cases::DIRECT, {{"si", NominalMarkers::SI}, {"an", NominalMarkers::AN}}},
      {Cases::INDIRECT, {{"ni", NominalMarkers::NI}, {"kan", NominalMarkers::KAN}}},
      {Cases::INDIRECT, {{"sa", NominalMarkers::SA}, {"ki", NominalMarkers::KI}}}
    };
  }

  void printAspects()
  {
    for(const auto& [key, value]: m_aspects)
      {
        std::cout << key << std::endl;
      }
  }

  void printVoices()
  {
    for(const auto& [key, value] : m_voices)
      {
        std::cout << key << std::endl;
      }
  }

  void printCases()
  {
    for(const auto& [key, value] : m_cases)
      {
        std::cout << key << std::endl;
      }
  }

  void printNomMarkers()
  {
    for(const auto& [caseKey, markers] : m_NomMarkers)
      {
        for(const auto& [markerKey, value] : markers)
          {
            std::cout << markerKey << std::endl;
          }
      }
  }
};
