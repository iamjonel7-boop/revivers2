#include "SceneInventory.h"
#include "GameEngine.h"
#include <iostream>

const float SceneInventory::TEXT_SIZE = 10;
const float SceneInventory::INPUT_SIZE = 9;
const sf::Vector2f SceneInventory::PANEL_SIZE = {44*7.f, 44*10.f};
const sf::Vector2f SceneInventory::INPUT_FIELD_SIZE = {220.f, 30.f};

const sf::Vector2f SceneInventory::INFO_PANEL_POS = {44.f, 44*2.f};
const sf::Vector2f SceneInventory::LIST_PANEL_POS = {44*10.f, 44*2.f};
const sf::Vector2f SceneInventory::NAT_NAME_FIELD_POS = {88.f, 146.f};
const sf::Vector2f SceneInventory::IMP_NAME_FIELD_POS = {88.f, 234.f};
const sf::Vector2f SceneInventory::WORD_CLASS_FIELD_POS = {88.f, 322.f};
const sf::Vector2f SceneInventory::NAT_NAME_TEXT_POS = {89.f, 119.f};
const sf::Vector2f SceneInventory::IMP_NAME_TEXT_POS = {89.f, 207.f};
const sf::Vector2f SceneInventory::CLASS_TEXT_POS = {89.f, 294.f};
const sf::Vector2f SceneInventory::NOUN_TEXT_POS = {484.f, 112.f};
const sf::Vector2f SceneInventory::ADJ_TEXT_POS = {570.f, 112.f};
const sf::Vector2f SceneInventory::VERB_TEXT_POS = {678.f, 112.f};
const sf::Vector2f SceneInventory::NAT_NAME_INPUT_POS = {100.f, 159.f};
const sf::Vector2f SceneInventory::IMP_NAME_INPUT_POS = {100.f, 247.f};
const sf::Vector2f SceneInventory::WORD_CLASS_INPUT_POS = {100.f, 334.f};

const sf::Color SceneInventory::INPUT_FIELD_COLOR = sf::Color::Cyan;
const sf::Color SceneInventory::PANEL_COLOR = sf::Color::White;
const sf::Color SceneInventory::TEXT_COLOR = sf::Color::Black;
const sf::Color SceneInventory::INDICATOR_COLOR = sf::Color::Red;

SceneInventory::SceneInventory(GameEngine* gameEngine) :
		Scene(gameEngine)
{
		init();

		registerAction(sf::Keyboard::Left, static_cast <int>(ActionName::LEFT));
		registerAction(sf::Keyboard::Right, static_cast<int>(ActionName::RIGHT));
		registerAction(sf::Keyboard::Up, static_cast<int>(ActionName::UP));
		registerAction(sf::Keyboard::Down, static_cast<int>(ActionName::DOWN));
		registerAction(sf::Keyboard::Tab, static_cast<int>(ActionName::SELECT));
		registerAction(sf::Keyboard::S, static_cast<int>(ActionName::SORT));
}

void SceneInventory::init()
{
		m_state = InventoryState::ADD_OR_LOOK;

		m_currentInitState = 0;
		m_currentWordClass = 0;
		m_currentWordInfo = 0;
		m_currentWordList = 0;
		m_currentPage = 0;

		m_initStates = {
				InventoryState::ADDING_WORD,
				InventoryState::LOOK_UP
		};

		m_wordClasses = {
				LexiconManager::WordClass::NOUN,
				LexiconManager::WordClass::ADJECTIVE,
				LexiconManager::WordClass::VERB
		};

		m_wordAddingDetails = {
				InventoryState::ENTERING_NATIVE_WORD,
				InventoryState::ENTERING_IMPERIAL_WORD,
				InventoryState::ENTERING_WORD_CLASS
		};

		m_helpMessage = createText("", m_font, {47.f, 18.f}, 10, sf::Color::White);
		m_helpMessage2 = createText("", m_font, {47.f, 38.f}, 10, sf::Color::White);

		m_font.loadFromFile("/usr/local/share/fonts/dejavu/DejaVuSans.ttf");
		makeIndicator();
		makeInfoBox();
		makeListBox();
		makeInputTexts();
}

void SceneInventory::makeInputTexts()
{
		m_nativeName = createText("Input here", m_font, NAT_NAME_INPUT_POS, INPUT_SIZE, TEXT_COLOR);
		m_imperialName = createText("Input here", m_font, IMP_NAME_INPUT_POS, INPUT_SIZE, TEXT_COLOR);
		m_wordClass = createText("Input here", m_font, WORD_CLASS_INPUT_POS, INPUT_SIZE, TEXT_COLOR);
}

void SceneInventory::makeIndicator()
{
		m_indicator.setSize(PANEL_SIZE);
		m_indicator.setFillColor(sf::Color::Transparent);
		m_indicator.setOutlineColor(sf::Color::Red);
		m_indicator.setOutlineThickness(2.f);
		m_indicator.setPosition(INFO_PANEL_POS);
}

void SceneInventory::makeInfoBox()
{
		m_infoBox = m_entities.addEntity("box");
		m_infoBox->addComponent<CBoxField>();

		auto& rectangles = m_infoBox->getComponent<CBoxField>().rectangleShapes;
		auto& texts = m_infoBox->getComponent<CBoxField>().texts;

		std::vector<sf::RectangleShape> rects = {
				createBox(INFO_PANEL_POS, PANEL_SIZE, PANEL_COLOR),
				createBox(NAT_NAME_FIELD_POS, INPUT_FIELD_SIZE, INPUT_FIELD_COLOR),
				createBox(IMP_NAME_FIELD_POS, INPUT_FIELD_SIZE, INPUT_FIELD_COLOR),
				createBox(WORD_CLASS_FIELD_POS, INPUT_FIELD_SIZE, INPUT_FIELD_COLOR)
		};

		std::vector<sf::Text> txts = {
				createText("Native Word", m_font, NAT_NAME_TEXT_POS, TEXT_SIZE, TEXT_COLOR),
				createText("Imperial Word", m_font, IMP_NAME_TEXT_POS, TEXT_SIZE, TEXT_COLOR),
				createText("Word Class", m_font, CLASS_TEXT_POS, TEXT_SIZE, TEXT_COLOR),
		};

		sf::Text saveButton = createText("Save", m_font, {222.f, 484.f}, TEXT_SIZE, TEXT_COLOR);
		sf::Text addButton= createText("Add Word", m_font, {222.f, 484.f}, TEXT_SIZE, TEXT_COLOR);

		for(const auto& r : rects)
				rectangles.push_back(r);

		for(const auto& t : txts)
				texts.push_back(t);

		switch(m_state)
		{
		case InventoryState::ADDING_WORD:
				texts.push_back(saveButton);
				break;
		case InventoryState::LOOK_UP:
				texts.push_back(addButton);
				break;
		default:
				break;
		}
}

void SceneInventory::makeListBox()
{
		m_listBox = m_entities.addEntity("box");
		m_listBox->addComponent<CBoxField>();

		auto& rectangles = m_listBox->getComponent<CBoxField>().rectangleShapes;
		auto& texts = m_listBox->getComponent<CBoxField>().texts;

		std::vector<sf::RectangleShape> rects = {
				createBox(LIST_PANEL_POS, PANEL_SIZE, PANEL_COLOR)
		};

		std::vector<sf::Text> txts = {
				createText("Nouns", m_font, NOUN_TEXT_POS, TEXT_SIZE, TEXT_COLOR),
				createText("Adjectives", m_font, ADJ_TEXT_POS, TEXT_SIZE, TEXT_COLOR),
				createText("Verbs", m_font, VERB_TEXT_POS, TEXT_SIZE, TEXT_COLOR)
		};

		for(const auto& r : rects)
				rectangles.push_back(r);

		for(const auto& t : txts)
				texts.push_back(t);
}

void SceneInventory::update()
{
		m_entities.update();

		std::string line1, line2;

		const auto& func = m_updateTable[static_cast<size_t>(m_state)];
		if(func != nullptr) (this->*func)(line1, line2);

		m_helpMessage.setString(line1);
		m_helpMessage2.setString(line2);
}

void SceneInventory::updateAddOrLook(std::string& help1, std::string& help2)
{
		help1 = "Use LEFT/RIGHT to navigate.";
		help2 = "Press TAB to select, LEFT/RIGHT to return.";
		switch(m_currentInitState)
		{
		case 0:
				updateIndicator(INFO_PANEL_POS, PANEL_SIZE, INDICATOR_COLOR);
				break;
		case 1:
				updateIndicator(LIST_PANEL_POS, PANEL_SIZE, INDICATOR_COLOR);
				break;
		}
}

void SceneInventory::updateAddingWord(std::string& help1, std::string& help2)
{
		help1 = "Use UP/DOWN to navigate.";
		help2 = "Enter to save input.";
		switch(m_currentWordInfo)
		{
		case 0:
				updateIndicator(NAT_NAME_FIELD_POS, INPUT_FIELD_SIZE, INDICATOR_COLOR);
				updateInput();
				break;
		case 1:
				updateIndicator(IMP_NAME_FIELD_POS, INPUT_FIELD_SIZE, INDICATOR_COLOR);
				updateInput();
				break;
		case 2:
				updateIndicator(WORD_CLASS_FIELD_POS, INPUT_FIELD_SIZE, INDICATOR_COLOR);
				updateInput();
				break;
		}
}

void SceneInventory::updateLookUp(std::string& help1, std::string& help2)
{
		help1 = "Select Word Class: Use LEFT/RIGHT to navigate";
		help2 = "Press TAB to view words, s to return";
		switch(m_currentWordClass)
		{
		case 0:
				updateIndicator({464.f, 102.f}, {70.f, 38.f}, INDICATOR_COLOR);
				break;
		case 1:
				updateIndicator({560.f, 102.f}, {70.f, 38.f}, INDICATOR_COLOR);
				break;
		case 2:
				updateIndicator({658.f, 102.f}, {70.f, 38.f}, INDICATOR_COLOR);
				break;
		}
}

void SceneInventory::updateSelecting(std::string& help1, std::string& help2)
{
		help1 = "Use UP/DOWN to navigate, TAB to view word information.";
		help2 = "Use RIGHT for next page and LEFT for previous page.";
		switch(m_currentWordList)
		{
		case 0:
				updateIndicator({498.f, 189.f}, {194.f, 31.f}, INDICATOR_COLOR);
				break;
		case 1:
				updateIndicator({498.f, 233.f}, {194.f, 31.f}, INDICATOR_COLOR);
				break;
		case 2:
				updateIndicator({498.f, 278.f}, {194.f, 31.f}, INDICATOR_COLOR);
				break;
		case 3:
				updateIndicator({498.f, 321.f}, {194.f, 31.f}, INDICATOR_COLOR);
				break;
		case 4:
				updateIndicator({498.f, 365.f}, {194.f, 31.f}, INDICATOR_COLOR);
				break;
		case 5:
				updateIndicator({498.f, 410.f}, {194.f, 31.f}, INDICATOR_COLOR);
				break;
		}
}

void SceneInventory::updateInput()
{
		std::string inputText = getTextInputManager().getText();

		switch(m_currentWordInfo) {
		case 0:
				m_nativeName.setString(inputText);
				break;
		case 1:
				m_imperialName.setString(inputText);
				break;
		case 2:
				m_wordClass.setString(inputText);
				break;
		}
}

void SceneInventory::updateIndicator(sf::Vector2f pos, sf::Vector2f size, sf::Color color)
{
		m_indicator.setPosition(pos);
		m_indicator.setSize(size);
		m_indicator.setOutlineColor(color);
}

void SceneInventory::sDoAction(const Action& action)
{
		ActionName act = static_cast<ActionName>(action.name());

		switch(action.type())
		{
		case ActionType::START:
		{
				const auto& func = m_actionTable[static_cast<size_t>(m_state)];
				if(func != nullptr) (this->*func)(act);
				break;
		}
		default:
				break;
		}
}

void SceneInventory::handleLookUp(ActionName act)
{
		std::cout << "handlelookup" << std::endl;
		switch(act)
		{
		case ActionName::RIGHT:
				m_currentWordClass = wrapIndex(m_currentWordClass, 1, m_wordClasses.size());
				break;
		case ActionName::LEFT:
				m_currentWordClass = wrapIndex(m_currentWordClass, -1, m_wordClasses.size());
				break;
		case ActionName::UP:
				m_state = InventoryState::ADD_OR_LOOK;
				break;
		case ActionName::SELECT:
				loadWords();
				if (m_words.empty())
				{
						std::cout << "No words in this category" << std::endl;
						return;
				}
				m_state = InventoryState::SELECTING;
				break;
		default:
				break;
		}
}

void SceneInventory::loadWords()
{
		switch(m_currentWordClass)
		{
		case 0:
				loadNouns(); break;
		case 1:
				loadAdj(); break;
		case 2:
				loadVerbs(); break;
		}
}

void SceneInventory::handleSelecting(ActionName act)
{
		std::cout << "handlelistnav" << std::endl;
		std::vector<std::string> currPageWords = getWords();

		if (currPageWords.empty())
		{
				m_state = InventoryState::LOOK_UP;
				//clearWordInfo();
				return;
		}

		switch(act)
		{
		case ActionName::DOWN:
				m_currentWordList = wrapIndex(m_currentWordList, 1, std::min(6, static_cast<int>(currPageWords.size())));
				break;
		case ActionName::UP:
				m_currentWordList = wrapIndex(m_currentWordList, -1, std::min(6, static_cast<int>(currPageWords.size())));
				break;
		case ActionName::SORT:
				m_state = InventoryState::LOOK_UP;
				break;
		case ActionName::RIGHT:
				nextPage();
				break;
		case ActionName::LEFT:
				previousPage();
				break;
		default:
				break;
		}
}

void SceneInventory::handleAddingWord(ActionName act)
{
		switch(act)
		{
		case ActionName::DOWN:
				m_currentWordInfo = wrapIndex(m_currentWordInfo, 1, m_wordAddingDetails.size());
				handleInput();
				break;
		case ActionName::UP:
				m_currentWordInfo = wrapIndex(m_currentWordInfo, -1, m_wordAddingDetails.size());
				handleInput();
				break;
				//case ActionName::SELECT:
				//m_state = m_wordAddingDetails[m_currentWordInfo];
				//handleInput(act);
		case ActionName::LEFT:
		case ActionName::RIGHT:
				m_state = InventoryState::ADD_OR_LOOK;
				break;
				//  case ActionName::SAVE_WORD:
				//if(!m_nativeName.getString().isEmpty() && !m_imperialName.getString().isEmpty() && !m_wordClass.getString().isEmpty())
				//{
				//  getLexiconManager().m_addWord(m_nativeName.getString(), m_imperialName.getString(), m_wordClass.getString());
				//}
				// break;
		default:
				break;
		}
}

void SceneInventory::handleInput()
{
		getTextInputManager().start();
}

void SceneInventory::handleAddOrLook(ActionName act)
{
		std::cout << "handleaddorlook" << std::endl;

		switch(act)
		{
		case ActionName::LEFT:
				m_currentInitState = wrapIndex(m_currentInitState, -1, m_initStates.size());
				break;
		case ActionName::RIGHT:
				m_currentInitState = wrapIndex(m_currentInitState, 1, m_initStates.size());
				break;
		case ActionName::SELECT:
				m_state = m_initStates[m_currentInitState];
				if(m_state == InventoryState::ADDING_WORD)
						handleInput();
		default:
				break;
		}
}

void SceneInventory::loadNouns()
{
		m_words.clear();
		m_words = getLexiconManager().getNouns();
}

void SceneInventory::loadAdj()
{
		m_words.clear();
		m_words = getLexiconManager().getAdjectives();
}

void SceneInventory::loadVerbs()
{
		m_words.clear();
		m_words = getLexiconManager().getVerbs();
}

std::vector<std::string> SceneInventory::getWords()
{
		int startIndex = m_currentPage*6;
		int endIndex = std::min(startIndex+6, static_cast<int>(m_words.size()));

		if (startIndex >= static_cast<int>(m_words.size()))
				return {};

        return std::vector<std::string>(
				m_words.begin() + startIndex,
				m_words.begin() + endIndex
				);
}

void SceneInventory::nextPage()
{
        int maxPage = (m_words.size() - 1) /6;
        if (m_currentPage < maxPage) {
				m_currentPage++;
        }
}

void SceneInventory::previousPage()
{
        if (m_currentPage > 0) {
				m_currentPage--;
        }
}

void SceneInventory::onEnd()
{
		m_game->quit();
}

void SceneInventory::sRender()
{
		m_game->window().setView(m_game->window().getDefaultView());
		renderBoxes();
		renderInputDetails();
		renderWordList();
		m_game->window().draw(m_helpMessage);
		m_game->window().draw(m_helpMessage2);
		m_game->window().draw(m_indicator);
}

void SceneInventory::renderInputDetails()
{
		m_game->window().draw(m_nativeName);
		m_game->window().draw(m_imperialName);
		m_game->window().draw(m_wordClass);
}

void SceneInventory::renderWordList()
{
		std::vector<std::string> words = getWords();

		if (words.empty())
		{
				sf::Text emptyText = createText("No words available",
												m_font,
												{503.f, 300.f},
												TEXT_SIZE,
												TEXT_COLOR);
				m_game->window().draw(emptyText);
				return;
		}

		sf::Vector2f startPos{515.f, 197.f};
		float lineHeight = 240.f-197.f;

		for (size_t i = 0; i < words.size(); i++)
		{
				sf::Vector2f wordPos = {startPos.x, startPos.y + (i * lineHeight)};
				sf::Text wordText = createText(words[i], m_font, wordPos,
											   TEXT_SIZE, TEXT_COLOR);
				m_game->window().draw(wordText);
		}
}

void SceneInventory::renderBoxes()
{
		for(auto& e : m_entities.getEntities("box"))
		{
				if(e->hasComponent<CBoxField>())
				{
						auto& rectangles = e->getComponent<CBoxField>().rectangleShapes;
						for(auto& r : rectangles)
								m_game->window().draw(r);

						auto& texts = e->getComponent<CBoxField>().texts;
						for(auto& t : texts)
								m_game->window().draw(t);
				}
		}
}


void SceneInventory::printWords(const std::unordered_map<std::string, LexiconManager::Word>& words) const
{
		for(const auto& [key, value] : words)
		{
				std::cout << key << ", ";
		}
		std::cout << std::endl;
}
