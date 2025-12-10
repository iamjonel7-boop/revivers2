#include "SceneGameOver.h"
#include "GameEngine.h"
#include "ManagerWorld.h"
#include "SceneTitleScreen.h"
#include <sstream>

SceneGameOver::SceneGameOver(GameEngine* gameEngine, bool victory)
		: Scene(gameEngine)
		, m_isVictory(victory)
{
		// Capture final statistics
		m_finalYear = WorldManager::currentYear;
		m_finalNativeSpeakers = WorldManager::nativeSpeakers;
		m_finalImperialSpeakers = WorldManager::imperialSpeakers;
		m_finalPopulation = WorldManager::population;

		init();

		registerAction(sf::Keyboard::Enter, static_cast<int>(ActionName::ENTER));
		registerAction(sf::Keyboard::Escape, static_cast<int>(ActionName::ESCAPE));
}

void SceneGameOver::init()
{
    m_font.loadFromFile("/usr/local/share/fonts/Liberation/LiberationMono-Bold.ttf");

    // Setup background
    m_background.setSize({800.f, 600.f});
    m_background.setPosition({0.f, 0.f});
    m_background.setFillColor(m_isVictory ? sf::Color(0, 50, 0) : sf::Color(50, 0, 0));

    // Title
    std::string title = m_isVictory ? "VICTORY!" : "GAME OVER";
    m_titleText = createText(title, m_font, {250.f, 100.f}, 48,
                            m_isVictory ? sf::Color::Green : sf::Color::Red);

    // Message based on victory/defeat
    if (m_isVictory)
    {
        float nativePercent = (m_finalNativeSpeakers * 100.0f) / m_finalPopulation;
        std::ostringstream msg;
        msg << "The native language has prevailed!\n\n";
        msg << "After " << m_finalYear << " years of struggle,\n";
        msg << nativePercent << "% of the population speaks\n";
        msg << "the native tongue.\n\n";
        msg << "The language will survive for generations to come.";
        m_endMessage = msg.str();
    }
	else
    {
			std::ostringstream msg;
			msg << "The native language has faded...\n\n";

			if (WorldManager::nativEthnicity <= 0)
			{
					msg << "The native population has disappeared.\n";
					msg << "With them, their language is lost forever.";
			}
			else if (m_finalPopulation <= 10)
			{
					msg << "The population has collapsed.\n";
					msg << "Too few remain to keep the language alive.";
			}
			else
			{
					msg << "Imperial speakers vastly outnumber native speakers.\n";
					msg << "The younger generation no longer learns\n";
					msg << "their ancestral tongue.";
			}

			m_endMessage = msg.str();
    }

    m_messageText = createText(m_endMessage, m_font, {100.f, 200.f}, 16, sf::Color::White);

    // Statistics
    std::ostringstream stats;
    stats << "\n\nFinal Statistics:\n";
    stats << "Year: " << m_finalYear << "\n";
    stats << "Total Population: " << m_finalPopulation << "\n";
    stats << "Native Speakers: " << m_finalNativeSpeakers << "\n";
    stats << "Imperial Speakers: " << m_finalImperialSpeakers << "\n";

    m_statsText = createText(stats.str(), m_font, {100.f, 400.f}, 14, sf::Color(200, 200, 200));

    // Prompt
    m_promptText = createText("Press ENTER to return to menu | ESC to quit",
							  m_font, {150.f, 550.f}, 12, sf::Color::Yellow);
}

void SceneGameOver::update()
{
		// Static scene, nothing to update
}

void SceneGameOver::sDoAction(const Action& action)
{
		if (action.type() != ActionType::START) return;

		ActionName act = static_cast<ActionName>(action.name());

		switch(act)
		{
		case ActionName::ENTER:
				// Return to title screen
				m_game->changeScene("titleScreen", std::make_shared<TitleScreen>(m_game));
				break;
		case ActionName::ESCAPE:
				m_game->quit();
				break;
		}
}

void SceneGameOver::onEnd()
{
		// Cleanup if needed
}

void SceneGameOver::sRender()
{
		m_game->window().setView(m_game->window().getDefaultView());

		m_game->window().draw(m_background);
		m_game->window().draw(m_titleText);
		m_game->window().draw(m_messageText);
		m_game->window().draw(m_statsText);
		m_game->window().draw(m_promptText);
}
