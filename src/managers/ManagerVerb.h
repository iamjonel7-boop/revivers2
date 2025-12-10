#pragma once

#include "GrammarBicol.h"
#include "Scene.h"
#include "Components.h"
#include "Entity.h"
#include <string>
#include "GameEngine.h"

class BicolGrammar;

class VerbManager
{
		GameEngine* m_game;
public:
		VerbManager(GameEngine* game);

		struct SentenceValidator
		{
				bool isValid = false;
				std::string errorMessage;
		};

		SentenceValidator  validateSentence(const BicolGrammar::Sentence& sentence);
		void executeSentence(const BicolGrammar::Sentence& sentence);

		using VerbAction = void(VerbManager::*)(const BicolGrammar::Sentence& sentence);
		std::unordered_map<std::string, VerbAction> m_verbActionMap;

		void initializeVerbActions();

		void verb_laog(const BicolGrammar::Sentence& sentence);
		void verb_kaulay(const BicolGrammar::Sentence& sentence);

		bool isPlayerInSentence(const BicolGrammar::Sentence& sentence);
};
