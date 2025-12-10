#pragma once

#include "ManagerEntity.h"
#include "ManagerTextInput.h"
#include "ManagerMap.h"
#include "ManagerLexicon.h"
#include "Actions.h"
#include "GrammarBicol.h"
#include "ManagerVerb.h"
#include <memory>
#include <map>

class GameEngine;
class VerbManager;

typedef std::map<int, int> ActionMap;
//the int key is the sfml key
//the value is player defined

class Scene
{
protected:
		GameEngine* m_game = nullptr;

		EntityManager m_entities;
		TextInputManager m_textInputManager;
		MapManager m_mapManager;
		LexiconManager m_lexiconManager;
		BicolGrammar m_bicolgrammar;
		VerbManager& m_verbManager;


		ActionMap m_actionMap;

		bool m_hasEnded = false;
		size_t m_currentFrame = 0;

		virtual void onEnd() = 0;

public:
		Scene();
		Scene(GameEngine* gameEngine);
		virtual ~Scene() = default;

		virtual void doAction(const Action& action);
		virtual void update() = 0;
		virtual void sDoAction(const Action& action) = 0;
		virtual void sRender() = 0;

		void simulate(const size_t frames);
		void registerAction(int inputKey, int actionName);

		const ActionMap& getActionMap() const;
		TextInputManager& getTextInputManager();
		EntityManager& getEntityManager();
		MapManager& getMapManager();
		LexiconManager& getLexiconManager();

		size_t currentFrame() const;
		bool hasEnded() const;

		sf::Text createText(const std::string& content, const sf::Font& font, sf::Vector2f pos, float charSize, sf::Color color) const;
		sf::RectangleShape createBox(sf::Vector2f pos, sf::Vector2f size, sf::Color color) const;

		int wrapIndex(int current, int delta, int size);
};
