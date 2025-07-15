#pragma once
#include <glm/glm.hpp>
#include "Hazel/Renderer/Camera.h"

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
		Hazel::Camera Camera;
		bool Primary = true;		// TODO: think about moving to Scene

		CameraComponent() = default;						// ctor
		CameraComponent(const CameraComponent&) = default;	// copy ctor
		CameraComponent(const glm::mat4& projection)
			: Camera(projection) {

		}
	};
}