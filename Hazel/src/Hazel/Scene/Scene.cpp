#include "hzpch.h"
#include "Scene.h"
#include <glm/glm.hpp>


namespace Hazel {
	


	Scene::Scene()
	{
		struct MeshComponent{};

		struct TransformComponent {
			glm::mat4 Transform;

			TransformComponent() = default;								// ctor
			TransformComponent(const TransformComponent&) = default;	// copy ctor
			TransformComponent(const glm::mat4& transform)
				: Transform(transform) {}

			operator glm::mat4() { return Transform; }
			operator const glm::mat4&() { return Transform; }
		};

		entt::entity entity = m_Registry.create();
	    m_Registry.emplace<TransformComponent>(entity, glm::mat4(1.0f));

		if (m_Registry.all_of<TransformComponent>(entity)) {
			TransformComponent& transform = m_Registry.get<TransformComponent>(entity);
		}

		// to iterate through all entities with transform component:
		auto view = m_Registry.view<TransformComponent>();
		for (auto entity : view) {
			TransformComponent& transform = view.get<TransformComponent>(entity);
		}

		auto group = m_Registry.group<TransformComponent>(entt::get<MeshComponent>);
		for (auto entity : group) {
			//auto&[transform, mesh] = group.get<TransformComponent, MeshComponent>(entity);
		}
	}

	Scene::~Scene()
	{
	}

}