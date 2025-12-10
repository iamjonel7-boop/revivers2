#pragma once

#include "ManagerLexicon.h"
#include "Scene.h"
#include <vector>

class SceneInventory : public Scene
{
        static const float TEXT_SIZE;
        static const float INPUT_SIZE;
        static const sf::Vector2f INPUT_FIELD_SIZE;
        static const sf::Vector2f PANEL_SIZE;

        static const sf::Vector2f INFO_PANEL_POS;
        static const sf::Vector2f LIST_PANEL_POS;
        static const sf::Vector2f NAT_NAME_FIELD_POS;
        static const sf::Vector2f IMP_NAME_FIELD_POS;
        static const sf::Vector2f WORD_CLASS_FIELD_POS;
        static const sf::Vector2f NAT_NAME_TEXT_POS;
        static const sf::Vector2f IMP_NAME_TEXT_POS;
        static const sf::Vector2f CLASS_TEXT_POS;
        static const sf::Vector2f NOUN_TEXT_POS;
        static const sf::Vector2f VERB_TEXT_POS;
        static const sf::Vector2f ADJ_TEXT_POS;
        static const sf::Vector2f NAT_NAME_INPUT_POS;
        static const sf::Vector2f IMP_NAME_INPUT_POS;
        static const sf::Vector2f WORD_CLASS_INPUT_POS;

        static const sf::Color INPUT_FIELD_COLOR;
        static const sf::Color PANEL_COLOR;
        static const sf::Color TEXT_COLOR;
        static const sf::Color INDICATOR_COLOR;

protected:
        enum class InventoryState
        {
                ADD_OR_LOOK,
                ADDING_WORD,
                LOOK_UP,
                ENTERING_NATIVE_WORD,
                ENTERING_IMPERIAL_WORD,
                ENTERING_WORD_CLASS,
				SELECTING,
                SAVING_WORD,
				COUNT
        };

		void updateAddOrLook(std::string& help1, std::string& help2);
		void updateAddingWord(std::string& help1, std::string& help2);
		void updateLookUp(std::string& help1, std::string& help2);
		void updateSelecting(std::string& help1, std::string& help2);

		using updateStateFunc = void(SceneInventory::*)(std::string& help1, std::string& help2);
		static constexpr std::array<updateStateFunc, (size_t)InventoryState::COUNT> m_updateTable = {
				&SceneInventory::updateAddOrLook,
				&SceneInventory::updateAddingWord,
				&SceneInventory::updateLookUp,
				nullptr, nullptr, nullptr,
				&SceneInventory::updateSelecting,
				nullptr
		};

        enum class ActionName
        {
                LEFT,
                RIGHT,
                UP,
                DOWN,
                SELECT,
                SAVE_WORD,
                SORT
        };

		void handleAddOrLook(ActionName act);
        void handleAddingWord(ActionName act);
		void handleLookUp(ActionName act);
		void handleSelecting(ActionName act);

		using actionStateFunc = void(SceneInventory::*)(ActionName act);
		static constexpr std::array<actionStateFunc, (size_t)InventoryState::COUNT > m_actionTable = {
				&SceneInventory::handleAddOrLook,
				&SceneInventory::handleAddingWord,
				&SceneInventory::handleLookUp,
				nullptr, nullptr, nullptr,
				&SceneInventory::handleSelecting
		};

        InventoryState m_state;

        int m_currentInitState;
        int m_currentWordClass;
        int m_currentWordInfo;
		int m_currentLookUp;
		int m_currentWordList;
		int m_currentPage;

        sf::Font m_font;
        sf::RectangleShape m_indicator;
        sf::Text m_nativeName;
        sf::Text m_imperialName;
        sf::Text m_wordClass;

		sf::Text m_helpMessage;
		sf::Text m_helpMessage2;

        std::vector<std::string> m_words;

        std::vector<LexiconManager::WordClass> m_wordClasses;
        std::vector<InventoryState> m_initStates;
        std::vector<InventoryState> m_wordAddingDetails;
		std::vector<InventoryState> m_lookUps;
		std::vector<sf::RectangleShape> m_rectangles;

        std::shared_ptr<Entity> m_infoBox;
        std::shared_ptr<Entity> m_listBox;

        void init();
        void makeIndicator();
        void makeInfoBox();
        void makeListBox();
        void makeInputTexts();

        void update() override;
        void updateIndicator(sf::Vector2f pos, sf::Vector2f size, sf::Color color);
        void updateInput();

        void sDoAction(const Action& action) override;

        void handleInput();

		void loadWords();
		void loadNouns();
		void loadVerbs();
		void loadAdj();

		void nextPage();
		void previousPage();

		std::vector<std::string> getWords();

        void onEnd() override;

        void printWords(const std::unordered_map<std::string, LexiconManager::Word>& words) const;

        void renderBoxes();
        void renderInputDetails();
		void renderWordList();

public:
        SceneInventory(GameEngine* gameEngine);
        void sRender() override;
};
