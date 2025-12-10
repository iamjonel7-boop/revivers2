#pragma once

#include "Grammar.h"
#include <iostream>
#include <map>
#include <Entity.h>

class BicolGrammar : public Grammar
{
public:
		BicolGrammar();

		enum class NominalMarker
		{
				//direct
				SI, AN,
				//indirect
				NI, KAN,
				//oblique
				SA, KI
		};

		enum PronominalMarker
		{
				//direct
				AKO, IKA, SIYA,
				//indirect
				KO, MO, NIYA,
				//oblique
				SAKO, SAIMO, SAIYA
		};

		struct SentenceElement
		{
				std::string marker;
				std::shared_ptr<Entity> entity;
				std::string entityName;
				Case caseType;
				bool isPronoun = false;
				std::string pronounForm;
		};

		struct Sentence
		{
				SentenceElement firstArgument;
				std::string verb; //base
				std::string conjugatedVerb; //with voice only for now
				Voice voice;
				SentenceElement secondArgument;
				bool hasFirstArgument = false;
				bool hasVerb = false;
				bool hasVoice = false;
				bool hasSecondArgument = false;

				void clear()
						{
								firstArgument = SentenceElement();
								verb = "";
								conjugatedVerb = "";
								secondArgument = SentenceElement();
								hasFirstArgument = false;
								hasVerb = false;
								hasVoice = false;
								hasSecondArgument = false;
						}
		};

		std::map<Case, std::map<std::string, NominalMarker>> m_nomMarkers;
		std::map<Case, std::map<std::string, PronominalMarker>> m_proNomMarkers;

		std::vector<std::string> getVoicesAsStrings() const;
		std::string conjugateVerb(const std::string& verb, Voice voice) const;
		std::string applyVerbAffix(const std::string& verb, Voice voice) const;

		void printAspects();
		void printVoices();
		void printCases();
		void printNomMarkers();
};
