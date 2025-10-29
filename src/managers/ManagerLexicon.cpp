#include "ManagerLexicon.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <random>

LexiconManager::LexiconManager(const std::string& gameLexiconPath, const std::string& playerDictPath)
    : m_gameLexiconPath(gameLexiconPath), m_playerDictionaryPath(playerDictPath), m_gen(m_rd()) {
    loadGameLexicon();
    loadPlayerDictionary();
}

void LexiconManager::loadGameLexicon() {
    std::ifstream file(m_gameLexiconPath);

    if (!file.is_open()) {
        std::cerr << "Cannot open game lexicon: " << m_gameLexiconPath << std::endl;
        return;
    }

    json lexicon;
    file >> lexicon;

    // Load words
    const auto& words = lexicon["words"];
    for (auto it = words.begin(); it != words.end(); ++it) {
        const std::string& nativeWord = it.key();
        const auto& wordData = it.value();

        WordData data;
        data.native = nativeWord;
        data.english = wordData.value("english", "");

        // Parse word type
        std::string typeStr = wordData.value("type", "NOUN");

        if (typeStr == "VERB")
           data.type = WordType::VERB;
        else if(typeStr == "ADJECTIVE")
           data.type = WordType::ADJECTIVE;
        else if (typeStr == "PHRASE")
           data.type = WordType::PHRASE;
        else if (typeStr == "BUILDING")
           data.type = WordType::BUILDING;
        else if (typeStr == "PROFESSION")
           data.type = WordType::PROFESSION;
        else
           data.type = WordType::NOUN;

        data.baseDamage = wordData.value("baseDamage", 0);
        data.duration = wordData.value("duration", 0.0f);
        data.insightCost = wordData.value("insightCost", 0);
        data.prestigeValue = wordData.value("prestigeValue", 0);
        data.difficulty = wordData.value("difficulty", 1);

        // Parse arrays
        if (wordData.contains("synonyms"))
        {
            data.synonyms = wordData["synonyms"].get<std::vector<std::string>>();
        }
        if (wordData.contains("relatedWords"))
        {
            data.relatedWords = wordData["relatedWords"].get<std::vector<std::string>>();
        }
        if (wordData.contains("requiredArguments"))
        {
            data.requiredArguments = wordData["requiredArguments"].get<std::vector<std::string>>();
        }
        if (wordData.contains("unlockableWords"))
        {
            data.unlockableWords = wordData["unlockableWords"].get<std::vector<std::string>>();
        }

        data.category = wordData.value("category", "");
        data.isTransitive = wordData.value("isTransitive", false);
        data.defenseBonus = wordData.value("defenseBonus", 0);

        // Parse verb category
        if (data.type == WordType::VERB)
        {
            std::string categoryStr = wordData.value("verbCategory", "MOVEMENT");
            if (categoryStr == "SOCIAL")
               data.verbCategory = VerbCategory::SOCIAL;
            else if (categoryStr == "COMBAT")
               data.verbCategory = VerbCategory::COMBAT;
            else if (categoryStr == "ESPIONAGE")
               data.verbCategory = VerbCategory::ESPIONAGE;
            else if (categoryStr == "RESOURCE")
               data.verbCategory = VerbCategory::RESOURCE;
            else
               data.verbCategory = VerbCategory::MOVEMENT;
        }

        // Parse system triggers
        if (wordData.contains("systemTriggers"))
        {
            data.systemTriggers = parseSystemTriggers(wordData["systemTriggers"]);
        }

        m_gameLexicon[nativeWord] = data;
    }

    std::cout << "Loaded " << m_gameLexicon.size() << " words from lexicon" << std::endl;
}

void LexiconManager::loadPlayerDictionary() {
    std::ifstream file(m_playerDictionaryPath);
    if (!file.is_open())
    {
        std::cerr << "Cannot open player dictionary: " << m_playerDictionaryPath << std::endl;
        return;
    }

    json playerDict;
    file >> playerDict;

    if (playerDict.contains("words")) {
        const auto& words = playerDict["words"];
        for (auto it = words.begin(); it != words.end(); ++it) {
            const std::string& nativeWord = it.key();
            const auto& wordData = it.value();

            // Check if word exists in game lexicon
            auto gameWordIt = m_gameLexicon.find(nativeWord);
            if (gameWordIt != m_gameLexicon.end()) {
                m_playerDictionary[nativeWord] = gameWordIt->second;
            }
        }
    }

    std::cout << "Loaded " << m_playerDictionary.size() << " words from player dictionary" << std::endl;
}

void LexiconManager::savePlayerDictionary() {
    json playerDict;
    playerDict["metadata"] = {
        {"version", "1.0"},
        {"total_words", m_playerDictionary.size()},
        {"last_saved", "2024-01-20"}
    };

    json wordsJson;
    for (const auto& [nativeWord, wordData] : m_playerDictionary) {
        wordsJson[nativeWord] = {
            {"english", wordData.english},
            {"type", static_cast<int>(wordData.type)},
            {"prestige", wordData.prestigeValue}
        };
    }
    playerDict["words"] = wordsJson;

    std::ofstream file(m_playerDictionaryPath);
    if (file.is_open()) {
        file << playerDict.dump(4);
        std::cout << "Saved player dictionary with " << m_playerDictionary.size() << " words" << std::endl;
    } else {
        std::cerr << "Cannot save player dictionary: " << m_playerDictionaryPath << std::endl;
    }
}

void LexiconManager::initializeSemanticChains() {
    // Semantic chains are loaded from JSON, but we can also generate some automatically
    for (const auto& [word, data] : m_gameLexicon) {
        if (!data.relatedWords.empty()) {
            m_semanticChains[word] = data.relatedWords;
        }
    }
}

std::vector<SystemTrigger> LexiconManager::parseSystemTriggers(const json& triggersArray) {
    std::vector<SystemTrigger> triggers;

    for (const auto& triggerData : triggersArray) {
        triggers.push_back(parseSystemTriggers(triggerData));
    }

    // Sort by priority (higher priority first)
    std::sort(triggers.begin(), triggers.end(),
              [](const SystemTrigger& a, const SystemTrigger& b) {
                  return a.priority > b.priority;
              });

    return triggers;
}

std::vector<SystemTrigger> LexiconManager::getSystemTriggers(const std::string& verb, const std::string& aspect) const {
    const WordData* wordData = getWordData(verb);
    if (!wordData) return {};

    // Fall back to general system triggers
    return wordData->systemTriggers;
}

const LexiconManager::WordData* LexiconManager::getWordData(const std::string& nativeWord) const {
    auto it = m_gameLexicon.find(nativeWord);
    if (it != m_gameLexicon.end()) {
        return &it->second;
    }
    return nullptr;
}

bool LexiconManager::playerKnowsWord(const std::string& nativeWord) const {
    return m_playerDictionary.find(nativeWord) != m_playerDictionary.end();
}

bool LexiconManager::addWordToPlayerDictionary(const std::string& nativeWord) {
    auto it = m_gameLexicon.find(nativeWord);
    if (it != m_gameLexicon.end()) {
        m_playerDictionary[nativeWord] = it->second;
        savePlayerDictionary();
        return true;
    }
    return false;
}

void LexiconManager::removeWordFromPlayerDictionary(const std::string& nativeWord) {
    auto it = m_playerDictionary.find(nativeWord);
    if (it != m_playerDictionary.end()) {
        m_playerDictionary.erase(it);
        savePlayerDictionary();
    }
}

// Random word selection
std::string LexiconManager::getRandomEnglishWord(int maxDifficulty) const
{
    std::vector<std::string> candidates;
    for (const auto& [native, data] : m_gameLexicon)
    {
        if (data.difficulty <= maxDifficulty) {
            candidates.push_back(data.english);
        }
    }

    if (candidates.empty()) return "";

    std::uniform_int_distribution<> dis(0, candidates.size() - 1);
    return candidates[dis(m_gen)];
}

std::string LexiconManager::getRandomWordByType(WordType type, int maxDifficulty) const
{
    std::vector<std::string> candidates;
    for (const auto& [native, data] : m_gameLexicon)
    {
        if (data.type == type && data.difficulty <= maxDifficulty)
        {
            candidates.push_back(native);
        }
    }

    if (candidates.empty()) return "";

    std::uniform_int_distribution<> dis(0, candidates.size() - 1);
    return candidates[dis(m_gen)];
}

std::string LexiconManager::getRandomWordByCategory(const std::string& category, int maxDifficulty) const
{
    std::vector<std::string> candidates;
    for (const auto& [native, data] : m_gameLexicon)
    {
        if (data.category == category && data.difficulty <= maxDifficulty)
        {
            candidates.push_back(native);
        }
    }

    if (candidates.empty()) return "";

    std::uniform_int_distribution<> dis(0, candidates.size() - 1);
    return candidates[dis(m_gen)];
}

std::vector<std::string> LexiconManager::getRandomWords(int count, int maxDifficulty) const
{
    std::vector<std::string> candidates;
    for (const auto& [native, data] : m_gameLexicon)
    {
        if (data.difficulty <= maxDifficulty)
        {
            candidates.push_back(native);
        }
    }

    if (candidates.empty()) return {};

    // Shuffle and return requested count
    std::shuffle(candidates.begin(), candidates.end(), m_gen);
    if (candidates.size() > count) {
        candidates.resize(count);
    }

    return candidates;
}

// Building and combat utilities
std::vector<std::string> LexiconManager::getBuildingNames() const {
    std::vector<std::string> buildings;
    for (const auto& [native, data] : m_gameLexicon) {
        if (data.type == WordType::BUILDING) {
            buildings.push_back(native);
        }
    }
    return buildings;
}

std::vector<std::string> LexiconManager::getMovementVerbs() const {
    std::vector<std::string> verbs;
    for (const auto& [native, data] : m_gameLexicon) {
        if (data.type == WordType::VERB && data.verbCategory == VerbCategory::MOVEMENT) {
            verbs.push_back(native);
        }
    }
    return verbs;
}

std::vector<std::string> LexiconManager::getCombatWords(int maxDifficulty) const {
    std::vector<std::string> words;
    for (const auto& [native, data] : m_gameLexicon) {
        if (data.baseDamage > 0 && data.difficulty <= maxDifficulty) {
            words.push_back(native);
        }
    }
    return words;
}

std::vector<std::string> LexiconManager::getAnagramCandidates(int wordLength) const {
    std::vector<std::string> candidates;
    for (const auto& [native, data] : m_gameLexicon) {
        if (native.length() == wordLength && data.difficulty <= 3) {
            candidates.push_back(native);
        }
    }
    return candidates;
}

// Semantic chaining
std::vector<std::string> LexiconManager::getRelatedWords(const std::string& baseWord) const {
    auto it = m_semanticChains.find(baseWord);
    if (it != m_semanticChains.end()) {
        return it->second;
    }

    // Fall back to word data related words
    const WordData* data = getWordData(baseWord);
    if (data) {
        return data->relatedWords;
    }

    return {};
}

bool LexiconManager::areWordsRelated(const std::string& word1, const std::string& word2) const {
    auto related = getRelatedWords(word1);
    return std::find(related.begin(), related.end(), word2) != related.end();
}

// Dictionary management
void LexiconManager::clearPlayerDictionary() {
    m_playerDictionary.clear();
    savePlayerDictionary();
}

// Search and filtering
std::vector<LexiconManager::WordData> LexiconManager::searchWords(const std::string& query, WordType type) const {
    std::vector<WordData> results;

    for (const auto& [native, data] : m_gameLexicon) {
        bool matches = false;

        // Check if query matches native word, english translation, or category
        if (native.find(query) != std::string::npos ||
            data.english.find(query) != std::string::npos ||
            data.category.find(query) != std::string::npos) {
            matches = true;
        }

        // Filter by type if specified
        if (matches && (type == WordType::NOUN || data.type == type)) {
            results.push_back(data);
        }
    }

    return results;
}

std::vector<std::string> LexiconManager::getWordsByVerbCategory(VerbCategory category) const {
    std::vector<std::string> verbs;
    for (const auto& [native, data] : m_gameLexicon) {
        if (data.type == WordType::VERB && data.verbCategory == category) {
            verbs.push_back(native);
        }
    }
    return verbs;
}

// Analytics
int LexiconManager::calculatePlayerPrestige() const {
    int totalPrestige = 0;
    for (const auto& [native, data] : m_playerDictionary) {
        totalPrestige += data.prestigeValue;
    }
    return totalPrestige;
}

std::map<LexiconManager::WordType, int> LexiconManager::getVocabularyBreakdown() const {
    std::map<WordType, int> breakdown;
    for (const auto& [native, data] : m_playerDictionary) {
        breakdown[data.type]++;
    }
    return breakdown;
}

// Building integration methods
bool LexiconManager::loadBuildingDefinitions(const std::string& filename) {
    // Buildings are already loaded in the main lexicon
    // This method can be used for additional building-specific data
    return true;
}

const LexiconManager::BuildingData* LexiconManager::getBuildingData(const std::string& buildingType) const {
    const WordData* wordData = getWordData(buildingType);
    if (!wordData || wordData->type != WordType::BUILDING) {
        return nullptr;
    }

    // Convert WordData to BuildingData
    static BuildingData building;
    building.nativeName = wordData->native;
    building.englishName = wordData->english;
    building.type = wordData->category;
    building.defenseBonus = wordData->defenseBonus;
    building.unlockableWords = wordData->unlockableWords;
    building.relatedWords = wordData->relatedWords;

    return &building;
}

std::vector<std::string> LexiconManager::getBuildingsByCategory(const std::string& category) const {
    std::vector<std::string> buildings;
    for (const auto& [native, data] : m_gameLexicon) {
        if (data.type == WordType::BUILDING && data.category == category) {
            buildings.push_back(native);
        }
    }
    return buildings;
}

std::vector<std::string> LexiconManager::getWordsRelatedToBuilding(const std::string& buildingType) const {
    const WordData* buildingData = getWordData(buildingType);
    if (buildingData && buildingData->type == WordType::BUILDING) {
        return buildingData->relatedWords;
    }
    return {};
}

std::vector<std::string> LexiconManager::getCombatWordsForBuilding(const std::string& buildingType, int maxDifficulty) const {
    const WordData* buildingData = getWordData(buildingType);
    if (!buildingData || buildingData->type != WordType::BUILDING) {
        return {};
    }

    // Get building-specific combat words or fall back to related words
    std::vector<std::string> combatWords;

    // First, try to find words specifically tagged for this building's combat
    for (const auto& word : buildingData->relatedWords) {
        const WordData* relatedWord = getWordData(word);
        if (relatedWord && relatedWord->difficulty <= maxDifficulty) {
            combatWords.push_back(word);
        }
    }

    // If no specific combat words, use building's related words
    if (combatWords.empty()) {
        combatWords = buildingData->relatedWords;
    }

    return combatWords;
}

std::string LexiconManager::getRandomWordForBuilding(const std::string& buildingType, int maxDifficulty) const {
    auto words = getCombatWordsForBuilding(buildingType, maxDifficulty);
    if (words.empty()) return "";

    std::uniform_int_distribution<> dis(0, words.size() - 1);
    return words[dis(m_gen)];
}

std::vector<std::string> LexiconManager::getWordsUnlockedByBuilding(const std::string& buildingType) const {
    const WordData* buildingData = getWordData(buildingType);
    if (buildingData && buildingData->type == WordType::BUILDING) {
        return buildingData->unlockableWords;
    }
    return {};
}

// Translation methods
std::string LexiconManager::getTranslation(const std::string& englishWord) const {
    for (const auto& [native, data] : m_gameLexicon) {
        if (data.english == englishWord) {
            return native;
        }
    }
    return "";
}

std::string LexiconManager::getEnglish(const std::string& nativeWord) const {
    const WordData* data = getWordData(nativeWord);
    return data ? data->english : "";
}

bool LexiconManager::wordExists(const std::string& word) const {
    return m_gameLexicon.find(word) != m_gameLexicon.end();
}

bool LexiconManager::verbExists(const std::string& verb) const {
    auto it = m_gameLexicon.find(verb);
    return it != m_gameLexicon.end() && it->second.type == WordType::VERB;
}

std::vector<LexiconManager::VerbAction> LexiconManager::getVerbActions(const std::string& verb, const std::string& aspect) const {
    auto it = m_verbActions.find(verb);
    if (it == m_verbActions.end()) return {};

    if (aspect.empty()) {
        return it->second;
    }

    std::vector<VerbAction> filtered;
    for (const auto& action : it->second) {
        if (action.aspect == aspect) {
            filtered.push_back(action);
        }
    }
    return filtered;
}
