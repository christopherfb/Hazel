#include "hzpch.h"
#include "Scene.h"
#include "Components.h"
#include "Hazel/Renderer/Renderer2D.h"
#include <glm/glm.hpp>
#include "Entity.h"
#include "entt.hpp"


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
		// Update scripts
		{
			m_Registry.view<NativeScriptComponent>().each([=](auto entity, auto& nsc) {
				if (!nsc.Instance) {
					nsc.InstantiateFunction();
					nsc.Instance->m_Entity = Entity{ entity, this };
					nsc.OnCreateFunction(nsc.Instance);
				}
				nsc.OnUpdateFunction(nsc.Instance, ts);

				});

		}

		Camera* mainCamera = nullptr;
		glm::mat4* cameraTransform = nullptr;

		// Render sprites
		{
			// find the main camera (if we have one)
			//auto group = m_Registry.group<TransformComponent, CameraComponent>();
			//for (auto entity : group) {
			//auto& [transform, camera] = group.get<TransformComponent, CameraComponent>(entity);

			//if (camera.Primary) {
			//	mainCamera = &camera.Camera;
			//	cameraTransform = &transform.Transform;
			//	break;
			//}

			auto view = m_Registry.view<TransformComponent, CameraComponent>();
			view.each([&](auto entity, auto& transform, auto& camera)
				{
					if (camera.Primary)
					{
						mainCamera = &camera.Camera;
						cameraTransform = &transform.Transform;
					}
				});
			
		}

		// if a main camera exists - then render the sprites
		if (mainCamera) {
			Renderer2D::BeginScene(mainCamera->GetProjection(), *cameraTransform);

			auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
			for (auto entity : group)
			{
				auto& [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);

				//Renderer2D::DrawQuad(transform, sprite.Color);
				Renderer2D::DrawQuadDefaultParams p;
				p.tint = sprite.Color;
				Renderer2D::DrawQuad(transform, p);
			}

			Renderer2D::EndScene();
		}
	}

	void Scene::OnViewportResize(uint32_t width, uint32_t height)
	{
		HZ_WARN("OnViewportResize()");
		mViewportWidth = width;
		mViewportHeight = height;

		// Resize our non-fixed aspect ratio cameras
		auto view = m_Registry.view<CameraComponent>();
		for (auto entity : view) {
			auto& cameraComponent = view.get<CameraComponent>(entity);
			if (!cameraComponent.FixedAspectRatio) {
				cameraComponent.Camera.SetViewportSize(width, height);
			}
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