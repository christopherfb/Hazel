#pragma once
#include "entt.hpp"

#include "Hazel/Core/Timestep.h"

namespace Hazel {
	class Entity;

	class Scene {
		friend class Entity;

	public:
		Scene();
		~Scene();

		void OnUpdate(Timestep ts);

		Entity CreateEntity(const std::string& name = "Entity");

		// TEMP
		entt::registry& Reg() { return m_Registry; }

	private:
		entt::registry m_Registry;
	};
}