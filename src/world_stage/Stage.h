#pragma once

#include <string>
#include <map>
#include <functional>
#include <vector>

class Stage
{
 protected:
  std::string name;
  std::map<std::string, std::function<bool()>> achievements;
  std::map<std::string, std::function<bool()>> requisites;

 public:
  Stage();
  virtual ~Stage() = default;
  virtual std::vector<std::string> getPlayerAchievements()
  {

  };
  virtual bool isComplete()
  {

  };
};
