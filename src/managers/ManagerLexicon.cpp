#include "ManagerLexicon.h"
#include <iostream>
#include <fstream>
#include <sstream>

const std::string LexiconManager::PLAYER_LEXICON_PATH = {"../file/playerLexicon.txt"};
const std::string LexiconManager::GAME_LEXICON_PATH = {"../file/bicolLexicon.txt"};

LexiconManager::LexiconManager()
{
		init();
}

void LexiconManager::init()
{
		m_readFile(GAME_LEXICON_PATH);
}

void LexiconManager::m_addWord(const std::string& nativeName, const std::string& imperialName, const std::string& wordClass)
{
		std::ofstream playerLexicon(PLAYER_LEXICON_PATH, std::ios::app);

		playerLexicon << nativeName << " "
					  << wordClass << " "
					  << 0 << " "
					  << imperialName << std::endl;

		playerLexicon.close();
}

uint8_t LexiconManager::m_parseWordClass(const std::string& wordclass)
{
		if(wordclass == "noun")
				return static_cast<uint8_t>(WordClass::NOUN);
		else if(wordclass == "adjective")
				return static_cast<uint8_t>(WordClass::ADJECTIVE);
		else
				return static_cast<uint8_t>(WordClass::VERB);
}

void LexiconManager::m_readFile(const std::string& filename)
{
		std::fstream fileIn(filename, std::ios::in);
		std::string line;

		std::cout << "Opening lexicon file." << std::endl;

		if(!fileIn.is_open())
		{
				std::cerr << "Could not open lexicon file." << std::endl;
		}

		int lineCount = 0;
		int wordCount = 0;

		while(std::getline(fileIn, line))
		{
				lineCount++;
				std::istringstream iss(line);
				Word w;
				std::string wordClass;

				if(line.empty())
						continue;

				if(!(iss >> w.m_nativeWord
					 >> wordClass
					 >> w.m_appeal
					 >> w.m_imperialWord))
				{
						std::cout << "Failed to parse line: " << line << std::endl;
						continue;
				}

				w.m_class = m_parseWordClass(wordClass);

				std::string verbSystem;
				while(iss >> verbSystem)
				{
						w.m_verbSystems.push_back(verbSystem);
				}

				wordCount++;

				switch(w.m_class)
				{
				case static_cast<uint8_t>(WordClass::NOUN):
						m_nouns.insert({w.m_nativeWord, w});
						std::cout << "Added noun: " << w.m_nativeWord << std::endl;
						break;
				case static_cast<uint8_t>(WordClass::VERB):
						m_verbs.insert({w.m_nativeWord, w});
						std::cout << "Added verb: " << w.m_nativeWord << std::endl;
						break;
				case static_cast<uint8_t>(WordClass::ADJECTIVE):
						m_adjectives.insert({w.m_nativeWord, w});
						std::cout << "Added adjective: " << w.m_nativeWord << std::endl;
						break;
				}
		}

		std::cout << "Finished reading lexicon." << std::endl
				  << "Total lines = " << lineCount << ".\n"
				  << "Total words = " << wordCount << "." << std::endl;
		std::cout << "Noun count: " << m_nouns.size() << std::endl
				  << "Verb count: " << m_verbs.size() << std::endl
				  << "Adjective count: " << m_adjectives.size() << std::endl;
}

const std::unordered_map<std::string, LexiconManager::Word>& LexiconManager::getNouns() const
{
  return m_nouns;
}

const std::unordered_map<std::string, LexiconManager::Word>& LexiconManager::getVerbs() const
{
  return m_verbs;
}

const std::unordered_map<std::string, LexiconManager::Word>& LexiconManager::getAdjectives() const
{
  return m_adjectives;
}
