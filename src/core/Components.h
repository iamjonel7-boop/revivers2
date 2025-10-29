#pragma once

#include <string>
#include <SFML/Graphics.hpp>
#include "Vec2.h"
#include <memory>

struct Component
{
   bool has = false;
};

enum class Gift
{
   Kind, //npcs more willing to cooperate but weak damage
   Clever, //unlocks hints, faster solutions, subtle advantages
   Cautious, //reduces risk in challenger, avoids negative outcomes
   Courageous //more damage to enemy but more encounters
};

struct CGift : Component
{
   Gift gift; //addComponent<CGift>({Gift::Clever})
};

struct CBoundingBox : Component
{
   Vec2 size;
   Vec2 halfSize;
   bool blockMove = false;
   bool blockVision = false;
   CBoundingBox()
   {
   }
   CBoundingBox(const Vec2& s)
      : size(s), halfSize(s.x / 2, s.y / 2)
   {
   }
   CBoundingBox(const Vec2& s, bool m, bool v)
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

struct CRender : Component
{
   std::shared_ptr<sf::Drawable> drawable;
   int layer; //lower : drawn firts, higher : ddrawn top
   CRender(std::shared_ptr<sf::Drawable> drawable, int layer = 0)
      : drawable(std::move(drawable)), layer(layer)
   {
   }
};

struct CShape: Component
{
   std::shared_ptr<sf::Shape> shape;
   CShape(std::shared_ptr<sf::Shape> s)
      : shape(std::move(s))
   {
   }
};

struct CText: Component
{
   sf::Text text;
   CText(const sf::Font& font, const std::string& str, unsigned size, sf::Vector2f pos, sf::Color color)
   {
      text.setFont(font);
      text.setString(str);
      text.setCharacterSize(size);
      text.setPosition(pos);
      text.setFillColor(color);
   }
};

struct CPlayer : Component
{
   int health = 100;
   int mana = 100;
   int energy = 100;
   int maxHealth = 100;
   int maxMana = 100;
   int maxEnergy = 100;
   CPlayer()
   CPlayer(int h, int m, int e, int mh = 100, int mm = 100, int me = 100)
      : health(h), mana(m), energy(e), maxHealth(mh), maxMana(mm), maxEnergy(me)
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

struct CSkills : Component
{
   int insight = 0;
   int fluency = 0;
   int literacy = 0;
   int prestige = 0;
   CSkills()
   {
   }
   CSkills(int ins, int flu, int lit, int pres)
      : insight(ins), fluency(flu), literacy(lit), prestige(pres)
   {
   }
};

struct CPopulation : Component
{
   int civilians = 100;
   int recruited = 0;
   CPopulation()
   {
   }
   CPopulation(int civ = 100, int rec = 0)
      : civilians(civ), recruited(rec)
   {
   }
};


struct CVerb:  Component
{
   std::string aspect = "default";
   std::string voice = "default";
   int cost = 10;
   CVerb()
   {
   }
   CVerb(std::string& aspect, std::string& voice, int cost = 10)
      : aspect(aspect), voice(voice), cost(cost)
   {
   }
};

struct CNoun:  Component
{
   std::string marker = "none";
   int cost = 5;
   CNoun()
   {
   }
   CNoun(std::string& marker, int cost = 5)
      : marker(marker), cost(cost)
   {
   }
};

struct CState:  Component
{
   std::string state = "idle";
   CState()
   {
   }
   CState(const std::string& state)
      : state(state)
   {
   }
};

struct CAffinity:  Component
{
   std::string affinity = "Imperium";
   CAffinity()
   {
   }
   CAffinity(std::string affinity)
      : affinity(affinity)
   {
   }
};
