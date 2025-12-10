#pragma once

#include "Scene.h"
#include <string>

class SceneGameOver : public Scene
{
protected:
		enum class ActionName
		{
				ENTER,
						ESCAPE
						};

		bool m_isVictory;
		std::string m_endMessage;
		int m_finalYear;
		int m_finalNativeSpeakers;
		int m_finalImperialSpeakers;
		int m_finalPopulation;

		sf::Font m_font;
		sf::Text m_titleText;
		sf::Text m_messageText;
		sf::Text m_statsText;
		sf::Text m_promptText;
		sf::RectangleShape m_background;

		void init();
		void onEnd() override;
		void update() override;
		void sDoAction(const Action& action) override;

public:
		SceneGameOver(GameEngine* gameEngine, bool victory);
		void sRender() override;
};
