#pragma once

#include "Scene.h"
#include "ManagerLexicon.h"
#include "GrammarBicol.h"
#include <memory>
#include <map>
#include <vector>

class BicolGrammar;

class SceneMain : public Scene
{
protected:
		enum class ActionName
		{
				MOVE_UP,
				MOVE_DOWN,
				MOVE_LEFT,
				MOVE_RIGHT,
				SELECT_TILE,
				OPEN_INVENTORY,
				SELECT_ENTITY,
				RETURN,
				NEXT_PAGE,
				PREV_PAGE,
				UP,
				DOWN,
				CONFIRM_SELECTION,
				EXECUTE_SENTENCE
		};

		enum class MapControlState
		{
				NAVIGATING,           //in sentencing, we navigate the entities using L/R
				SENTENCING,           //we press enter to select entity, we are now in selecting marker
                SELECTING_PRONOUN,
				SELECTING_ENTITY,     //markers appear in the panel, we can copy the logic of entity render
				MARKING_ENTITY,     //press enter again to select, then we are now in selecting verb
				SELECTING_VERB,           //remove the aspect for now, but we keep the voice
				CONJUGATING_VERB,
				EXECUTE_SENTENCE, //we send the sentence to validator
				COUNT
		};

		void updateNavigating(std::string& help1, std::string& help2);
		void updateSentencing(std::string& help1, std::string& help2);
		void updateSelectingPronoun(std::string& help1, std::string& help2);
		void updateSelectingEntity(std::string& help1, std::string& help2);
		void updateMarkingEntity(std::string& help1, std::string& help2);
		void updateSelectingVerb(std::string& help1, std::string& help2);
		void updateConjugatingVerb(std::string& help1, std::string& help2);
		void updateExecutionSentence(std::string& help1, std::string& help2);
		void updateConfirmSelection(std::string& help1, std::string& help2);

		using updateStateFunc = void(SceneMain::*)(std::string& help1, std::string& help2);
		static constexpr std::array<updateStateFunc, (size_t)MapControlState::COUNT> m_updateTable = {
				&SceneMain::updateNavigating,
				&SceneMain::updateSentencing,
				&SceneMain::updateSelectingPronoun,
				&SceneMain::updateSelectingEntity,
				&SceneMain::updateMarkingEntity,
				&SceneMain::updateSelectingVerb,
				&SceneMain::updateConjugatingVerb
		};

		void handleCursorNavigation(const Action& action, ActionName act);
		void handleSentencing(const Action& action, ActionName act);
		void handleSelectingPronoun(const Action& action, ActionName act);
		void handleSelectingEntity(const Action& action, ActionName act);
		void handleMarkingEntity(const Action& action, ActionName act);
		void handleSelectingVerb(const Action& action, ActionName act);
		void handleConjugatingVerb(const Action& action, ActionName act);
		void handleExecuteSentence(const Action& action, ActionName act);
		void handleConfirmSelection(const Action& action, ActionName act);

		using actionStateFunc = void(SceneMain::*)(const Action& action, ActionName act);
		static constexpr std::array<actionStateFunc, (size_t)MapControlState::COUNT> m_actionTable = {
				&SceneMain::handleCursorNavigation,
				&SceneMain::handleSentencing,
				&SceneMain::handleSelectingPronoun,
				&SceneMain::handleSelectingEntity,
				&SceneMain::handleMarkingEntity,
				&SceneMain::handleSelectingVerb,
				&SceneMain::handleConjugatingVerb,
		};

		sf::View m_mainView;

		std::shared_ptr<Entity> m_cursorEntity;
		std::shared_ptr<Entity> m_tileOutlineShape;
		std::vector<std::shared_ptr<Entity>> m_tileEntities;

		MapControlState m_controlState;

		std::vector<std::shared_ptr<Entity>> m_nativeEntities;
		std::vector<std::shared_ptr<Entity>> m_imperialEntities;

		void init();
		void update() override;
		void sDoAction(const Action& action) override;
		void onEnd() override;

		void updateCursorPos();
		bool isValidTilePosition(const sf::Vector2f& pos);

		std::shared_ptr<Entity> getTileAtCursorPosition(const sf::Vector2f& pos);
		std::vector<std::shared_ptr<Entity>> getTileEntities(const sf::Vector2f& pos);

		void makeMapView();
		void makeMapGrid();
		void makeCursor();
		void makeBuildings();
		void makePaths();
		void makeStatistics();
		void updateStatistics();

		void renderTileGrid();
		void renderCursor();
		void renderBuildings();
		void renderPaths();
		void renderTileEntities(std::vector<std::shared_ptr<Entity>> tileEntities);

		sf::Font m_font;

		sf::Text m_helpMsg;
		sf::Text m_helpMsg2;

		sf::RectangleShape m_sentencePanel;
		sf::RectangleShape m_sentencingPanel;

		sf::Text m_totalPopulation;
		sf::Text m_year;
		sf::Text m_nativEthnicity;
		sf::Text m_imperialEthnicity;
		sf::Text m_imperialSpeakers;
		sf::Text m_nativeSpeakers;

		int m_sentencingPage = 0;
		int m_entitiesPerPage = 3;
		std::vector<std::shared_ptr<Entity>> m_currentTileEntities;
		int m_currentTileEntity = 0;

		std::shared_ptr<Entity> m_selectedEntity;
		std::vector<Case> m_availableCases;
		int m_currentCaseIndex = 0;
		sf::RectangleShape m_markerPanel;
		void renderCaseMarkers();

		BicolGrammar::Sentence m_currentSentence;
		bool m_selectingFirstEntity = true;
		void renderSentence();
		std::string getEntityDisplayName(const std::shared_ptr<Entity>& entity);
		std::string getCaseMarkerString(Case caseType, bool isProper = false);

		std::vector<std::string> m_pronounOptions;
		int m_currentPronounIndex = 0;
		void createPronounOptions();
		void renderPronounSelection();
		int m_pronounPage = 0;
		int m_pronounsPerPage = 3;

		std::vector<std::string> m_availableVerbs;
		int m_currentVerbIndex = 0;
		int m_verbPage = 0;
		int m_verbsPerPage = 3;
		void renderVerbSelection();

		std::vector<std::string> m_availableVoices;
		std::vector<std::string> m_availableAspects;
		int m_currentVoiceIndex = 0;
		int m_currentAspectIndex = 0;
		int m_conjugationPage = 0;
		int m_conjugationItemsPerPage = 5;
		bool m_selectingVoice = true;
		void renderVerbConjugation();

		sf::Text m_statusHint;
		int m_hintTimer = 0;
		std::string getCurrentHint();
		void updateHintSystem();

		sf::RectangleShape m_progressBarBackground;
		sf::RectangleShape m_progressBarNative;
		sf::RectangleShape m_progressBarImperial;
		void updateProgressBar();

public:
		SceneMain(GameEngine* gameEngine);
		void sRender() override;
		int getPathTileCount();

		bool m_sentenceComplete = false;
		bool m_awaitingConfirmation = false;

		std::shared_ptr<Entity> getPlayerEntity();
		Building getBuildingType(const std::string& buildingName);
};
