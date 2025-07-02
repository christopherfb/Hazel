#pragma once
#include "OrthographicCamera.h"
#include "Texture.h"

namespace Hazel {

	struct DrawQuadDefaultParams {
		glm::vec4& tint = glm::vec4(1.0f);
		float rotationInDeg = 0.0f;
		float tilingFactor = 1.0f;
	};

	class Renderer2D {

	public:

		static void Init();
		static void Shutdown();

		static void BeginScene(const OrthographicCamera& camera);
		static void EndScene();
		static void Flush();

		// Primitives
		
		static void DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color, const DrawQuadDefaultParams& p = drawQuadDefaultParams);
		static void DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color, const DrawQuadDefaultParams& p = drawQuadDefaultParams);

		static void DrawQuad(const glm::vec2& position, const glm::vec2& size, const Ref<Texture2D>& texture, const DrawQuadDefaultParams& p = drawQuadDefaultParams);
		static void DrawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<Texture2D>& texture, const DrawQuadDefaultParams& p = drawQuadDefaultParams);

		static const DrawQuadDefaultParams drawQuadDefaultParams;

		
	private:

	};
}
