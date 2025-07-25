#pragma once
#include "Scene.h"
#include "entt.hpp"

namespace Hazel {


	class Entity {
	public:
		Entity() {}
		Entity(entt::entity handle, Scene* scene);
		Entity(const Entity& other) = default;


		template<typename T, typename... Args>
		T& AddComponent(Args&&... args)
		{
			HZ_CORE_ASSERT(!HasComponent<T>(), "Entity already has component!")
			// this syntax allows us to pass on a variable number of arguments (in a template fashion)
			T& component = m_Scene->m_Registry.emplace<T>(m_EntityHandle, std::forward<Args>(args)...);
			m_Scene->OnComponentAdded<T>(*this, component);
			return component;
		}
		
		template<typename T>
		T& GetComponent() {

			HZ_CORE_ASSERT(HasComponent<T>(), "Entity does not have component!")
			return m_Scene->m_Registry.get<T>(m_EntityHandle);
		}

		template<typename T>
		bool HasComponent() {
			//return m_Scene->m_Registry.all_of<T>(m_EntityHandle);  // the new way
			return m_Scene->m_Registry.has<T>(m_EntityHandle);	// the old way
		}

		template<typename T>
		void RemoveComponent() {
			HZ_CORE_ASSERT(HasComponent<T>(), "Entity does not have component!")
			m_Scene->m_Registry.remove<T>(m_EntityHandle);
		}

		explicit operator bool() const {	return m_EntityHandle != entt::null; }

		operator entt::entity() const { return m_EntityHandle; }

		operator uint32_t() const { return (uint32_t)m_EntityHandle; }

		bool operator==(const Entity& other) {
			return m_EntityHandle == other.m_EntityHandle &&
				m_Scene == other.m_Scene;
		}	
		
		bool operator!=(const Entity& other) {
			return !(*this == other);
		}


	private:
		entt::entity m_EntityHandle = { entt::null };
		Scene* m_Scene = nullptr;
	};




}