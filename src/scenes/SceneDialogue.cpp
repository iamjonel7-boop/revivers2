#include "SceneDialogue.h"
#include "GameEngine.h"
#include "Components.h"
#include "SceneMain.h"
#include <iostream>
#include <algorithm>
#include <cctype>

SceneDialogue::SceneDialogue(GameEngine* gameEngine,
                             std::shared_ptr<Entity> targetCivilian,
                             const std::string& imperialWord,
                             const std::string& correctTranslation,
                             int nearbyNativeCivs,
                             int nearbyImperialCivs)
    : Scene(gameEngine)
    , m_targetCivilian(targetCivilian)
    , m_imperialWord(imperialWord)
    , m_correctTranslation(correctTranslation)
    , m_nearbyNativeCivs(nearbyNativeCivs)
    , m_nearbyImperialCivs(nearbyImperialCivs)
    , m_translationCorrect(false)
{
    init();

    registerAction(sf::Keyboard::Enter, static_cast<int>(ActionName::ENTER));
    registerAction(sf::Keyboard::Escape, static_cast<int>(ActionName::RETURN));
    registerAction(sf::Keyboard::Space, static_cast<int>(ActionName::SUBMIT));
}

void SceneDialogue::init()
{
    m_state = DialogueState::SHOWING_PROMPT;

    m_font.loadFromFile("/usr/local/share/fonts/Liberation/LiberationMono-Regular.ttf");

    // Calculate success chance based on nearby civilians
    calculateSuccessChance();

    // Setup UI elements
    m_dialogueBox.setSize({600.f, 400.f});
    m_dialogueBox.setPosition({100.f, 100.f});
    m_dialogueBox.setFillColor(sf::Color(40, 40, 40));
    m_dialogueBox.setOutlineThickness(2.f);
    m_dialogueBox.setOutlineColor(sf::Color::White);

    m_inputBox.setSize({500.f, 40.f});
    m_inputBox.setPosition({150.f, 350.f});
    m_inputBox.setFillColor(sf::Color(60, 60, 60));
    m_inputBox.setOutlineThickness(1.f);
    m_inputBox.setOutlineColor(sf::Color::Cyan);

    m_promptText = createText(
        "An Imperial speaker approaches you.\nThey want to learn the native language!",
        m_font, {120.f, 120.f}, 14, sf::Color::White);

    m_wordToTranslate = createText(
        "Imperial word: \"" + m_imperialWord + "\"",
        m_font, {120.f, 200.f}, 16, sf::Color::Yellow);

    m_inputLabel = createText(
        "Enter the native translation:",
        m_font, {120.f, 320.f}, 12, sf::Color::White);

    m_userInput = createText("", m_font, {160.f, 360.f}, 12, sf::Color::Cyan);

    std::string chanceText = "Success chance: " + std::to_string((int)(m_successChance * 100)) + "%";
    if (m_nearbyNativeCivs > 0)
        chanceText += "\n(+" + std::to_string(m_nearbyNativeCivs) + " native speakers nearby help!)";

    m_helpText = createText(chanceText, m_font, {120.f, 250.f}, 12, sf::Color::Green);
}

void SceneDialogue::calculateSuccessChance()
{
    // Base chance: 50%
    float baseChance = 0.5f;

    // Each nearby native speaker adds 10% (up to 40%)
    float nativeBonus = std::min(0.4f, m_nearbyNativeCivs * 0.1f);

    // Each nearby imperial speaker reduces by 5% (up to -20%)
    float imperialPenalty = std::min(0.2f, m_nearbyImperialCivs * 0.05f);

    m_successChance = std::max(0.1f, std::min(0.95f, baseChance + nativeBonus - imperialPenalty));

    std::cout << "Success chance calculated: " << (m_successChance * 100) << "%" << std::endl;
    std::cout << "Native bonus: +" << (nativeBonus * 100) << "%" << std::endl;
    std::cout << "Imperial penalty: -" << (imperialPenalty * 100) << "%" << std::endl;
}

void SceneDialogue::checkTranslation()
{
    // Normalize strings for comparison (lowercase, trim whitespace)
    auto normalize = [](std::string s) {
        s.erase(0, s.find_first_not_of(" \t\n\r"));
        s.erase(s.find_last_not_of(" \t\n\r") + 1);
        std::transform(s.begin(), s.end(), s.begin(), ::tolower);
        return s;
    };

    std::string normalizedInput = normalize(m_playerTranslation);
    std::string normalizedCorrect = normalize(m_correctTranslation);

    m_translationCorrect = (normalizedInput == normalizedCorrect);

    std::cout << "Translation check:" << std::endl;
    std::cout << "  Player: '" << normalizedInput << "'" << std::endl;
    std::cout << "  Correct: '" << normalizedCorrect << "'" << std::endl;
    std::cout << "  Result: " << (m_translationCorrect ? "CORRECT" : "WRONG") << std::endl;
}

void SceneDialogue::applyConversionResult()
{
    if (!m_targetCivilian || !m_targetCivilian->hasComponent<CPopulation>())
        return;

    auto& pop = m_targetCivilian->getComponent<CPopulation>();

    if (m_translationCorrect)
    {
        // Correct translation - roll for success based on chance
        float roll = static_cast<float>(rand()) / RAND_MAX;

        if (roll <= m_successChance)
        {
            // SUCCESS! Convert to native speaker
            pop.speakingLanguage = Language::NATIVE;
            WorldManager::nativeSpeakers++;
            WorldManager::imperialSpeakers--;

            m_resultText.setString("SUCCESS!\n\nThe civilian now speaks the native language!\n\n"
                                 "Native speakers: " + std::to_string(WorldManager::nativeSpeakers) +
                                 "\nImperial speakers: " + std::to_string(WorldManager::imperialSpeakers));
            m_resultText.setFillColor(sf::Color::Green);

            std::cout << "Conversion successful! Roll: " << roll << " <= " << m_successChance << std::endl;
        }
        else
        {
            // Correct translation but failed roll
            m_resultText.setString("The translation was correct, but the civilian\nneeds more convincing...\n\n"
                                 "Try again when there are more native speakers nearby.");
            m_resultText.setFillColor(sf::Color::Yellow);

            std::cout << "Conversion failed despite correct answer. Roll: " << roll << " > " << m_successChance << std::endl;
        }
    }
    else
    {
        // Wrong translation
        m_resultText.setString("INCORRECT!\n\nThe correct translation was: \"" + m_correctTranslation + "\"\n\n"
                             "The civilian remains unconvinced.\nStudy the language more and try again!");
        m_resultText.setFillColor(sf::Color::Red);
    }

    m_resultText.setPosition({120.f, 200.f});
    m_resultText.setCharacterSize(14);
}

void SceneDialogue::updateShowingPrompt(std::string& help)
{
    help = "Press ENTER to begin translation\nPress ESC to cancel";
}

void SceneDialogue::updateEnteringTranslation(std::string& help)
{
    help = "Type the native translation\nPress SPACE to submit | ESC to cancel";
    m_playerTranslation = getTextInputManager().getText();
    m_userInput.setString(m_playerTranslation);
}

void SceneDialogue::updateShowingResult(std::string& help)
{
    help = "Press ENTER to return to map";
}

void SceneDialogue::handleShowingPrompt(const Action& action, ActionName act)
{
    if (action.type() != ActionType::START) return;

    switch(act)
    {
    case ActionName::ENTER:
        m_state = DialogueState::ENTERING_TRANSLATION;
        getTextInputManager().start();
        break;
    case ActionName::RETURN:
        m_game->changeScene("main", std::make_shared<SceneMain>(m_game));
        break;
    default:
        break;
    }
}

void SceneDialogue::handleEnteringTranslation(const Action& action, ActionName act)
{
    if (action.type() != ActionType::START) return;

    switch(act)
    {
    case ActionName::SUBMIT:
        if (!m_playerTranslation.empty())
        {
            getTextInputManager().stop();
            checkTranslation();
            applyConversionResult();
            m_state = DialogueState::SHOWING_RESULT;
        }
        break;
    case ActionName::RETURN:
        getTextInputManager().stop();
        m_game->changeScene("main", std::make_shared<SceneMain>(m_game));
        break;
    default:
        break;
    }
}

void SceneDialogue::handleShowingResult(const Action& action, ActionName act)
{
    if (action.type() != ActionType::START) return;

    switch(act)
    {
    case ActionName::ENTER:
    case ActionName::RETURN:
        m_game->changeScene("main", std::make_shared<SceneMain>(m_game));
        break;
    default:
        break;
    }
}

void SceneDialogue::update()
{
    m_entities.update();

    std::string help;
    const auto& func = m_updateTable[static_cast<size_t>(m_state)];
    if(func != nullptr) (this->*func)(help);
}

void SceneDialogue::sDoAction(const Action& action)
{
    ActionName act = static_cast<ActionName>(action.name());

    const auto& func = m_actionTable[static_cast<size_t>(m_state)];
    if(func != nullptr) (this->*func)(action, act);
}

void SceneDialogue::onEnd()
{
    // Return to main scene
}

void SceneDialogue::sRender()
{
    m_game->window().setView(m_game->window().getDefaultView());

    m_game->window().draw(m_dialogueBox);

    switch(m_state)
    {
    case DialogueState::SHOWING_PROMPT:
        m_game->window().draw(m_promptText);
        m_game->window().draw(m_wordToTranslate);
        m_game->window().draw(m_helpText);
        break;

    case DialogueState::ENTERING_TRANSLATION:
        m_game->window().draw(m_wordToTranslate);
        m_game->window().draw(m_inputLabel);
        m_game->window().draw(m_inputBox);
        m_game->window().draw(m_userInput);
        m_game->window().draw(m_helpText);
        break;

    case DialogueState::SHOWING_RESULT:
        m_game->window().draw(m_resultText);
        break;
    }
}
