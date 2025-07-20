#pragma once
#include "OrthographicCamera.h"
#include "Camera.h"
#include "Texture.h"
#include "Hazel/Renderer/SubTexture2D.h"

namespace Hazel {



	class Renderer2D {

	public:


		struct DrawQuadDefaultParams {
			glm::vec4 tint = glm::vec4(1.0f);
			float rotationInRad = 0.0f;
			float tilingFactor = 1.0f;
			glm::mat4 transform = glm::mat4(1.0f);
		};

		//static void Init();
		//static void Shutdown();


		//static void BeginScene(const Camera& camera, const glm::mat4& transform);
		//static void BeginScene(const OrthographicCamera& camera);		// TODO:: remove
		//static void EndScene();
		//static void Flush();

		static void Init();
		static void Shutdown();

		static void BeginScene(const Camera& camera, const glm::mat4& transform);
		static void BeginScene(const OrthographicCamera& camera); // TODO: Remove
		static void EndScene();
		static void Flush();

		// Primitives
		
		static void DrawQuad(const glm::vec2& position, const glm::vec2& size, const DrawQuadDefaultParams& p = drawQuadDefaultParams);
		static void DrawQuad(const glm::vec3& position, const glm::vec2& size, const DrawQuadDefaultParams& p = drawQuadDefaultParams);

		static void DrawQuad(const glm::vec2& position, const glm::vec2& size, const Ref<Texture2D>& texture, const DrawQuadDefaultParams& p = drawQuadDefaultParams);
		static void DrawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<Texture2D>& texture, const DrawQuadDefaultParams& p = drawQuadDefaultParams);
	
		static void DrawQuad(const glm::vec2& position, const glm::vec2& size, const Ref<SubTexture2D>& subtexture, const DrawQuadDefaultParams& p = drawQuadDefaultParams);
		static void DrawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<SubTexture2D>& subtexture, const DrawQuadDefaultParams& p = drawQuadDefaultParams);

		static void DrawQuad(const glm::mat4& transform, const DrawQuadDefaultParams& p = drawQuadDefaultParams);
		static void DrawQuad(const glm::mat4& transform, const Ref<Texture2D>& texture, const DrawQuadDefaultParams& p = drawQuadDefaultParams);

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
		//static void FlushAndReset();

		static float Renderer2D::GetTextureSlotIndex(const Ref<Texture2D>& texture);
		static void LoopThroughSDataAndAssignValues(float textureIndex, const glm::mat4& transform, const glm::vec2* textureCoords, const DrawQuadDefaultParams& p);
		static glm::mat4 Renderer2D::CalculateTransform(const glm::vec3& position, const glm::vec2& size, const DrawQuadDefaultParams& p);

	private:
		static void StartBatch();
		static void NextBatch();
	};
}
