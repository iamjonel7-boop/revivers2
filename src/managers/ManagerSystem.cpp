#include "ManagerSystem.h"

void SystemManager::registerSystem(const std::string& systemName, std::function<void(EntityManager&, const std::map<std::string, std::shared_ptr<Entity>>&, const std::string& aspect)> system)
{
t
