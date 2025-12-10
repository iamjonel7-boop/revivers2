#include "GrammarBicol.h"

BicolGrammar::BicolGrammar()
{
		m_language = "Bicol";
		m_aspects = {
				{"imperfective", Aspect::IMPERFECTIVE},
				{"perfective", Aspect::PERFECTIVE},
				{"contemplative", Aspect::CONTEMPLATIVE}
		};
		m_voices = {
				{"actor", Voice::ACTOR},
				{"patient", Voice::PATIENT},
				{"locative", Voice::LOCATIVE}
		};
		m_cases = {
				{"direct", Case::DIRECT},
				{"indirect", Case::INDIRECT},
				{"oblique", Case::OBLIQUE}
		};
		m_nomMarkers = {
				{Case::DIRECT, {{"si", NominalMarker::SI}, {"an", NominalMarker::AN}}},
				{Case::INDIRECT, {{"ni", NominalMarker::NI}, {"kan", NominalMarker::KAN}}},
				{Case::INDIRECT, {{"sa", NominalMarker::SA}, {"ki", NominalMarker::KI}}}
		};
		m_proNomMarkers = {
				{Case::DIRECT, {{"ako", PronominalMarker::AKO},
								 {"ika", PronominalMarker::IKA},
								 {"SIYA", PronominalMarker::SIYA}}},
				{Case::INDIRECT, {{"ko", PronominalMarker::KO},
								   {"mo", PronominalMarker::MO},
								   {"niya", PronominalMarker::NIYA}}},
				{Case::OBLIQUE, {{"sako", PronominalMarker::SAKO},
								  {"saimo", PronominalMarker::SAIMO},
								  {"saiya", PronominalMarker::SAIYA}}}
		};
}

void BicolGrammar::printAspects()
{
		for(const auto& [key, value]: m_aspects)
		{
				std::cout << key << std::endl;
		}
}

void BicolGrammar::printVoices()
{
		for(const auto& [key, value] : m_voices)
		{
				std::cout << key << std::endl;
		}
}

void BicolGrammar::printCases()
{
		for(const auto& [key, value] : m_cases)
		{
				std::cout << key << std::endl;
		}
}

void BicolGrammar::printNomMarkers()
{
		for(const auto& [caseKey, markers] : m_nomMarkers)
		{
				for(const auto& [markerKey, value] : markers)
				{
						std::cout << markerKey << std::endl;
				}
		}
}

std::vector<std::string> BicolGrammar::getVoicesAsStrings() const
{
		std::vector<std::string> voices;
		for(const auto& [key, value] : m_voices)
		{
				voices.push_back(key);
		}
		return voices;
}

std::string BicolGrammar::conjugateVerb(const std::string& verb, Voice voice) const
{
    std::string conjugated = verb;

	if (verb.empty()) {
			return conjugated;
	}

    switch(voice)
    {
        case Voice::ACTOR:
				conjugated = "nag" + verb;
            break;
        case Voice::PATIENT:
				conjugated = std::string(1, verb[0]) + "in" + verb.substr(1);
				break;
        case Voice::LOCATIVE:
				conjugated = std::string(1, verb[0]) + "in" + verb.substr(1) + "an";
				break;
    }
    return conjugated;
}

std::string BicolGrammar::applyVerbAffix(const std::string& verb, Voice voice) const
{
		return conjugateVerb(verb, voice);
}
