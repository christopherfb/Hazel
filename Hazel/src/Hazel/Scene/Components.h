#pragma once
#include <glm/glm.hpp>

namespace Hazel {



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
			int i;

		}


	};
}