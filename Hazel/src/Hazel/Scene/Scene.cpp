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

	}

	Scene::~Scene()
	{

	}



	void Scene::OnUpdate(Timestep ts)
	{
		// Update scripts
		{

			m_Registry.view<NativeScriptComponent>().each([=](auto entity, auto& nsc) {

				// TODO:: Move to Scene::OnScenePlay
				if (!nsc.Instance) {
					nsc.Instance = nsc.InstantiateScript();
					nsc.Instance->m_Entity = Entity{ entity, this };
					nsc.Instance->OnCreate();
				}
				nsc.Instance->OnUpdate(ts);

				});

		}

		Camera* mainCamera = nullptr;
		glm::mat4 cameraTransform;

		// Render sprites
		{
			// find the main camera (if we have one)
			auto view = m_Registry.view<TransformComponent, CameraComponent>();

			//for (auto entity : view) {
			//	auto [transform, camera] = view.get<TransformComponent, CameraComponent>(entity);

			//	if (camera.Primary) {
			//		mainCamera = &camera.Camera;
			//		cameraTransform = &transform.Transform;
			//		break;
			//	}
			//}

			// got this from chatGPT to replace the loop above (since it refused to compile)
			view.each([&](auto entity, auto& transform, auto& camera)
				{
					if (camera.Primary)
					{
						mainCamera = &camera.Camera;
						cameraTransform = transform.GetTransform();
					}
				});

		}

		// if a main camera exists - then render the sprites
		if (mainCamera) {
			Renderer2D::BeginScene(mainCamera->GetProjection(), cameraTransform);

			auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);

			for (auto entity : group)
			{
				auto [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);

				//Renderer2D::DrawQuad(transform, sprite.Color);
				Renderer2D::DrawQuadDefaultParams p;
				p.tint = sprite.Color;
				Renderer2D::DrawQuad(transform.GetTransform(), p);
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



	Entity Scene::CreateEntity(const std::string& name)
	{
		Entity entity = Entity(m_Registry.create(), this);
		entity.AddComponent<TransformComponent>();
		int32_t entityID = entity;
		HZ_CORE_TRACE("CreateEntity(): {0}", entityID);
		auto tag = entity.AddComponent<TagComponent>(name);
		return entity;
	}

	// TODO: maybe? shouldn't this be pass by ref?
	void Scene::DestroyEntity(Entity entity)
	{
		int32_t entityID = entity;

		HZ_CORE_TRACE("DestroyEntity(): {0}", entityID);
		m_Registry.destroy(entity);

	}

	template<typename T>
	void Scene::OnComponentAdded(Entity entity, T& component) {
		static_assert(false);
	}

	template<>
	void Scene::OnComponentAdded<CameraComponent>(Entity entity, CameraComponent& component) {
		component.Camera.SetViewportSize(mViewportWidth, mViewportHeight);
	}

	template<>
	void Scene::OnComponentAdded<NativeScriptComponent>(Entity entity, NativeScriptComponent& component) {
	}

	template<>
	void Scene::OnComponentAdded<SpriteRendererComponent>(Entity entity, SpriteRendererComponent& component) {
	}

	template<>
	void Scene::OnComponentAdded<TagComponent>(Entity entity, TagComponent& component) {
	}

	template<>
	void Scene::OnComponentAdded<TransformComponent>(Entity entity, TransformComponent& component) {
	}

}