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
}

void SceneInventory::init()
{
  m_state = InventoryState::ADD_OR_LOOK;

  m_currentInitState = 0;
  m_currentWordClass = 0;
  m_currentWordInfo = 0;

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
  sf::Text addButton= createText("Add New Word", m_font, {222.f, 484.f}, TEXT_SIZE, TEXT_COLOR);

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

  if(m_state == InventoryState::LOOK_UP)
    {
      switch(m_currentWordClass)
        {
        case 0:
          renderWordList(m_lexiconManager.getNouns());
          break;
        case 1:
          renderWordList(m_lexiconManager.getAdjectives());
          break;
        case 2:
          renderWordList(m_lexiconManager.getVerbs());
          break;
        }
    }
}

void SceneInventory::update()
{
  m_entities.update();

  switch(m_state)
    {
    case InventoryState::ADD_OR_LOOK:
      switch(m_currentInitState)
        {
        case 0:
          updateIndicator(INFO_PANEL_POS, PANEL_SIZE, INDICATOR_COLOR);
          break;
        case 1:
          updateIndicator(LIST_PANEL_POS, PANEL_SIZE, INDICATOR_COLOR);
          break;
        }
      break;
    case InventoryState::ADDING_WORD:
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
      break;
      //    case InventoryState::WAITING_FOR_INPUT:
      //updateInput();
      // break;
    default:
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

  //m_state = InventoryState::ADDING_WORD;
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

  switch(m_state)
    {
    case InventoryState::ADD_OR_LOOK:
      handleAddOrLook(action, act);
      break;
    case InventoryState::ADDING_WORD:
      handleAddingWord(action, act);
      break;
    case InventoryState::LOOK_UP:
      break;
    default:
      break;
    }
}

void SceneInventory::handleAddingWord(const Action& action, ActionName act)
{
  auto& natName = m_nativeName->getComponent<CText>().text;
  auto& impName = m_imperialName->getComponent<CText>().text;
  auto& wordClass = m_wordClass->getComponent<CText>().text;

  switch(action.type())
    {
    case ActionType::START:
      switch(act)
        {
        case ActionName::DOWN:
          m_currentWordInfo = (m_currentWordInfo + 1) % m_wordAddingDetails.size();
          std::cout << m_currentWordInfo << std::endl;
          break;
        case ActionName::UP:
          m_currentWordInfo = (m_currentWordInfo - 1) % m_wordAddingDetails.size();
          std::cout << m_currentWordInfo << std::endl;
          break;
        case ActionName::SELECT:
          m_state = m_wordAddingDetails[m_currentWordInfo];
          switch(m_currentWordInfo)
            {
            case 0:
              std::cout << "Input native name." << std::endl;
              getTextInputManager().start();
              natName.setString(getTextInputManager().getText());
              std::cout << getTextInputManager().getText() << std::endl;
              break;
            case 1:
              std::cout << "Input imperial name." << std::endl;
              getTextInputManager().start();
              impName.setString(getTextInputManager().getText());
              std::cout << getTextInputManager().getText() << std::endl;
              break;
            case 2:
              std::cout << "Input word class." << std::endl;
              getTextInputManager().start();
              wordClass.setString(getTextInputManager().getText());
              std::cout << getTextInputManager().getText() << std::endl;
              break;
            }
        case ActionName::SAVE_WORD:
          if(!natName.getString().isEmpty() && !impName.getString().isEmpty() && !wordClass.getString().isEmpty())
            {
              getLexiconManager().m_addWord(natName.getString(), impName.getString(), wordClass.getString());
            }
          break;
        default:
          break;
        }
    default:
      break;
    }
}

void SceneInventory::handleAddOrLook(const Action& action, ActionName act)
{
  switch(action.type())
    {
    case ActionType::START:
      switch(act)
        {
        case ActionName::LEFT:
          m_currentInitState = (m_currentInitState - 1) % m_initStates.size();
          std::cout << m_currentInitState << std::endl;
          break;
        case ActionName::RIGHT:
          m_currentInitState = (m_currentInitState + 1) % m_initStates.size();
          std::cout << m_currentInitState << std::endl;
          break;
        case ActionName::SELECT:
          m_state = m_initStates[m_currentInitState];
          switch(m_currentInitState)
            {
            case 0:
              std::cout << "Info state." << std::endl; break;
            case 1:
              std::cout << "List state." << std::endl; break;
            }
        default:
          break;
        }
    default:
      break;
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
  m_game->window().draw(m_indicator);
}

void SceneInventory::renderInputDetails()
{
  m_game->window().draw(m_nativeName);
  m_game->window().draw(m_imperialName);
  m_game->window().draw(m_wordClass);
}

void SceneInventory::renderWordList(const std::unordered_map<std::string, LexiconManager::Word>& words)
{
  for(const auto& [key, value] : words)
    {
      m_words.push_back(key);
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
