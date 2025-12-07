#pragma once

#include <vector>
#include <string>
#include <unordered_map>

class LexiconManager
{
		static const std::string PLAYER_LEXICON_PATH;
		static const std::string GAME_LEXICON_PATH;

public:
		struct Word
		{
				std::string m_nativeWord;
				std::string m_imperialWord;
				uint8_t m_class;
				float m_appeal;
				std::vector<std::string> m_verbSystems; //the logics that will be called for verbs
		};

		enum class WordClass : uint8_t
		{
				NOUN,
				VERB,
				ADJECTIVE
		};

protected:
		std::unordered_map<std::string, Word> m_nouns;
		std::unordered_map<std::string, Word> m_verbs;
		std::unordered_map<std::string, Word> m_adjectives;

		void init();

		void m_readFile(const std::string& filename);
		uint8_t m_parseWordClass(const std::string& wordClass);

 public:
  LexiconManager();
  const std::unordered_map<std::string, Word>& getNouns() const;
  const std::unordered_map<std::string, Word>& getVerbs() const;
  const std::unordered_map<std::string, Word>& getAdjectives() const;

  void m_addWord(const std::string& nativeName, const std::string& imperialName, const std::string& wordClass);
};
