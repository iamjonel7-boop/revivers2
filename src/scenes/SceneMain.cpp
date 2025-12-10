#include "SceneMain.h"
#include "Components.h"
#include "GameEngine.h"
#include "ManagerMap.h"
#include "ManagerWorld.h"
#include "SceneInventory.h"
#include "SceneGameOver.h"
#include <iostream>
#include <fstream>

using SMain = SceneMain;

SMain::SceneMain(GameEngine* gameEngine) :
		Scene(gameEngine)
{
		init();

		m_controlState = MapControlState::NAVIGATING;
		std::cout << "Navigating state.";

		registerAction(sf::Keyboard::W, static_cast<int>(ActionName::MOVE_UP));
		registerAction(sf::Keyboard::A, static_cast<int>(ActionName::MOVE_LEFT));
		registerAction(sf::Keyboard::S, static_cast<int>(ActionName::MOVE_DOWN));
		registerAction(sf::Keyboard::D, static_cast<int>(ActionName::MOVE_RIGHT));
		registerAction(sf::Keyboard::Tab, static_cast<int>(ActionName::OPEN_INVENTORY));
		registerAction(sf::Keyboard::E, static_cast<int>(ActionName::SELECT_TILE));
		registerAction(sf::Keyboard::Q, static_cast<int>(ActionName::RETURN));
		registerAction(sf::Keyboard::Right, static_cast<int>(ActionName::NEXT_PAGE));
		registerAction(sf::Keyboard::Left, static_cast<int>(ActionName::PREV_PAGE));
		registerAction(sf::Keyboard::Up, static_cast<int>(ActionName::UP));
		registerAction(sf::Keyboard::Down, static_cast<int>(ActionName::DOWN));
		registerAction(sf::Keyboard::Enter, static_cast<int>(ActionName::CONFIRM_SELECTION));
		registerAction(sf::Keyboard::Space, static_cast<int>(ActionName::EXECUTE_SENTENCE));
}

void SMain::init()
{
		m_mapManager.makeMapView(m_mainView);
		m_mapManager.makeMapGrid(m_entities, m_tileOutlineShape);

		m_mapManager.locateBuildings();
		m_mapManager.printBuildingCoords();
		m_mapManager.makeBuildings(m_entities);

		m_mapManager.locatePaths();
		m_mapManager.printPathCoords();
		m_mapManager.makePaths(m_entities);

		makeStatistics();
		m_entities.update(); //solve paths not exist and cursor pos

		makeCursor();
		createPronounOptions();

		m_availableVerbs = m_lexiconManager.getVerbs();
		m_availableVoices = m_bicolgrammar.getVoicesAsStrings();

		auto  m_player = m_game->getWorldManager()->getPlayer();

		std::cout << "=== SCENE MAIN ===" << std::endl;
		if (m_player)
		{
				std::cout << "Player memory address: " << m_player.get() << std::endl;
				std::cout << "Player name: " << m_player->getComponent<CProfile>().playerName << std::endl;
				std::cout << "Has CTransform: " << m_player->hasComponent<CTransform>() << std::endl;
				std::cout << "Has CProfile: " << m_player->hasComponent<CProfile>() << std::endl;
		} else
		{
				std::cout << "ERROR: Player is null!" << std::endl;
		}

		m_sentencePanel = createBox({0.f, 450.f}, {800.f, 40.f}, sf::Color::Black);
		m_sentencingPanel = createBox({0.f, 480.f}, {600.f, 120.f}, sf::Color::Black);
		m_sentencingPanel.setOutlineThickness(1.f);
		m_sentencingPanel.setOutlineColor(sf::Color::White);

		m_font.loadFromFile("/usr/local/share/fonts/Liberation/LiberationMono-Regular.ttf");

		m_helpMsg = createText("", m_font, {610.f, 388.f}, 10, sf::Color::White);
		m_helpMsg2 = createText("", m_font, {610.f, 415.f}, 10, sf::Color::White);

		m_statusHint = createText("", m_font, {7.f, 40.f}, 10, sf::Color::Yellow);

		m_progressBarBackground.setSize({586.f, 20.f});
		m_progressBarBackground.setPosition({7.f, 58.f});
		m_progressBarBackground.setFillColor(sf::Color(50, 50, 50));
		m_progressBarBackground.setOutlineThickness(1.f);
		m_progressBarBackground.setOutlineColor(sf::Color::White);

		m_progressBarNative.setSize({0.f, 20.f});
		m_progressBarNative.setPosition({7.f, 58.f});
		m_progressBarNative.setFillColor(sf::Color::Green);

		m_progressBarImperial.setSize({0.f, 20.f});
		m_progressBarImperial.setFillColor(sf::Color::Red);
}

void SceneMain::createPronounOptions()
{
		// All available pronoun forms - player must choose correctly!
		m_pronounOptions = {
				"ako",   // Direct case (I - subject)
				"ko",    // Indirect case (my, me - possessive/agent)
				"sako",  // Oblique case (to me, for me)
				"ika",   // Direct case (you - subject)
				"mo",    // Indirect case (your, you - possessive/agent)
				"saimo", // Oblique case (to you, for you)
				"siya",  // Direct case (he/she - subject)
				"niya",  // Indirect case (his/her - possessive/agent)
				"saiya"  // Oblique case (to him/her, for him/her)
		};

		std::cout << "Created pronoun options" << std::endl;
		//todo this will be retried from the grammar
}

void SMain::update()
{
		m_entities.update();

		float deltaTime = m_game->getDeltaTime();

		m_game->getWorldManager()->updateTime(deltaTime);
		m_game->getWorldManager()->updateWorld(m_nativeEntities, m_imperialEntities, m_entities);

		updateStatistics();
		updateHintSystem();
		updateProgressBar();

		std::string help1, help2;
		const auto& func = m_updateTable[static_cast<size_t>(m_controlState)];
		if(func != nullptr) (this->*func)(help1, help2);

		m_helpMsg.setString(help1);
		m_helpMsg2.setString(help2);

		if(m_game->getWorldManager()->isGameOver())
		{
				std::cout << "Game Over detected in SceneMain!" << std::endl;
				bool victory = m_game->getWorldManager()->isVictory();

				// Need to include SceneGameOver.h at top of SceneMain.cpp:
				// #include "SceneGameOver.h"
				m_game->changeScene("gameover", std::make_shared<SceneGameOver>(m_game, victory));
		}
}

void SceneMain::makeStatistics()
{
		auto& worldMgr = *m_game->getWorldManager();

		std::string a = "Population: " + std::to_string(worldMgr.population);
		m_totalPopulation = createText(a, m_font, {7.f, 6.f}, 10, sf::Color::White);

		std::string b = "Year: " + std::to_string(worldMgr.currentYear);
		m_year = createText(b, m_font, {7.f, 23.f}, 10, sf::Color::White);

		std::string c = "Native Ethnicity: " + std::to_string(worldMgr.nativEthnicity);
		m_nativEthnicity = createText(c, m_font, {142.f, 6.f}, 10, sf::Color::White);

		std::string d = "Imperial Ethnicity: " + std::to_string(worldMgr.imperialEthnicity);
		m_imperialEthnicity = createText(d, m_font, {142.f, 23.f}, 10, sf::Color::White);

		std::string e = "Native Language Speakers: " + std::to_string(worldMgr.nativeSpeakers);
		m_nativeSpeakers = createText(e, m_font, {300.f, 6.f}, 10, sf::Color::White);

		std::string f = "Imperial Language Speakers: " + std::to_string(worldMgr.imperialSpeakers);
		m_imperialSpeakers = createText(f, m_font, {300.f, 23.f}, 10, sf::Color::White);
}

void SceneMain::updateStatistics()
{
		auto& worldMgr = *m_game->getWorldManager();

		std::string a = "Population: " + std::to_string(worldMgr.population);
		m_totalPopulation.setString(a);

		std::string b = "Year: " + std::to_string(worldMgr.currentYear);
		m_year.setString(b);

		std::string c = "Native People: " + std::to_string(worldMgr.nativEthnicity);
		m_nativEthnicity.setString(c);

		std::string d = "Imperial People: " + std::to_string(worldMgr.imperialEthnicity);
		m_imperialEthnicity.setString(d);

		std::string e = "Native Language Speakers: " + std::to_string(worldMgr.nativeSpeakers);
		m_nativeSpeakers.setString(e);

		std::string f = "Imperial Language Speakers: " + std::to_string(worldMgr.imperialSpeakers);
		m_imperialSpeakers.setString(f);
}

void SceneMain::updateSelectingPronoun(std::string& help1, std::string& help2)
{
		int totalPages = (m_pronounOptions.size() + m_pronounsPerPage - 1) / m_pronounsPerPage;
		help1 = "UP/DOWN to select pronoun form\nEnter to confirm";
		help2 = "LEFT/RIGHT for pages (" + std::to_string(m_pronounPage + 1) +
				"/" + std::to_string(totalPages) + ") | Q to cancel";
}

void SceneMain::handleSelectingPronoun(const Action& action, ActionName act)
{
		if(action.type() != ActionType::START) return;

		std::cout << "selecting pronoun" << std::endl;

		int totalPages = (m_pronounOptions.size() + m_pronounsPerPage - 1) / m_pronounsPerPage;

		switch(act)
		{
		case ActionName::DOWN:
				m_currentPronounIndex++;
				if(m_currentPronounIndex >= (int)m_pronounOptions.size())
						m_currentPronounIndex = 0;

				// Update page if needed
				if (m_currentPronounIndex / m_pronounsPerPage != m_pronounPage)
						m_pronounPage = m_currentPronounIndex / m_pronounsPerPage;
				break;
		case ActionName::UP:
				m_currentPronounIndex--;
				if(m_currentPronounIndex < 0)
						m_currentPronounIndex = m_pronounOptions.size() - 1;

				// Update page if needed
				if (m_currentPronounIndex / m_pronounsPerPage != m_pronounPage)
						m_pronounPage = m_currentPronounIndex / m_pronounsPerPage;
				break;
		case ActionName::NEXT_PAGE:
				if (m_pronounPage < totalPages - 1) {
						m_pronounPage++;
						// Set selection to first item on new page
						m_currentPronounIndex = m_pronounPage * m_pronounsPerPage;
				}
				break;
		case ActionName::PREV_PAGE:
				if (m_pronounPage > 0) {
						m_pronounPage--;
						// Set selection to first item on new page
						m_currentPronounIndex = m_pronounPage * m_pronounsPerPage;
				}
				break;
		case ActionName::CONFIRM_SELECTION:
        {
				std::string selectedPronoun = m_pronounOptions[m_currentPronounIndex];
				std::cout << "Selected pronoun: " << selectedPronoun << std::endl;

				// Save the pronoun choice
				if(m_selectingFirstEntity)
				{
						m_currentSentence.firstArgument.entity = nullptr;
						m_currentSentence.firstArgument.isPronoun = true;
						m_currentSentence.firstArgument.pronounForm = selectedPronoun;
						m_currentSentence.firstArgument.marker = selectedPronoun;
						m_currentSentence.firstArgument.entityName = "";
						m_currentSentence.hasFirstArgument = true;

						// Move to verb selection
						m_controlState = MapControlState::SELECTING_VERB;
				}
				else
				{
						m_currentSentence.secondArgument.entity = nullptr;
						m_currentSentence.secondArgument.isPronoun = true;
						m_currentSentence.secondArgument.pronounForm = selectedPronoun;
						m_currentSentence.secondArgument.marker = selectedPronoun;
						m_currentSentence.secondArgument.entityName = "";
						m_currentSentence.hasSecondArgument = true;

						// Sentence complete!
						std::cout << "Sentence construction complete!" << std::endl;

						m_sentenceComplete = true;
						m_awaitingConfirmation = true;
						m_controlState = MapControlState::EXECUTE_SENTENCE;

						std::cout << "space to confirm" << std::endl;

						// Reset for next sentence
						//m_currentSentence.clear();
						//m_selectedEntity = nullptr;
						//m_selectingFirstEntity = true;
						//m_controlState = MapControlState::NAVIGATING;
						//WorldManager::setPaused(false);
				}
				m_pronounPage = 0;
				m_currentPronounIndex = 0;
        }
        break;
		case ActionName::RETURN:
				m_currentPronounIndex = 0;
				m_pronounPage = 0;
				m_controlState = MapControlState::SENTENCING;
				std::cout << "returning to sentencing" << std::endl;
				break;
		default:
				break;
		}
}

void SceneMain::updateSentencing(std::string& help1, std::string& help2)
{
		if(!m_currentTileEntities.empty())
		{
				int totalPages = (m_currentTileEntities.size() + m_entitiesPerPage - 1) / m_entitiesPerPage;
				help1 = "UP/DOWN to select entity\nEnter to confirm selection";
				help2 = "LEFT/RIGHT for pages ("
						+ std::to_string(m_sentencingPage + 1) + "/"
						+ std::to_string(totalPages) + ") | Q to return";
		}
		else
		{
				help1 = "Press Q to return.";
				help2 = "";
		}
}

void SceneMain::updateNavigating(std::string& help1, std::string& help2)
{
		help1 = "Use the WASD keys to navigate\nthe map.";
		help2 = "Press E to select a tile.";
		updateCursorPos();
}

void SceneMain::updateSelectingEntity(std::string& help1, std::string& help2)
{
		if(!m_currentTileEntities.empty())
		{
				int totalPages = (m_currentTileEntities.size() + m_entitiesPerPage - 1) / m_entitiesPerPage;

				if(m_selectingFirstEntity)
						help1 = "UP/DOWN to select first entity\nEnter to confirm selection";
				else
						help1 = "UP/DOWN to select second entity\nEnter to confirm selection";

				help2 = "LEFT/RIGHT for pages ("
						+ std::to_string(m_sentencingPage + 1) + "/"
						+ std::to_string(totalPages) + ") | Q to return";
		}
		else
		{
				help1 = "No entities on this tile.";
				help2 = "Press Q to return.";
		}
}

void SceneMain::updateMarkingEntity(std::string& help1, std::string& help2)
{
		if(m_selectedEntity)
		{
				help1 = "UP/DOWN to select case marker\nEnter to confirm";

				if(m_selectingFirstEntity)
						help2 = "Marking first entity | Q to cancel";
				else
						help2 = "Marking second entity | Q to cancel";
		}
		else
		{
				help1 = "Error: No entity selected";
				help2 = "Press Q to return";
		}
}

void SceneMain::updateSelectingVerb(std::string& help1, std::string& help2)
{
		int totalPages = (m_availableVerbs.size() + m_verbsPerPage - 1) / m_verbsPerPage;
		help1 = "UP/DOWN to select verb\nENTER to confirm";
		help2 = "LEFT/RIGHT for pages (" + std::to_string(m_verbPage + 1) +
				"/" + std::to_string(totalPages) + ") | Q to return";
}

void SceneMain::updateConjugatingVerb(std::string& help1, std::string& help2)
{
		if (m_selectingVoice)
		{
				help1 = "UP/DOWN to select voice\nENTER to confirm voice";
				help2 = "Selected verb:\n " + m_currentSentence.verb + " | Q to return to verb selection";
		}
		else
		{
				help1 = "UP/DOWN to select aspect\nENTER to confirm aspect and conjugate";
				help2 = "Voice:\n " + m_availableVoices[m_currentVoiceIndex] + " | Q to change voice";
		}
}

void SceneMain::handleSelectingEntity(const Action& action, ActionName act)
{

}

void SceneMain::handleMarkingEntity(const Action& action, ActionName act)
{
		std::cout << "marking entity" << std::endl;
		if(action.type() != ActionType::START) return;

		switch(act)
		{
		case ActionName::DOWN:
				if(!m_availableCases.empty())
				{
						m_currentCaseIndex++;
						if(m_currentCaseIndex >= (int)m_availableCases.size())
								m_currentCaseIndex = 0;
				}
				break;
		case ActionName::UP:
				if(!m_availableCases.empty())
				{
						m_currentCaseIndex--;
						if(m_currentCaseIndex < 0)
								m_currentCaseIndex = m_availableCases.size() - 1;
				}
				break;
		case ActionName::CONFIRM_SELECTION:
				if(!m_availableCases.empty() && m_currentCaseIndex < m_availableCases.size())
				{
						Case selectedCase = m_availableCases[m_currentCaseIndex];
						std::cout << "Selected case: " << static_cast<int>(selectedCase) << std::endl;

						// Save the marked entity with its case
						if(m_selectingFirstEntity)
						{
								m_currentSentence.firstArgument.entity = m_selectedEntity;
								m_currentSentence.firstArgument.caseType = selectedCase;
								m_currentSentence.firstArgument.entityName = getEntityDisplayName(m_selectedEntity);
								m_currentSentence.hasFirstArgument = true;

								// Move to verb selection
								m_controlState = MapControlState::SELECTING_VERB;
						}
						else
						{
								m_currentSentence.secondArgument.entity = m_selectedEntity;
								m_currentSentence.secondArgument.caseType = selectedCase;
								m_currentSentence.secondArgument.entityName = getEntityDisplayName(m_selectedEntity);
								m_currentSentence.hasSecondArgument = true;

								// Sentence complete!
								std::cout << "Sentence construction complete!" << std::endl;

								m_sentenceComplete = true;
								m_awaitingConfirmation = true;
								m_controlState = MapControlState::EXECUTE_SENTENCE;

								std::cout << "Sentence complete. press space to execute" << std::endl;

								// Reset for next sentence
								//	m_currentSentence.clear();
								//m_selectedEntity = nullptr;
								//m_selectingFirstEntity = true;
								//m_controlState = MapControlState::NAVIGATING;
								//WorldManager::setPaused(false);
						}
				}
				break;
		case ActionName::RETURN:
				m_selectedEntity = nullptr;
				m_availableCases.clear();
				m_currentCaseIndex = 0;
				m_controlState = MapControlState::SENTENCING;
				break;
		default:
				break;
		}
}

void SceneMain::handleSelectingVerb(const Action& action, ActionName act)
{
		if(action.type() != ActionType::START) return;

		int totalPages = (m_availableVerbs.size() + m_verbsPerPage - 1) / m_verbsPerPage;

		switch(act)
		{
		case ActionName::DOWN:
				m_currentVerbIndex++;
				if(m_currentVerbIndex >= (int)m_availableVerbs.size())
						m_currentVerbIndex = 0;

				// Update page if needed
				if (m_currentVerbIndex / m_verbsPerPage != m_verbPage)
						m_verbPage = m_currentVerbIndex / m_verbsPerPage;
				break;

		case ActionName::UP:
				m_currentVerbIndex--;
				if(m_currentVerbIndex < 0)
						m_currentVerbIndex = m_availableVerbs.size() - 1;

				// Update page if needed
				if (m_currentVerbIndex / m_verbsPerPage != m_verbPage)
						m_verbPage = m_currentVerbIndex / m_verbsPerPage;
				break;

		case ActionName::NEXT_PAGE:
				if (m_verbPage < totalPages - 1) {
						m_verbPage++;
						m_currentVerbIndex = m_verbPage * m_verbsPerPage;
				}
				break;

		case ActionName::PREV_PAGE:
				if (m_verbPage > 0) {
						m_verbPage--;
						m_currentVerbIndex = m_verbPage * m_verbsPerPage;
				}
				break;

		case ActionName::CONFIRM_SELECTION:
				if (!m_availableVerbs.empty() && m_currentVerbIndex < (int)m_availableVerbs.size())
				{
						m_currentSentence.verb = m_availableVerbs[m_currentVerbIndex];
						m_currentSentence.hasVerb = true;
						std::cout << "Selected verb: " << m_currentSentence.verb << std::endl;

						// Reset conjugation state
						m_currentVoiceIndex = 0;
						m_selectingVoice = true;

						// Move to conjugation state
						m_controlState = MapControlState::CONJUGATING_VERB;
				}
				break;

		case ActionName::RETURN:
				m_verbPage = 0;
				m_currentVerbIndex = 0;
				m_controlState = MapControlState::SENTENCING;
				break;

		default:
				break;
		}
}

void SceneMain::handleConjugatingVerb(const Action& action, ActionName act)
{
		if(action.type() != ActionType::START) return;

		switch(act)
		{
		case ActionName::DOWN:
				m_currentVoiceIndex++;
				if (m_currentVoiceIndex >= (int)m_availableVoices.size())
						m_currentVoiceIndex = 0;
				break;

		case ActionName::UP:
				m_currentVoiceIndex--;
				if (m_currentVoiceIndex < 0)
						m_currentVoiceIndex = m_availableVoices.size() - 1;
				break;

		case ActionName::CONFIRM_SELECTION:
		{
				// Store voice
				std::string voiceStr = m_availableVoices[m_currentVoiceIndex];
				m_currentSentence.voice = m_bicolgrammar.m_voices.at(voiceStr);
				m_currentSentence.hasVoice = true;
				//std::cout << "Selected voice: " << voiceStr << std::endl;

				Voice voice = m_bicolgrammar.m_voices.at(voiceStr);

				// Conjugate the verb
				m_currentSentence.conjugatedVerb = m_bicolgrammar.conjugateVerb(
						m_currentSentence.verb, voice);

				std::cout << "Conjugated verb: " << m_currentSentence.conjugatedVerb
						  << " (Voice: " << voiceStr << ")" << std::endl;

				// Return to sentencing to select second argument
//						m_selectingVoice = true; // Reset for next time
				m_controlState = MapControlState::SENTENCING;
				m_selectingFirstEntity = false; // Now selecting second argument
		}
		break;

		case ActionName::RETURN:
				m_currentSentence.hasVerb = false;
				m_currentSentence.verb = "";
				m_controlState = MapControlState::SELECTING_VERB;
				break;

		default:
				break;
		}
}

void SMain::sDoAction(const Action& action)
{
		ActionName act = static_cast<ActionName>(action.name());

		const auto& func = m_actionTable[static_cast<size_t>(m_controlState)];
		if(func != nullptr) (this->*func)(action, act);
}

void SceneMain::onEnd()
{
		m_game->quit();
}

void SceneMain::sRender()
{
		m_game->window().setView(m_mainView);

		for (auto& e : m_entities.getEntities())
		{
				if (e->hasComponent<CShape>() && e->hasComponent<CTransform>())
				{
						auto& shape = e->getComponent<CShape>().shape;
						auto& transform = e->getComponent<CTransform>();

						shape->setPosition(transform.position.x, transform.position.y);
						m_game->window().draw(*shape);
				}
		}

		renderTileGrid();
		renderBuildings();
		renderCursor();

		m_game->window().setView(m_game->window().getDefaultView());

		m_game->window().draw(m_helpMsg);
		m_game->window().draw(m_helpMsg2);

		m_game->window().draw(m_totalPopulation);
		m_game->window().draw(m_year);
		m_game->window().draw(m_nativEthnicity);
		m_game->window().draw(m_imperialEthnicity);
		m_game->window().draw(m_imperialSpeakers);
		m_game->window().draw(m_nativeSpeakers);
		m_game->window().draw(m_progressBarBackground);
		m_game->window().draw(m_progressBarNative);
		m_game->window().draw(m_progressBarImperial);

		m_game->window().draw(m_statusHint);

		if(m_currentSentence.hasFirstArgument ||
		   m_controlState == MapControlState::SENTENCING ||
		   m_controlState == MapControlState::MARKING_ENTITY ||
		   m_controlState == MapControlState::SELECTING_VERB ||
		   m_controlState == MapControlState::CONJUGATING_VERB)
		{
				renderSentence();
		}

		// Render the appropriate panel content based on state
		if (m_controlState == MapControlState::SENTENCING)
		{
				const auto& cursor = m_cursorEntity->getComponent<CTransform>();
				sf::Vector2f cursorPos = cursor.position;
				auto tileEntities = getTileEntities(cursorPos);

				if (tileEntities.empty())
				{
						renderTileEntities(tileEntities); // Will show pronoun option
				}
				else
				{
						renderTileEntities(tileEntities);
				}
		}
		else if (m_controlState == MapControlState::SELECTING_PRONOUN)
		{
				renderPronounSelection();
		}
		else if (m_controlState == MapControlState::MARKING_ENTITY)
		{
				renderCaseMarkers();
		}
		else if (m_controlState == MapControlState::SELECTING_VERB)
		{
				renderVerbSelection();
		}
		else if (m_controlState == MapControlState::CONJUGATING_VERB)
		{
				renderVerbConjugation();
		}
}

void SceneMain::handleSentencing(const Action& action, ActionName act)
{
		if(action.type() != ActionType::START)
				return;

		std::cout << "sentencing" << std::endl;

		const auto& cursor = m_cursorEntity->getComponent<CTransform>();
		sf::Vector2f cursorPos = cursor.position;

		auto tileEntities = getTileEntities(cursorPos);
		m_currentTileEntities = tileEntities;

		if(!tileEntities.empty())
		{
				std::cout << "found " << tileEntities.size() << " entities "
						  << "at position (" << cursorPos.x << ", "
						  << cursorPos.y << ")." << std::endl;
		}
		else
				std::cout << "no entities found at current tile." << std::endl;

		switch(act)
		{
		case ActionName::NEXT_PAGE:
				if(!m_currentTileEntities.empty())
				{
						int totalItems = m_currentTileEntities.size() + 1;
						//	int maxPage = (m_currentTileEntities.size() -1)/m_entitiesPerPage;
						int maxPage = (totalItems + m_entitiesPerPage -1)/m_entitiesPerPage - 1;
						if(m_sentencingPage < maxPage)
						{
								m_sentencingPage++;
								m_currentTileEntity = m_sentencingPage * m_entitiesPerPage; // Reset to first item on new page
						}
				}
				break;
		case ActionName::PREV_PAGE:
				if(m_sentencingPage > 0)
				{
						m_sentencingPage--;
						m_currentTileEntity = m_sentencingPage * m_entitiesPerPage;
				}
				break;
		case ActionName::CONFIRM_SELECTION:
				//int totalItems = m_currentTileEntities.size() + 1;
				// Check if special index for "Use Pronoun" option
				if(m_currentTileEntity == m_currentTileEntities.size())
				{
						// Player chose to use a pronoun
						std::cout << "Selected pronoun option" << std::endl;
						m_selectedEntity = nullptr; // No entity, will use pronoun
						m_controlState = MapControlState::SELECTING_PRONOUN;

						m_pronounPage = 0;
						m_currentPronounIndex = 0;
				}
				else if(!m_currentTileEntities.empty() && m_currentTileEntity < m_currentTileEntities.size())
				{
						m_selectedEntity = m_currentTileEntities[m_currentTileEntity];
						std::cout << "Selected entity at index " << m_currentTileEntity << std::endl;

						// Initialize available cases
						m_availableCases = {Case::DIRECT, Case::INDIRECT, Case::OBLIQUE};
						m_currentCaseIndex = 0;

						m_controlState = MapControlState::MARKING_ENTITY;
				}
				break;
		case ActionName::RETURN:
				m_tileEntities.clear();
				m_currentTileEntities.clear();
				m_sentencingPage = 0;
				m_currentTileEntity = 0;

				// Clear sentence if we're starting over
				if(!m_currentSentence.hasFirstArgument)
				{
						m_currentSentence.clear();
						m_selectingFirstEntity = true;
						m_controlState = MapControlState::NAVIGATING;
						WorldManager::setPaused(false);
						std::cout << "returning to navigation - time resumed" << std::endl;
				}
				else
				{
						// If we have a first argument, go back to verb selection
						m_controlState = MapControlState::SELECTING_VERB;
				}
				break;
		case ActionName::DOWN:
		{
				// +1 for the "Use Pronoun" option at the end
				int maxSelection = m_currentTileEntities.empty() ? 0 : m_currentTileEntities.size();

				m_currentTileEntity++;
				if(m_currentTileEntity > maxSelection)
						m_currentTileEntity = 0;

				int newPage = m_currentTileEntity / m_entitiesPerPage;
				if(newPage != m_sentencingPage && m_currentTileEntity < maxSelection)
						m_sentencingPage = newPage;
        }
        break;
		case ActionName::UP:
		{
				int maxSelection = m_currentTileEntities.empty() ? 0 : m_currentTileEntities.size();

				m_currentTileEntity--;
				if(m_currentTileEntity < 0)
						m_currentTileEntity = maxSelection;

				int newPage = m_currentTileEntity / m_entitiesPerPage;
				if(newPage != m_sentencingPage && m_currentTileEntity < maxSelection)
						m_sentencingPage = newPage;
        }
        break;
		default:
				break;
		}
}

std::shared_ptr<Entity> SceneMain::getTileAtCursorPosition(const sf::Vector2f& pos)
{
		auto checkTile = [&](const std::shared_ptr<Entity>& entity) -> bool
				{
						if(entity->hasComponent<CTransform>())
						{
								auto& entityPos = entity->getComponent<CTransform>().position;
								return entityPos.x == pos.x && entityPos.y == pos.y;
						}
						return false;
				};

		auto& pathEnt = m_entities.getEntities("path");
		for(auto& path : pathEnt)
		{
				if(checkTile(path))
						return path;
		}

		auto& buildEnt = m_entities.getEntities("building");
		for(auto& build : buildEnt)
		{
				if(checkTile(build))
						return build;
		}
		return nullptr;
}

std::vector<std::shared_ptr<Entity>> SceneMain::getTileEntities(const sf::Vector2f& pos)
{
		std::vector<std::shared_ptr<Entity>> tileEntities;

		auto tile = getTileAtCursorPosition(pos);
		if(!tile)
		{
				return tileEntities;
		}

		if(tile->hasComponent<CPath>())
		{
				auto& pathEnt = tile->getComponent<CPath>().entities;
				tileEntities.insert(tileEntities.end(), pathEnt.begin(), pathEnt.end());
		}

		if(tile->hasComponent<CBuilding>())
		{
				tileEntities.push_back(tile);
		}

		m_tileEntities = tileEntities;
		return tileEntities;
}

void SceneMain::handleCursorNavigation(const Action& action, ActionName act)
{
		auto& cinput = m_cursorEntity->getComponent<CInput>();

		switch(action.type())
		{
		case ActionType::START:
				switch(act)
				{
				case ActionName::MOVE_UP:
						cinput.up = true;
						break;
				case ActionName::MOVE_DOWN:
						cinput.down = true;
						break;
				case ActionName::MOVE_LEFT:
						cinput.left = true;
						break;
				case ActionName::MOVE_RIGHT:
						cinput.right = true;
						break;
				case ActionName::SELECT_TILE:
						WorldManager::setPaused(true);
						std::cout << "entering sentencing mode - time paused" << std::endl;
						m_controlState = MapControlState::SENTENCING;
						break;
				case ActionName::OPEN_INVENTORY:
						WorldManager::setPaused(true);
						std::cout << "entering inventory - time paused" << std::endl;
						m_game->changeScene("inventory", std::make_shared<SceneInventory>(m_game));
				default:
						break;
				}
				break;
		case ActionType::END:
				switch(act)
				{
				case ActionName::MOVE_UP:
						cinput.up = false;
						break;
				case ActionName::MOVE_DOWN:
						cinput.down = false;
						break;
				case ActionName::MOVE_LEFT:
						cinput.left = false;
						break;
				case ActionName::MOVE_RIGHT:
						cinput.right = false;
						break;
				default:
						break;
				}
				break;
		case ActionType::NONE:
				break;
		}
}

void SceneMain::renderCaseMarkers()
{
		if(m_availableCases.empty()) return;

		float startY = 505.f; float lineHeight = 30.f;

		auto getCaseName = [](Case c) -> std::string {
				switch(c)
				{
				case Case::DIRECT: return "si/an";
				case Case::INDIRECT: return "ni/kan";
				case Case::OBLIQUE: return "sa/ki";
				default: return "Unknown Case";
				}
		};

		for(int i = 0; i < (int)m_availableCases.size(); i++)
		{
				float yPos = startY + i * lineHeight;

				if(i == m_currentCaseIndex)
				{
						sf::RectangleShape selector({560.f, 25.f});
						selector.setPosition({10.f, yPos - 2.f});
						selector.setFillColor(sf::Color(255, 255, 0, 50));
						selector.setOutlineThickness(2.f);
						selector.setOutlineColor(sf::Color::Yellow);
						m_game->window().draw(selector);
				}

				std::string caseName = getCaseName(m_availableCases[i]);
				sf::Text caseText = createText(caseName, m_font, {20.f, yPos}, 12, sf::Color::White);
				m_game->window().draw(caseText);
		}

		/*	if(m_selectedEntity)
			{
			std::string entityInfo = "Marking entity: ";

			if(m_selectedEntity->hasComponent<CPopulation>())
			{
			auto& pop = m_selectedEntity->getComponent<CPopulation>();
			entityInfo += (pop.ethnicity == Ethnicity::NATIVE_CIV)
			? "Native Civilian" : "Imperial Civilian";
			}
			else if(m_selectedEntity->hasComponent<CBuilding>())
			{
			auto& building = m_selectedEntity->getComponent<CBuilding>();
			switch(building.type)
			{
			case Building::HOME: entityInfo += "Home"; break;
			case Building::PLAZA: entityInfo += "Plaza"; break;
			case Building::SCHOOL: entityInfo += "School"; break;
			case Building::WORKPLACE: entityInfo += "Workplace"; break;
			case Building::GOVERNMENT: entityInfo += "Government"; break;
			default: entityInfo += "Building"; break;
			}
			}

			sf::Text infoText = createText(entityInfo, m_font, {20.f, 485.f}, 10, sf::Color::Cyan);
			m_game->window().draw(infoText);
			}*/
}

void SceneMain::renderCursor()
{
		for(auto& e : m_entities.getEntities("cursor"))
		{
				if(e->hasComponent<CTransform>() && e->hasComponent<CShape>() && e->hasComponent<CInput>())
				{
						auto& shape = e->getComponent<CShape>().shape;
						auto& transform = e->getComponent<CTransform>();
						shape->setPosition(transform.position.x, transform.position.y);
						m_game->window().draw(*shape);
				}
		}
}

void SceneMain::renderTileEntities(std::vector<std::shared_ptr<Entity>> tileEntities)
{
		if (tileEntities.empty())
		{
				// Even if empty, show the pronoun option
				float yPos = 505.f;

				if (0 == m_currentTileEntity)
				{
						sf::RectangleShape selector({560.f, 25.f});
						selector.setPosition({10.f, yPos - 2.f});
						selector.setFillColor(sf::Color(255, 255, 0, 50));
						selector.setOutlineThickness(2.f);
						selector.setOutlineColor(sf::Color::Yellow);
						m_game->window().draw(selector);
				}

				std::string text = "Use Pronoun (ako, ko, sako, etc.)";
				sf::Text displayText = createText(text, m_font, {20.f, yPos}, 12, sf::Color::Cyan);
				m_game->window().draw(displayText);

				return;
		}

		int startIdx = m_sentencingPage * m_entitiesPerPage;
		int endIdx = std::min(startIdx + m_entitiesPerPage, (int)tileEntities.size());

		/* sf::RectangleShape clearRect({580.f, 110.f});
		   clearRect.setPosition({10.f, 490.f});
		   clearRect.setFillColor(sf::Color::Red);
		   m_game->window().draw(clearRect); */

		float startY = 505.f;
		float lineHeight = 30.f; // Space between lines

		//m_game->window().draw(m_entityIndicator);

		for (int i = startIdx; i < endIdx; i++)
		{
				const auto& entity = tileEntities[i];
				float yPos = startY + (i - startIdx) * lineHeight;

				if (i == m_currentTileEntity)
				{
						sf::RectangleShape selector({560.f, 25.f});
						selector.setPosition({10.f, yPos - 2.f});
						selector.setFillColor(sf::Color(255, 255, 0, 50)); // Semi-transparent yellow
						selector.setOutlineThickness(2.f);
						selector.setOutlineColor(sf::Color::Yellow);
						m_game->window().draw(selector);
				}

				if (entity->hasComponent<CPopulation>())
				{
						auto& pop = entity->getComponent<CPopulation>();
						std::string text;
						sf::Color color;

						switch(pop.ethnicity)
						{
						case Ethnicity::NATIVE_CIV:
								text = "Native Civilian";
								color = sf::Color::Green;
								break;
						case Ethnicity::IMPERIAL_CIV:
								text = "Imperial Civilian";
								color = sf::Color::Red;
								break;
						}

						if (pop.speakingLanguage == Language::NATIVE)
								text += " (Native Speaker)";
						else if (pop.speakingLanguage == Language::IMPERIAL)
								text += " (Imperial Speaker)";

						sf::Text displayText = createText(text, m_font, {20.f, yPos}, 12, color);
						m_game->window().draw(displayText);
				}
				else if (entity->hasComponent<CBuilding>())
				{
						auto& building = entity->getComponent<CBuilding>();
						std::string buildingName;

						switch(building.type)
						{
						case Building::HOME: buildingName = "Home"; break;
						case Building::PLAZA: buildingName = "Plaza"; break;
						case Building::SCHOOL: buildingName = "School"; break;
						case Building::WORKPLACE: buildingName = "Workplace"; break;
						case Building::GOVERNMENT: buildingName = "Government"; break;
						default: buildingName = "Building"; break;
						}

						sf::Text displayText = createText(buildingName, m_font, {20.f, yPos}, 12, sf::Color::Yellow);
						m_game->window().draw(displayText);
				}
		}
		// Add "Use Pronoun" option at the end
		int pronounOptionIndex = tileEntities.size();
		if(pronounOptionIndex >= startIdx && pronounOptionIndex < startIdx + m_entitiesPerPage)
		{
				float yPos = startY + (pronounOptionIndex - startIdx) * lineHeight;

				if (pronounOptionIndex == m_currentTileEntity)
				{
						sf::RectangleShape selector({560.f, 25.f});
						selector.setPosition({10.f, yPos - 2.f});
						selector.setFillColor(sf::Color(255, 255, 0, 50));
						selector.setOutlineThickness(2.f);
						selector.setOutlineColor(sf::Color::Yellow);
						m_game->window().draw(selector);
				}

				std::string text = "Use Pronoun (ako, ko, sako, etc.)";
				sf::Text displayText = createText(text, m_font, {20.f, yPos}, 12, sf::Color::Cyan);
				m_game->window().draw(displayText);
		}

		if (tileEntities.size() + 1 > static_cast<size_t>(m_entitiesPerPage)) // +1 for pronoun option
		{
				int totalPages = (tileEntities.size() + 1 + m_entitiesPerPage - 1) / m_entitiesPerPage;
				std::string pageInfo = "Page " + std::to_string(m_sentencingPage + 1) +
						"/" + std::to_string(totalPages);
				sf::Text pageText = createText(pageInfo, m_font, {20.f, 590.f}, 10, sf::Color::White);
				m_game->window().draw(pageText);
		}
}

void SceneMain::renderPronounSelection()
{
		float startY = 485.f;
		float lineHeight = 18.f;

		sf::Text titleText = createText("Select pronoun form:", m_font, {20.f, 490.f}, 12, sf::Color::White);
		m_game->window().draw(titleText);

		startY = 510.f;

		int startIdx = m_pronounPage * m_pronounsPerPage;
		int endIdx = std::min(startIdx + m_pronounsPerPage, (int)m_pronounOptions.size());

		for(int i = startIdx; i < endIdx; i++)
		{
				float yPos = startY + (i - startIdx) * lineHeight;

				// Draw selection indicator
				if(i == m_currentPronounIndex)
				{
						sf::RectangleShape selector({560.f, 16.f});
						selector.setPosition({10.f, yPos - 2.f});
						selector.setFillColor(sf::Color(255, 255, 0, 50));
						selector.setOutlineThickness(2.f);
						selector.setOutlineColor(sf::Color::Yellow);
						m_game->window().draw(selector);
				}

				// Draw pronoun option with hint
				std::string pronounText = m_pronounOptions[i];

				// Add case hints
				if(pronounText == "ako" || pronounText == "ika" || pronounText == "siya")
						pronounText += " (Direct - subject)";
				else if(pronounText == "ko" || pronounText == "mo" || pronounText == "niya")
						pronounText += " (Indirect - possessor/agent)";
				else if(pronounText == "sako" || pronounText == "saimo" || pronounText == "saiya")
						pronounText += " (Oblique - location/beneficiary)";

				sf::Text optionText = createText(pronounText, m_font, {20.f, yPos}, 10, sf::Color::Cyan);
				m_game->window().draw(optionText);
		}

		// Show page info if multiple pages
		if (m_pronounOptions.size() > m_pronounsPerPage)
		{
				int totalPages = (m_pronounOptions.size() + m_pronounsPerPage - 1) / m_pronounsPerPage;
				std::string pageInfo = "Page " + std::to_string(m_pronounPage + 1) +
						"/" + std::to_string(totalPages);
				sf::Text pageText = createText(pageInfo, m_font, {20.f, 590.f}, 10, sf::Color::White);
				m_game->window().draw(pageText);
		}
}

void SceneMain::renderBuildings()
{
		for(auto& e : m_entities.getEntities("building"))
		{
				if(e->hasComponent<CBuilding>() && e->hasComponent<CTransform>() && e->hasComponent<CShape>())
				{
						auto& shape = e->getComponent<CShape>().shape;
						auto& transform = e->getComponent<CTransform>();
						shape->setPosition(transform.position.x, transform.position.y);
						m_game->window().draw(*shape);
				}
		}
}

void SceneMain::makeCursor()
{
		m_cursorEntity = m_entities.addEntity("cursor");
		m_cursorEntity->addComponent<CTransform>();
		auto& transform = m_cursorEntity->getComponent<CTransform>();

		const auto& buildings = m_entities.getEntities("building");
		if (!buildings.empty())
		{
				auto& bpos = buildings[0]->getComponent<CTransform>().position;
				transform.position = bpos;
		}
		else
		{
				transform.position = sf::Vector2f(100.f, 100.f);
		}

		m_cursorEntity->addComponent<CInput>();

		auto shape = std::make_shared<sf::RectangleShape>(sf::Vector2f(20.f, 20.f));
		shape->setFillColor(sf::Color::Yellow);
		m_cursorEntity->addComponent<CShape>(shape);
}

void SceneMain::updateCursorPos()
{
		auto& transform = m_cursorEntity->getComponent<CTransform>();
		auto& input = m_cursorEntity->getComponent<CInput>();

		const float tileSize = 20.f;

		sf::Vector2f originalPos = transform.position;

		if(input.up)
		{
				sf::Vector2f targetPos = transform.position;
				targetPos.y -= tileSize;

				if(isValidTilePosition(targetPos))
				{
						transform.position = targetPos;
				}
				input.up = false;
		}
		if(input.down)
		{
				sf::Vector2f targetPos = transform.position;
				targetPos.y += tileSize;

				if(isValidTilePosition(targetPos))
				{
						transform.position = targetPos;
				}
				input.down = false;
		}
		if(input.left)
		{
				sf::Vector2f targetPos = transform.position;
				targetPos.x -= tileSize;

				if(isValidTilePosition(targetPos))
				{
						transform.position = targetPos;
				}
				input.left = false;
		}
		if(input.right)
		{
				sf::Vector2f targetPos = transform.position;
				targetPos.x += tileSize;

				if(isValidTilePosition(targetPos))
				{
						transform.position = targetPos;
				}
				input.right = false;
		}
}

void SMain::renderTileGrid()
{
		for(int y = 0; y < m_mapManager.getHeight(); y++)
		{
				for(int x = 0; x < m_mapManager.getWidth(); x++)
				{
						auto& tileShape = m_tileOutlineShape->getComponent<CShape>().shape;
						tileShape->setPosition(x*20.f, y*20.f);
						m_game->window().draw(*tileShape);
				}
		}
}

void SceneMain::renderPaths()
{
		for(auto& e : m_entities.getEntities("path"))
		{
				if(e->hasComponent<CTransform>() && e->hasComponent<CShape>() && e->hasComponent<CPath>())
				{
						auto& shape = e->getComponent<CShape>().shape;
						auto& transform = e->getComponent<CTransform>();

						shape->setPosition(transform.position.x, transform.position.y);
						m_game->window().draw(*shape);
				}
		}
}

int SceneMain::getPathTileCount()
{
		auto& paths = m_entities.getEntities("path");
		return static_cast<int>(paths.size());
}

std::string SceneMain::getEntityDisplayName(const std::shared_ptr<Entity>& entity)
{
		if(!entity) return "[None]";

		if(entity->hasComponent<CPopulation>())
		{
				auto& pop = entity->getComponent<CPopulation>();
				return(pop.ethnicity == Ethnicity::NATIVE_CIV)
						? "Native Civilian" : "Imperial Civilian";
		}
		else if(entity->hasComponent<CBuilding>())
		{
				auto& building = entity->getComponent<CBuilding>();
				switch(building.type)
				{
				case Building::HOME: return "harong";
				case Building::PLAZA: return "plaza";
				case Building::SCHOOL: return "eskwelahan";
				case Building::WORKPLACE: return "trabahuhan";
				case Building::GOVERNMENT: return "gobierno";
				default: return "lugar";
				}
		}
		return "[Unknown]";
}

std::string SceneMain::getCaseMarkerString(Case caseType, bool isProper)
{
		switch(caseType)
		{
		case Case::DIRECT:
				return isProper ? "si" : "an";
		case Case::INDIRECT:
				return isProper ? "ni" : "kan";
		case Case::OBLIQUE:
				return isProper ? "sa" : "ki";
		default:
				return "";
		}
}

void SceneMain::renderSentence()
{
		std::string sentenceStr = "";
		float xPos = 10.f;
		float yPos = 458.f;

		if(m_currentSentence.hasFirstArgument)
		{
				if(!m_currentSentence.firstArgument.isPronoun && m_currentSentence.firstArgument.entity)
				{
						bool isProper = m_currentSentence.firstArgument.entity->hasComponent<CPopulation>();
						std::string marker = getCaseMarkerString(m_currentSentence.firstArgument.caseType, isProper);
						sentenceStr += marker + " " + m_currentSentence.firstArgument.entityName;
						sentenceStr += " ";
				}
				else if(m_currentSentence.firstArgument.isPronoun)
				{
						sentenceStr += m_currentSentence.firstArgument.pronounForm;
				}
				else
				{
						sentenceStr += "[Error: No entity]";
				}
				sentenceStr += " ";
		}
		else if (m_controlState == MapControlState::MARKING_ENTITY && m_selectingFirstEntity && m_selectedEntity)
		{
				// Show selected entity while marking (before case is chosen)
				std::string entityName = getEntityDisplayName(m_selectedEntity);
				sentenceStr += "[Select Marker] " + entityName + " ";
		}
		else
		{
				sentenceStr += "[Select Entity]";
		}

		sentenceStr += " ";

		if(m_currentSentence.hasVoice && !m_currentSentence.conjugatedVerb.empty())
		{
				sentenceStr += m_currentSentence.conjugatedVerb;
		}
		else if(m_currentSentence.hasVerb)
		{
				sentenceStr += m_currentSentence.verb + " [needs conjugation]";
		}
		else
		{
				sentenceStr += "[Select Verb]";
		}

		sentenceStr += " ";

		if(m_currentSentence.hasSecondArgument)
		{
				if(!m_currentSentence.secondArgument.isPronoun && m_currentSentence.secondArgument.entity)
				{
						bool isProper = m_currentSentence.secondArgument.entity->hasComponent<CPopulation>();
						std::string marker = getCaseMarkerString(m_currentSentence.secondArgument.caseType, isProper);
						sentenceStr += marker + " " + m_currentSentence.secondArgument.entityName;
				}
				else if(m_currentSentence.secondArgument.isPronoun)
				{
						sentenceStr += m_currentSentence.secondArgument.pronounForm;
				}
				else
				{
						sentenceStr += "[Error: No entity]";
				}
		}
		else if(m_currentSentence.hasVerb)
		{
				sentenceStr += "[Select Entity]";
		}

		sf::Text sentenceText = createText(sentenceStr, m_font, {xPos, yPos}, 14, sf::Color::White);
		m_game->window().draw(sentenceText);

		std::string stageInfo = "";
		switch(m_controlState)
		{
		case MapControlState::SENTENCING: //edit after thursday, wrote too much to edit pa lol
		case MapControlState::MARKING_ENTITY: //merge sentencing and marking entity into one
				if(m_selectingFirstEntity)
						stageInfo = "Stage: Marking first entity";
				else
						stageInfo = "Stage: Marking second entity";
				break;
        case MapControlState::SELECTING_VERB:
				stageInfo = "Stage: Selecting verb";
				break;
        case MapControlState::CONJUGATING_VERB:
				stageInfo = "Stage: Selecting voice";
				break;
		case MapControlState::EXECUTE_SENTENCE:
				stageInfo = "Stage: Ready to execute (SPACE)";
				break;
        default:
				break;
		}

		if(!stageInfo.empty())
		{
				sf::Text stageText = createText(stageInfo, m_font, {xPos, yPos + 20.f}, 10, sf::Color(150, 150, 150));
				m_game->window().draw(stageText);
		}
}

void SceneMain::renderVerbSelection()
{
		float startY = 485.f;
		float lineHeight = 18.f;

		// Clear the area
		/*sf::RectangleShape clearRect({580.f, 120.f});
		  clearRect.setPosition({10.f, 480.f});
		  clearRect.setFillColor(sf::Color::Black);
		  clearRect.setOutlineThickness(1.f);
		  clearRect.setOutlineColor(sf::Color::White);
		  m_game->window().draw(clearRect);*/

		//	sf::Text titleText = createText("Select a verb:", m_font, {20.f, 490.f}, 12, sf::Color::White);
		//m_game->window().draw(titleText);

		startY = 510.f;

		int startIdx = m_verbPage * m_verbsPerPage;
		int endIdx = std::min(startIdx + m_verbsPerPage, (int)m_availableVerbs.size());

		for(int i = startIdx; i < endIdx; i++)
		{
				float yPos = startY + (i - startIdx) * lineHeight;

				if(i == m_currentVerbIndex)
				{
						sf::RectangleShape selector({560.f, 16.f});
						selector.setPosition({10.f, yPos - 2.f});
						selector.setFillColor(sf::Color(255, 255, 0, 50));
						selector.setOutlineThickness(2.f);
						selector.setOutlineColor(sf::Color::Yellow);
						m_game->window().draw(selector);
				}

				// Get verb details from lexicon manager
				std::string verbText = m_availableVerbs[i];

				// You could add imperial translation here if available
				// auto& lexiconMgr = m_game->getLexiconManager();
				// std::string imperialTranslation = ...;

				sf::Text optionText = createText(verbText, m_font, {20.f, yPos}, 10, sf::Color::Cyan);
				m_game->window().draw(optionText);
		}

		if (m_availableVerbs.size() > m_verbsPerPage)
		{
				int totalPages = (m_availableVerbs.size() + m_verbsPerPage - 1) / m_verbsPerPage;
				std::string pageInfo = "Page " + std::to_string(m_verbPage + 1) +
						"/" + std::to_string(totalPages);
				sf::Text pageText = createText(pageInfo, m_font, {20.f, 590.f}, 10, sf::Color::White);
				m_game->window().draw(pageText);
		}
}

void SceneMain::renderVerbConjugation()
{
		float startY = 485.f;
		float lineHeight = 18.f;

		// Clear the area
		/*sf::RectangleShape clearRect({580.f, 120.f});
		  clearRect.setPosition({10.f, 480.f});
		  clearRect.setFillColor(sf::Color::Black);
		  clearRect.setOutlineThickness(1.f);
		  clearRect.setOutlineColor(sf::Color::White);
		  m_game->window().draw(clearRect);*/

		// Show current verb
		//	sf::Text verbText = createText("Verb: " + m_currentSentence.verb,
		// m_font, {20.f, 490.f}, 12, sf::Color::Green);
//m_game->window().draw(verbText);

		startY = 510.f;

		if (m_selectingVoice)
		{
				// Render voice selection
				//	sf::Text titleText = createText("Select voice:", m_font, {20.f, 505.f}, 10, sf::Color::White);
				//m_game->window().draw(titleText);

				for(int i = 0; i < (int)m_availableVoices.size(); i++)
				{
						float yPos = startY + i * lineHeight;

						if(i == m_currentVoiceIndex)
						{
								sf::RectangleShape selector({560.f, 16.f});
								selector.setPosition({10.f, yPos - 2.f});
								selector.setFillColor(sf::Color(255, 255, 0, 50));
								selector.setOutlineThickness(2.f);
								selector.setOutlineColor(sf::Color::Yellow);
								m_game->window().draw(selector);
						}

						std::string voiceName = m_availableVoices[i];
						// Add description
						if (voiceName == "actor") voiceName += " (does the action)";
						else if (voiceName == "patient") voiceName += " (receives the action)";
						else if (voiceName == "locative") voiceName += " (location of action)";

						sf::Text optionText = createText(voiceName, m_font, {20.f, yPos}, 10, sf::Color::Cyan);
						m_game->window().draw(optionText);
				}
		}
}

bool SceneMain::isValidTilePosition(const sf::Vector2f& pos)
{
		auto& pathEnt = m_entities.getEntities("path");
		for(auto& path : pathEnt)
		{
				if(path->hasComponent<CTransform>())
				{
						auto& entityPos = path->getComponent<CTransform>().position;
						if(entityPos.x == pos.x && entityPos.y == pos.y)
								return true;
				}
		}

		auto& buildEnt = m_entities.getEntities("building");
		for(auto& build : buildEnt)
		{
				if(build->hasComponent<CTransform>())
				{
						auto& entityPos = build->getComponent<CTransform>().position;
						if(entityPos.x == pos.x && entityPos.y == pos.y)
								return true;
				}
		}

		return false;
}

void SceneMain::handleExecuteSentence(const Action& action, ActionName act)
{
		if(action.type() != ActionType::START) return;

		switch(act)
		{
        case ActionName::EXECUTE_SENTENCE: // Space key
        {
				if(m_awaitingConfirmation && m_sentenceComplete)
				{
						std::cout << "\n=== EXECUTING SENTENCE ===" << std::endl;

						// Validate the sentence
						auto validation = m_verbManager.validateSentence(m_currentSentence);

						if(validation.isValid)
						{
								std::cout << "Sentence is valid. Executing..." << std::endl;
								m_verbManager.executeSentence(m_currentSentence);

								// Reset for next sentence
								m_currentSentence.clear();
								m_selectedEntity = nullptr;
								m_selectingFirstEntity = true;
								m_sentenceComplete = false;
								m_awaitingConfirmation = false;
								m_controlState = MapControlState::NAVIGATING;
								WorldManager::setPaused(false);
						}
						else
						{
								std::cout << "Sentence validation failed: "
										  << validation.errorMessage << std::endl;
								// Show error to player but stay in execute state
								// Could display error message in UI
						}
				}
        }
        break;
		case ActionName::RETURN: // Q key - cancel execution
		{
				std::cout << "Sentence execution cancelled." << std::endl;
				m_currentSentence.clear();
				m_selectedEntity = nullptr;
				m_selectingFirstEntity = true;
				m_sentenceComplete = false;
				m_awaitingConfirmation = false;
				m_controlState = MapControlState::NAVIGATING;
				WorldManager::setPaused(false);
		}
		break;
		default:
				break;
		}
}

std::shared_ptr<Entity> SceneMain::getPlayerEntity()
{
		return m_game->getWorldManager()->getPlayer();
}

Building SceneMain::getBuildingType(const std::string& buildingName)
{
		if(buildingName == "harong") return Building::HOME;
		if(buildingName == "plaza") return Building::PLAZA;
		if(buildingName == "eskwelahan") return Building::SCHOOL;
		if(buildingName == "trabahuhan") return Building::WORKPLACE;
		if(buildingName == "gobierno") return Building::GOVERNMENT;

		return Building::HOME; // default
}

void SceneMain::updateExecutionSentence(std::string& help1, std::string& help2)
{
		help1 = "Sentence complete!\nPress SPACE to execute";
		help2 = "Press Q to cancel";
}

std::string SceneMain::getCurrentHint()
{
    auto& worldMgr = *m_game->getWorldManager();

    // Calculate percentages
    float nativePercent = (worldMgr.nativeSpeakers * 100.0f) / worldMgr.population;
    float imperialPercent = (worldMgr.imperialSpeakers * 100.0f) / worldMgr.population;

    // Critical situations
    if (nativePercent < 15.0f)
        return "CRITICAL! Native language near extinction! Convert more speakers NOW!";

    if (imperialPercent > 85.0f)
        return "WARNING: Imperial dominance very high. Find and convert speakers quickly!";

    // Positive progress
    if (nativePercent > 85.0f)
        return "EXCELLENT! Native language thriving! Keep it up to win!";

    if (nativePercent > 70.0f)
        return "GOOD PROGRESS! Native language spreading well!";

    // Guidance hints
    if (worldMgr.currentYear < 5)
        return "TIP: Use 'kaulay' verb to talk with Imperial speakers and teach them Native.";

    if (worldMgr.currentYear < 10)
        return "TIP: Convert near Native speakers for better success rates!";

    // Population concerns
    if (worldMgr.population < 50)
        return "Population declining. Focus on language before it's too late.";

    // Default strategic hints
    static int hintRotation = 0;
    hintRotation = (hintRotation + 1) % 3;

    switch(hintRotation)
    {
    case 0:
        return "Press TAB to study the lexicon and learn translations.";
    case 1:
        return "Children learn the dominant language - convert to shift the balance!";
    case 2:
        return "Native ethnicity parents raise Native speakers if language is strong.";
    default:
        return "";
    }
}

void SceneMain::updateHintSystem()
{
		m_hintTimer++;

		// Update hint every 3 seconds (180 frames at 60fps)
		if (m_hintTimer >= 180)
		{
				m_statusHint.setString(getCurrentHint());
				m_hintTimer = 0;
		}
}

void SceneMain::updateProgressBar()
{
		auto& worldMgr = *m_game->getWorldManager();

		if (worldMgr.population == 0) return;

		float nativePercent = static_cast<float>(worldMgr.nativeSpeakers) / worldMgr.population;
		float imperialPercent = static_cast<float>(worldMgr.imperialSpeakers) / worldMgr.population;

		float barWidth = 586.f;

		m_progressBarNative.setSize({barWidth * nativePercent, 20.f});
		m_progressBarImperial.setSize({barWidth * imperialPercent, 20.f});
		m_progressBarImperial.setPosition({7.f + barWidth * nativePercent, 58.f});
}
