#pragma once
#include <glm/glm.hpp>
#include "Hazel/Scene/SceneCamera.h"
#include "ScriptableEntity.h"

namespace Hazel {

	struct TagComponent {
		std::string Tag;
		TagComponent() = default;
		TagComponent(const TagComponent&) = default;
		TagComponent(const std::string& tag)
			: Tag(tag) {}
	};

	struct TransformComponent {
		glm::mat4 Transform{ 1.0f };

		TransformComponent() = default;								// ctor
		TransformComponent(const TransformComponent&) = default;	// copy ctor
		TransformComponent(const glm::mat4& transform)
			: Transform(transform) {}

		operator glm::mat4() { return Transform; }
		operator const glm::mat4& () { return Transform; }
	};


	struct SpriteRendererComponent {
		glm::vec4 Color = { 1.0f, 1.0f, 1.0f, 1.0f };
	
		SpriteRendererComponent() = default;								// ctor
		SpriteRendererComponent(const SpriteRendererComponent&) = default;	// copy ctor
		SpriteRendererComponent(const glm::vec4& color)
			: Color(color) {

		}
	};

	struct CameraComponent {
		SceneCamera Camera;
		bool Primary = true;		// TODO: think about moving to Scene
		bool FixedAspectRatio = false;


		CameraComponent() = default;						// ctor
		CameraComponent(const CameraComponent&) = default;	// copy ctor
	};

	struct NativeScriptComponent {
		ScriptableEntity* Instance = nullptr;


		std::function<void()> InstantiateFunction;
		std::function<void()> DestroyInstanceFunction;

		std::function<void(ScriptableEntity*)> OnCreateFunction;
		std::function<void(ScriptableEntity*)> OnDestroyFunction;
		std::function<void(ScriptableEntity*, Timestep)> OnUpdateFunction;


		template<typename T>
		void Bind() {
			InstantiateFunction = [&]() { Instance = new T(); };
			DestroyInstanceFunction = [&]() { delete(T*)Instance; Instance = nullptr; };

			OnCreateFunction = [](ScriptableEntity* instance) { ((T*)instance)->OnCreate(); };
			OnDestroyFunction = [](ScriptableEntity* instance) { ((T*)instance)->OnDestroy(); };
			OnUpdateFunction = [](ScriptableEntity* instance, Timestep ts) { ((T*)instance)->OnUpdate(ts); };
		}
	};



}