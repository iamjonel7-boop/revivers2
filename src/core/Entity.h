#pragma once

#include "Components.h"
#include <memory>
#include <string>
#include <optional>

class EntityManager;

typedef std::tuple<
   std::optional<CTransform>,
   std::optional<CLifeSpan>,
   std::optional<CInput>,
   std::optional<CNoun>,
   std::optional<CVerb>,
   std::optional<CRender>,
   std::optional<CGift>,
   std::optional<CText>
   > ComponentTuple;

class Entity{
   friend class EntityManager;

   bool m_active = true;
   size_t m_id = 0;
   std::string m_tag = "default";
   ComponentTuple m_components;

   // constructor and destructor
   Entity(const size_t id, const std::string& tag):
      m_id(id), m_tag(tag){}

public:
   bool isDead() const;
   const std::string& tag() const;
   const std::string& role() const;
   size_t id() const;
  
   void destroy();

   template <typename T>
   bool hasComponent() const{
      return std::get<std::optional<T>>(m_components).has_value();
   }
 
   template <typename T, typename... TArgs>
   T& addComponent(TArgs&&... mArgs){
      auto& opt = std::get<std::optional<T>>(m_components);
      opt.emplace(std::forward<TArgs>(mArgs)...);
      return *opt;
      /* auto& component = etComponent<T>();
      component = T(std::forward<TArgs>(mArgs)...);
      component.has = true;
      return component;*/
   }

   template <typename T>
   T& getComponent(){
      return *std::get<std::optional<T>>(m_components);
      //return std::get<std::optional<T>>(m_components);
   }

   template <typename T>
   const T& getComponent() const{
      return *std::get<std::optional<T>>(m_components);
      //return std::get<T>(m_components);
   }

   template <typename T>
   void removeComponent(){
      std::get<std::optional<T>>(m_components).reset();
      //getComponent<T>() = T();
   }
  
};
