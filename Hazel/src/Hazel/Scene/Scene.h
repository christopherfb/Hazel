#pragma once
#include "entt.hpp"

#include "Hazel/Core/Timestep.h"


namespace Hazel {
	class Entity;

	class Scene {
		friend class Entity;
		friend class SceneHierarchyPanel;

	public:
		Scene();
		~Scene();

		void OnUpdate(Timestep ts);
		void OnViewportResize(uint32_t width, uint32_t height);

		Entity CreateEntity(const std::string& name = "Entity");
		void DestroyEntity(Entity entity);


		// TEMP
		entt::registry& Reg() { return m_Registry; }

	private:
		template<typename T>
		void OnComponentAdded(Entity entity, T& component);

	private:
		entt::registry m_Registry;
		uint32_t mViewportWidth = 0;
		uint32_t mViewportHeight = 0;

	};
}