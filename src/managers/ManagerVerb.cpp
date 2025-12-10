#include "ManagerVerb.h"

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

void VerbManager::verb_kaulay(const BicolGrammar::Sentence& sentence)
{
		std::cout << "=== BAKO (TALK/SPEAK) ACTION ===" << std::endl;

		// Implementation for talking to NPCs
		// Check if second argument is another entity

		if(!sentence.secondArgument.entity ||
		   !sentence.secondArgument.entity->hasComponent<CPopulation>())
		{
				std::cout << "Error: Can only talk to people" << std::endl;
				return;
		}

		std::cout << "Initiating conversation" << std::endl;
		// Trigger dialogue scene translation scene
}
