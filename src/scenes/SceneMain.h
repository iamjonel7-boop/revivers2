#pragma once

#include "Scene.h"
#include <memory>
#include <map>
#include <vector>

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
				RETURN
		};

		enum class MapControlState
		{
				NAVIGATING,
				SENTENCING,
				COUNT
		};

		void updateNavigating(std::string& help1, std::string& help2);
		void updateSentencing(std::string& help1, std::string& help2);

		using updateStateFunc = void(SceneMain::*)(std::string& help1, std::string& help2);
		static constexpr std::array<updateStateFunc, (size_t)MapControlState::COUNT> m_updateTable = {
				&SceneMain::updateNavigating,
				&SceneMain::updateSentencing
		};

		sf::View m_mainView;

		std::shared_ptr<Entity> m_cursorEntity;
		std::shared_ptr<Entity> m_tileOutlineShape;

		MapControlState m_controlState;

		void createPopulation();

		void init();
		void update() override;
		void sDoAction(const Action& action) override;
		void onEnd() override;

		void handleCursorNavigation(const Action& action, ActionName act, CInput& cinput);
		void handleSentencing(ActionName act);
		void updateCursorPos();

		void makeMapView();
		void makeMapGrid();
		void makeCursor();
		void makeBuildings();
		void makePaths();


		void renderTileGrid();
		void renderCursor();
		void renderBuildings();
		void renderPaths();

		sf::Font m_font;

		sf::Text m_helpMsg;
		sf::Text m_helpMsg2;

		sf::RectangleShape m_sentencePanel;
		sf::RectangleShape m_sentencingPanel;

public:
		SceneMain(GameEngine* gameEngine);
		void sRender() override;
};
