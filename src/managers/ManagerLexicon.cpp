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

const std::vector<std::string> LexiconManager::getNouns() const
{
		std::vector<std::string> nouns;
		for(const auto& [key, value] : m_nouns)
		{
				nouns.push_back(key);
		}
		return nouns;
}

const std::vector<std::string> LexiconManager::getVerbs() const
{
		std::vector<std::string> verbs;
		for(const auto& [key, value] : m_verbs)
		{
				verbs.push_back(key);
		}
		return verbs;
}

const std::vector<std::string> LexiconManager::getAdjectives() const
{
		std::vector<std::string> adjectives;
		for(const auto& [key, value] : m_adjectives)
				adjectives.push_back(key);
		return adjectives;
}

const std::vector<std::string> LexiconManager::searchWord(std::string& nativeName) const
{
		//return the native name, imperial name, and wordclass
		std::vector<std::string> wordInfo;
}

std::string LexiconManager::getRandomImperialWord() const
{
		// Collect all imperial words from all word types
		std::vector<std::string> allImperialWords;

		for (const auto& [key, word] : m_nouns)
				allImperialWords.push_back(word.m_imperialWord);

		for (const auto& [key, word] : m_verbs)
				allImperialWords.push_back(word.m_imperialWord);

		for (const auto& [key, word] : m_adjectives)
				allImperialWords.push_back(word.m_imperialWord);

		if (allImperialWords.empty())
		{
				std::cerr << "No words available in lexicon!" << std::endl;
				return "unknown";
		}

		int randomIndex = rand() % allImperialWords.size();
		return allImperialWords[randomIndex];
}

std::string LexiconManager::getTranslation(const std::string& imperialWord) const
{
		// Search through all word maps to find the native translation
		auto searchMap = [&](const std::unordered_map<std::string, Word>& wordMap) -> std::string
				{
						for (const auto& [key, word] : wordMap)
						{
								if (word.m_imperialWord == imperialWord)
										return word.m_nativeWord;
						}
						return "";
				};

		std::string translation = searchMap(m_nouns);
		if (!translation.empty()) return translation;

		translation = searchMap(m_verbs);
		if (!translation.empty()) return translation;

		translation = searchMap(m_adjectives);
		if (!translation.empty()) return translation;

		std::cerr << "Translation not found for: " << imperialWord << std::endl;
		return "unknown";
}
