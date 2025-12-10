#pragma once

#include "Scene.h"
#include "Entity.h"
#include <memory>
#include <vector>
#include <string>

class SceneDialogue :  public Scene
{
protected:
		enum class ActionName
		{
				UP,
						DOWN,
						ENTER,
						RETURN,
						SUBMIT
						};

		enum class DialogueState
		{
				SHOWING_PROMPT,
						ENTERING_TRANSLATION,
						SHOWING_RESULT,
						COUNT
						};

		DialogueState m_state;

		std::shared_ptr<Entity> m_targetCivilian;
		std::string m_imperialWord;
		std::string m_correctTranslation;
		int m_nearbyNativeCivs;
		int m_nearbyImperialCivs;

		// UI elements
		sf::Font m_font;
		sf::Text m_promptText;
		sf::Text m_wordToTranslate;
		sf::Text m_inputLabel;
		sf::Text m_userInput;
		sf::Text m_resultText;
		sf::Text m_helpText;
		sf::RectangleShape m_dialogueBox;
		sf::RectangleShape m_inputBox;

		std::string m_playerTranslation;
		bool m_translationCorrect;
		float m_successChance;

		void init();
		void calculateSuccessChance();
		void checkTranslation();
		void applyConversionResult();

		void updateShowingPrompt(std::string& help);
		void updateEnteringTranslation(std::string& help);
		void updateShowingResult(std::string& help);
		void handleShowingPrompt(const Action& action, ActionName act);
		void handleEnteringTranslation(const Action& action, ActionName act);
		void handleShowingResult(const Action& action, ActionName act);

		using updateStateFunc = void(SceneDialogue::*)(std::string& help);
		static constexpr std::array<updateStateFunc, (size_t)DialogueState::COUNT> m_updateTable = {
				&SceneDialogue::updateShowingPrompt,
				&SceneDialogue::updateEnteringTranslation,
				&SceneDialogue::updateShowingResult
		};

		using actionStateFunc = void(SceneDialogue::*)(const Action& action, ActionName act);
		static constexpr std::array<actionStateFunc, (size_t)DialogueState::COUNT> m_actionTable = {
				&SceneDialogue::handleShowingPrompt,
				&SceneDialogue::handleEnteringTranslation,
				&SceneDialogue::handleShowingResult
		};

		void onEnd() override;
		void update() override;
		void sDoAction(const Action& action) override;

public:
		SceneDialogue(GameEngine* gameEngine,
					  std::shared_ptr<Entity> targetCivilian,
					  const std::string& imperialWord,
					  const std::string& correctTranslation,
					  int nearbyNativeCivs,
					  int nearbyImperialCivs);

		void sRender() override;
};
