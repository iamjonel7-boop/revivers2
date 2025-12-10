#pragma once

#include <string>
#include <SFML/Graphics.hpp>
#include <memory>
#include <random>

class Entity;

enum class Language
{
		IMPERIAL,
		NATIVE
};

enum class Ethnicity
{
		NATIVE_CIV,
		IMPERIAL_CIV
};

enum class Building
{
		HOME,
		PLAZA,
		SCHOOL,
		WORKPLACE,
		GOVERNMENT,
		COUNT
};

struct Component
{
        bool has = false;
};

struct CPopulation : Component
{
		Ethnicity ethnicity;
		Language speakingLanguage;
		int age = 0;
		int maxAge;
		bool willDieNextCycle = false;
		CPopulation(Ethnicity eth, Language lan)
				: ethnicity(eth), speakingLanguage(lan)
				{
						std::random_device rd;
						std::mt19937 gen(rd());
						std::uniform_int_distribution<> ageDist(15,70); //lifespan between 15 to 70
						maxAge = ageDist(gen);
				}
};

struct CBoxField : Component
{
        std::vector<sf::RectangleShape> rectangleShapes;
        std::vector<sf::Text> texts;
        CBoxField()
        {
        }
};

struct CRectangleShape : Component
{
        sf::RectangleShape rectangle;
        CRectangleShape()
        {
        }
};

struct CPath : Component
{
        std::vector<std::shared_ptr<Entity>> entities;
        CPath()
        {
        }
};

struct CBuilding : Component
{
        std::string nativeName;
        std::string imperialName;
        bool isClaimed = false;
		Building type;
        CBuilding()
        {
        }
};

struct CProfile : Component
{
        std::string playerName;
        CProfile()
        {
        }
};

struct CBoundingBox : Component
{
        sf::Vector2i size;
        sf::Vector2i halfSize;
        bool blockMove = false;
        bool blockVision = false;
        CBoundingBox()
        {
        }
        CBoundingBox(const sf::Vector2i& s)
                : size(s), halfSize(s.x / 2, s.y / 2)
        {
        }
        CBoundingBox(const sf::Vector2i& s, bool m, bool v)
                : size(s)
                , halfSize(s.x / 2, s.y / 2)
                , blockMove(m)
                , blockVision(v)
        {
        }
};

struct CTransform: Component
{
        sf::Vector2f position{0.f,0.f};
        sf::Vector2f scale{1.f, 1.f};
        sf::Vector2f velocity{0.f,0.f};
        float rotation{0.f};
        CTransform()
        {
        }
        CTransform(const sf::Vector2f& p, const sf::Vector2f& s, const sf::Vector2f& v, float r)
                : position(p)
                , scale(s)
                , velocity(v)
                , rotation(r)
        {
        }
};

struct CInput:  Component
{
        bool up = false;
        bool down = false;
        bool right = false;
        bool left = false;
        CInput()
        {
        }
};

struct CShape: Component
{
        bool isVisible = true;
        std::shared_ptr<sf::Shape> shape;
        CShape(std::shared_ptr<sf::Shape> s)
                : shape(std::move(s))
        {
        }
};

struct CText: Component
{
        bool isVisible = true;
        sf::Text text;
        CText(const sf::Font& font, const std::string& str, unsigned int size, sf::Vector2f pos, sf::Color color)
        {
                text.setFont(font);
                text.setString(str);
                text.setCharacterSize(size);
                text.setPosition(pos);
                text.setFillColor(color);
        }
        CText()
        {
        }
};

struct CLifeSpan:  Component
{
        int lifespan = 0;
        int frameCreated = 0;
        CLifeSpan()
        {
        }
        CLifeSpan(int duration, int frame)
                : lifespan(duration)
                , frameCreated(frame)
        {
        }
};
