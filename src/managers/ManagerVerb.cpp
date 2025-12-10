#include "ManagerVerb.h"
#include "SceneDialogue.h"

VerbManager::VerbManager(GameEngine* game) :
		m_game(game)
{
		initializeVerbActions();
}

void VerbManager::initializeVerbActions()
{
		m_verbActionMap["laog"] = &VerbManager::verb_laog;
		m_verbActionMap["kaulay"] = &VerbManager::verb_kaulay;

		std::cout << "Initialized " << m_verbActionMap.size() << " verb actions." << std::endl;
}

void VerbManager::executeSentence(const BicolGrammar::Sentence& sentence)
{
		std::cout << "Executing: " << sentence.conjugatedVerb << std::endl;
		std::cout << "First arg: " << sentence.firstArgument.entityName
				  << " (" << (sentence.firstArgument.isPronoun ?
							  sentence.firstArgument.pronounForm : "entity") << ")" << std::endl;
		std::cout << "Second arg: " << sentence.secondArgument.entityName
				  << " (" << (sentence.secondArgument.isPronoun ?
							  sentence.secondArgument.pronounForm : "entity") << ")" << std::endl;

		// Find and execute the verb action
		auto it = m_verbActionMap.find(sentence.verb);
		if(it != m_verbActionMap.end())
		{
				VerbAction action = it->second;
				(this->*action)(sentence);
		}
		else
		{
				std::cerr << "Error: No action mapped for verb: "
						  << sentence.verb << std::endl;
		}
}

VerbManager::SentenceValidator VerbManager::validateSentence(const BicolGrammar::Sentence& sentence)
{
		SentenceValidator result;
		result.isValid = false;

		// Check all parts are present
		if(!sentence.hasFirstArgument)
		{
				result.errorMessage = "Missing first argument";
				return result;
		}

		if(!sentence.hasVerb || !sentence.hasVoice)
		{
				result.errorMessage = "Missing or incomplete verb";
				return result;
		}

		if(!sentence.hasSecondArgument)
		{
				result.errorMessage = "Missing second argument";
				return result;
		}

		if(m_verbActionMap.find(sentence.verb) == m_verbActionMap.end())
		{
				result.errorMessage = "Unknown verb action: " + sentence.verb;
				return result;
		}

		result.isValid = true;
		return result;
}

bool VerbManager::isPlayerInSentence(const BicolGrammar::Sentence& sentence)
{
		// Check if first argument is "ako" (I/me)
		if(sentence.firstArgument.isPronoun &&
		   sentence.firstArgument.pronounForm == "ako")
		{
				return true;
		}

		// Check if it's the player entity
		auto player = m_game->getWorldManager()->getPlayer();
		if(sentence.firstArgument.entity == player)
		{
				return true;
		}

		return false;
}

void VerbManager::verb_laog(const BicolGrammar::Sentence& sentence)
{
		std::cout << "=== LAOG (ENTER) ACTION ===" << std::endl;

		// Check if player is the actor
		if(!isPlayerInSentence(sentence))
		{
				std::cout << "Error: Only player can enter buildings" << std::endl;
				return;
		}

		// Check if second argument is a building with oblique case (sa)
		if(sentence.secondArgument.caseType != Case::OBLIQUE)
		{
				std::cout << "Error: Use 'sa' marker for location" << std::endl;
				return;
		}

		if(!sentence.secondArgument.entity ||
		   !sentence.secondArgument.entity->hasComponent<CBuilding>())
		{
				std::cout << "Error: Can only enter buildings" << std::endl;
				return;
		}

		auto& building = sentence.secondArgument.entity->getComponent<CBuilding>();

		std::cout << "Player entering building: "
				  << sentence.secondArgument.entityName << std::endl;

		// Trigger scene change based on building type
		switch(building.type)
		{
        case Building::SCHOOL:
				std::cout << "Entering school - triggering school scene" << std::endl;
				// m_game->changeScene("school", std::make_shared<SceneSchool>(m_game));
				break;

        case Building::HOME:
				std::cout << "Entering home - triggering home scene" << std::endl;
				// m_game->changeScene("home", std::make_shared<SceneHome>(m_game));
				break;

        case Building::PLAZA:
				std::cout << "Entering plaza - triggering plaza scene" << std::endl;
				// m_game->changeScene("plaza", std::make_shared<ScenePlaza>(m_game));
				break;

        case Building::WORKPLACE:
				std::cout << "Entering workplace - triggering work scene" << std::endl;
				// m_game->changeScene("work", std::make_shared<SceneWork>(m_game));
				break;

        case Building::GOVERNMENT:
				std::cout << "Entering government building" << std::endl;
				// m_game->changeScene("government", std::make_shared<SceneGov>(m_game));
				break;
		default:
				break;
		}
}

//void VerbManager::verb_kaulay(const BicolGrammar::Sentence& sentence)
/*{
  std::cout << "=== KAULAY (TALK/CONVERSE) ACTION ===" << std::endl;

  // Check if player is the actor
  if(!isPlayerInSentence(sentence))
  {
  std::cout << "Error: Only player can initiate conversations" << std::endl;
  return;
  }

  // Check if second argument is a person
  if(!sentence.secondArgument.entity ||
  !sentence.secondArgument.entity->hasComponent<CPopulation>())
  {
  std::cout << "Error: Can only talk to people" << std::endl;
  return;
  }

  auto& targetPop = sentence.secondArgument.entity->getComponent<CPopulation>();

  // Check if target speaks Imperial language (target for conversion)
  if(targetPop.speakingLanguage != Language::IMPERIAL)
  {
  std::cout << "This person already speaks the native language!" << std::endl;
  // TODO: Show a message in UI instead of just logging
  return;
  }

  std::cout << "Initiating conversation with Imperial speaker..." << std::endl;

  // Count nearby civilians for success chance calculation
  // For now using placeholder values - you could enhance this by checking
  // the tile's CPath component for nearby entities
  int nearbyNativeSpeakers = 0;
  int nearbyImperialSpeakers = 0;

  // Get a random word to translate from the lexicon
  auto& lexicon = m_game->getLexiconManager();
  std::vector<std::string> allWords = lexicon.getVerbs();

  // Add adjectives to the pool of words
  auto adjectives = lexicon.getAdjectives();
  allWords.insert(allWords.end(), adjectives.begin(), adjectives.end());

  if(allWords.empty())
  {
  std::cout << "Error: No words in lexicon!" << std::endl;
  return;
  }

  // Pick a random word
  int randomIndex = rand() % allWords.size();
  std::string nativeWord = allWords[randomIndex];

  // Get the imperial translation
  std::string imperialWord = lexicon.getImperialTranslation(nativeWord);

  if(imperialWord.empty())
  {
  std::cout << "Error: No translation found for " << nativeWord << std::endl;
  return;
  }

  std::cout << "Challenge: Translate \"" << imperialWord
  << "\" to native language" << std::endl;
  std::cout << "Correct answer: " << nativeWord << std::endl;

  // Create and switch to dialogue scene
  m_game->changeScene("dialogue",
  std::make_shared<SceneDialogue>(
  m_game,
  sentence.secondArgument.entity,  // target civilian
  imperialWord,                     // word to translate FROM (Imperial)
  nativeWord,                       // correct translation TO (Native)
  nearbyNativeSpeakers,            // bonus from nearby natives
  nearbyImperialSpeakers           // penalty from nearby imperials
  )
  );

  std::cout << "Dialogue scene started!" << std::endl;
  }*/

void VerbManager::verb_kaulay(const BicolGrammar::Sentence& sentence)
{
		std::cout << "=== KAULAY (TALK/CONVERSE) ACTION ===" << std::endl;

		// Check if player is the actor
		if(!isPlayerInSentence(sentence))
		{
				std::cout << "Error: Only player can initiate conversations" << std::endl;
				return;
		}

		// Check if second argument is a person
		if(!sentence.secondArgument.entity ||
		   !sentence.secondArgument.entity->hasComponent<CPopulation>())
		{
				std::cout << "Error: Can only talk to people" << std::endl;
				return;
		}

		auto& targetPop = sentence.secondArgument.entity->getComponent<CPopulation>();

		// Check if target speaks Imperial language (target for conversion)
		if(targetPop.speakingLanguage != Language::IMPERIAL)
		{
				std::cout << "This person already speaks the native language!" << std::endl;
				return;
		}

		std::cout << "Initiating conversation with Imperial speaker..." << std::endl;

		// Count nearby civilians for success chance calculation
		int nearbyNativeSpeakers = 0;
		int nearbyImperialSpeakers = 0;

		// Hardcoded word pairs for now (Native -> Imperial)
		// TODO: Get these from LexiconManager once we have access to it
		std::vector<std::pair<std::string, std::string>> wordPairs = {
				{"duman", "to go"},
				{"laog", "to enter"},
				{"kaulay", "to talk"},
				{"magayon", "beautiful"},
				{"matibay", "strong"},
				{"makusog", "powerful"},
				{"mahigos", "difficult"},
				{"dakula", "big"},
				{"sadit", "small"},
				{"makanos", "hot"}
		};

		if(wordPairs.empty())
		{
				std::cout << "Error: No words available!" << std::endl;
				return;
		}

		// Pick a random word
		int randomIndex = rand() % wordPairs.size();
		std::string nativeWord = wordPairs[randomIndex].first;
		std::string imperialWord = wordPairs[randomIndex].second;

		std::cout << "Challenge: Translate \"" << imperialWord
				  << "\" to native language" << std::endl;
		std::cout << "Correct answer: " << nativeWord << std::endl;

		// Create and switch to dialogue scene
		m_game->changeScene("dialogue",
							std::make_shared<SceneDialogue>(
									m_game,
									sentence.secondArgument.entity,  // target civilian
									imperialWord,                     // word to translate FROM (Imperial)
									nativeWord,                       // correct translation TO (Native)
									nearbyNativeSpeakers,            // bonus from nearby natives
									nearbyImperialSpeakers           // penalty from nearby imperials
									)
				);

		std::cout << "Dialogue scene started!" << std::endl;
}
