#pragma once
#include "OrthographicCamera.h"
#include "Texture.h"

namespace Hazel {



	class Renderer2D {

	public:

		struct DrawQuadDefaultParams {
			glm::vec4& tint = glm::vec4(1.0f);
			float rotationInRad = 0.0f;
			float tilingFactor = 1.0f;
		};

		static void Init();
		static void Shutdown();

		static void BeginScene(const OrthographicCamera& camera);
		static void EndScene();
		static void Flush();

		// Primitives
		
		static void DrawQuad(const glm::vec2& position, const glm::vec2& size, const DrawQuadDefaultParams& p = drawQuadDefaultParams);
		static void DrawQuad(const glm::vec3& position, const glm::vec2& size, const DrawQuadDefaultParams& p = drawQuadDefaultParams);

		static void DrawQuad(const glm::vec2& position, const glm::vec2& size, const Ref<Texture2D>& texture, const DrawQuadDefaultParams& p = drawQuadDefaultParams);
		static void DrawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<Texture2D>& texture, const DrawQuadDefaultParams& p = drawQuadDefaultParams);

		static const DrawQuadDefaultParams drawQuadDefaultParams;


		struct Statistics {
			uint32_t DrawCalls = 0;
			uint32_t QuadCount = 0;
			uint32_t GetTotalVertexCount() { return QuadCount * 4; }
			uint32_t GetTotalIndexCount() { return QuadCount * 6; }
		};

		static void ResetStats();
		static Statistics GetStats();
		
	private:
		static void FlushAndReset();
	};
}
