#include "hzpch.h"
#include "Scene.h"
#include "Components.h"
#include "Hazel/Renderer/Renderer2D.h"
#include <glm/glm.hpp>
#include "Entity.h"

namespace Hazel {
	
	/*static void OnTransformConstruct(entt::registry& registry, entt::entity ent) {

	}*/

	Scene::Scene()
	{
#if ENTT_EXAMPLE_CODE
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

#endif

	}

	Scene::~Scene()
	{
	
	}
	void Scene::OnUpdate(Timestep ts)
	{
		auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
		for (auto entity : group)
		{
			auto& [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);

			//Renderer2D::DrawQuad(transform, sprite.Color);
			Renderer2D::DrawQuadDefaultParams p;
			p.tint = sprite.Color;
			Renderer2D::DrawQuad(transform, p);
		}
	}

	Entity Scene::CreateEntity(const std::string& name )
	{
		Entity entity = Entity(m_Registry.create(), this);
		entity.AddComponent<TransformComponent>();
		auto tag = entity.AddComponent<TagComponent>(name);
		return entity;
	}

}