#include "hzpch.h"
#include "Renderer2D.h"

#include "VertexArray.h"
#include "Shader.h"
#include "RenderCommand.h"
//#include "Platform/OpenGL/OpenGLShader.h"
#include <glm/gtc/matrix_transform.hpp>



namespace Hazel {
	
    const Renderer2D::DrawQuadDefaultParams Renderer2D::drawQuadDefaultParams;

	struct QuadVertex {
		glm::vec3 Position;
		glm::vec4 Color;
		glm::vec2 TexCoord;
		float TexIndex;
		float TilingFactor;
		// TODO: maskid
	};

	struct Renderer2DData
	{
		// maximums per draw call
		static const uint32_t MaxQuads = 10000;
		static const uint32_t MaxVertices = MaxQuads * 4;;
		static const uint32_t MaxIndices = MaxQuads * 6;
		static const uint32_t MaxTextureSlots = 32;		// TODO: RenderCaps

		Ref<VertexArray> QuadVertexArray;
		Ref<VertexBuffer> QuadVertexBuffer;
		Ref<Shader> TextureShader;
		Ref<Texture2D> WhiteTexture;

		uint32_t QuadIndexCount = 0;  // increment by 6 every time we draw (6 indices per quad)
		QuadVertex* QuadVertexBufferBase = nullptr;
		QuadVertex* QuadVertexBufferPtr = nullptr;

		std::array<Ref<Texture2D>, MaxTextureSlots> TextureSlots;
		uint32_t TextureSlotIndex = 1;  // 0 == white texture

		glm::vec4 QuadVertexPositions[4];
		glm::vec2 TextureCoordinates[4] = { { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f } };

		
		Renderer2D::Statistics Stats;
	};


	static Renderer2DData s_Data;


void Renderer2D::Init()
{
	HZ_PROFILE_FUNCTION();

	s_Data.QuadVertexArray = VertexArray::Create();

	s_Data.QuadVertexBuffer = VertexBuffer::Create(s_Data.MaxVertices * sizeof(QuadVertex));
	s_Data.QuadVertexBuffer->SetLayout({
		{ ShaderDataType::Float3, "a_Position" },
		{ ShaderDataType::Float4, "a_Color" },
		{ ShaderDataType::Float2, "a_TexCoord" },
		{ ShaderDataType::Float, "a_TexIndex" },
		{ ShaderDataType::Float, "a_TilingFactor" }
		});
	s_Data.QuadVertexArray->AddVertexBuffer(s_Data.QuadVertexBuffer);

	s_Data.QuadVertexBufferBase = new QuadVertex[s_Data.MaxVertices];

	uint32_t* quadIndices = new uint32_t[s_Data.MaxIndices];

	uint32_t offset = 0;
	for (uint32_t i = 0; i < s_Data.MaxIndices; i += 6)
	{
		quadIndices[i + 0] = offset + 0;
		quadIndices[i + 1] = offset + 1;
		quadIndices[i + 2] = offset + 2;

		quadIndices[i + 3] = offset + 2;
		quadIndices[i + 4] = offset + 3;
		quadIndices[i + 5] = offset + 0;

		offset += 4;
	}

	Ref<IndexBuffer> quadIB = IndexBuffer::Create(quadIndices, s_Data.MaxIndices);
	s_Data.QuadVertexArray->SetIndexBuffer(quadIB);
	delete[] quadIndices;

	s_Data.WhiteTexture = Texture2D::Create(1, 1);
	uint32_t whiteTextureData = 0xffffffff;
	s_Data.WhiteTexture->SetData(&whiteTextureData, sizeof(uint32_t));

	int32_t samplers[s_Data.MaxTextureSlots];
	for (uint32_t i = 0; i < s_Data.MaxTextureSlots; i++)
		samplers[i] = i;

	s_Data.TextureShader = Shader::Create("assets/shaders/Texture.glsl");
	s_Data.TextureShader->Bind();
	s_Data.TextureShader->SetIntArray("u_Textures", samplers, s_Data.MaxTextureSlots);

	// Set first texture slot to 0
	s_Data.TextureSlots[0] = s_Data.WhiteTexture;

	s_Data.QuadVertexPositions[0] = { -0.5f, -0.5f, 0.0f, 1.0f };
	s_Data.QuadVertexPositions[1] = { 0.5f, -0.5f, 0.0f, 1.0f };
	s_Data.QuadVertexPositions[2] = { 0.5f,  0.5f, 0.0f, 1.0f };
	s_Data.QuadVertexPositions[3] = { -0.5f,  0.5f, 0.0f, 1.0f };
}

void Renderer2D::Shutdown()
{
	HZ_PROFILE_FUNCTION();

	delete[] s_Data.QuadVertexBufferBase;
}

void Renderer2D::BeginScene(const OrthographicCamera& camera)
{
	HZ_PROFILE_FUNCTION();

	s_Data.TextureShader->Bind();
	s_Data.TextureShader->SetMat4("u_ViewProjection", camera.GetViewProjectionMatrix());

	StartBatch();
}

void Renderer2D::BeginScene(const Camera& camera, const glm::mat4& transform)
{
	HZ_PROFILE_FUNCTION();

	glm::mat4 viewProj = camera.GetProjection() * glm::inverse(transform);

	s_Data.TextureShader->Bind();
	s_Data.TextureShader->SetMat4("u_ViewProjection", viewProj);

	StartBatch();
}

void Renderer2D::EndScene()
{
	HZ_PROFILE_FUNCTION();

	Flush();
}

void Renderer2D::StartBatch()
{
	s_Data.QuadIndexCount = 0;
	s_Data.QuadVertexBufferPtr = s_Data.QuadVertexBufferBase;

	s_Data.TextureSlotIndex = 1;
}

void Renderer2D::Flush()
{
	if (s_Data.QuadIndexCount == 0)
		return; // Nothing to draw

	uint32_t dataSize = (uint32_t)((uint8_t*)s_Data.QuadVertexBufferPtr - (uint8_t*)s_Data.QuadVertexBufferBase);
	s_Data.QuadVertexBuffer->SetData(s_Data.QuadVertexBufferBase, dataSize);

	// Bind textures
	for (uint32_t i = 0; i < s_Data.TextureSlotIndex; i++)
		s_Data.TextureSlots[i]->Bind(i);

	RenderCommand::DrawIndexed(s_Data.QuadVertexArray, s_Data.QuadIndexCount);
	s_Data.Stats.DrawCalls++;
}

void Renderer2D::NextBatch()
{
	Flush();
	StartBatch();
}




	glm::mat4 Renderer2D::CalculateTransform(const glm::vec3& position, const glm::vec2& size, const DrawQuadDefaultParams& p) {
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position);
		if (p.rotationInRad != 0.0f) {
			transform *= glm::rotate(glm::mat4(1.0f), p.rotationInRad, { 0.0f, 0.0f, 1.0f });
		}
		transform *= glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });
		return transform;
	}
	


	void Renderer2D::LoopThroughSDataAndAssignValues(float textureIndex, const glm::mat4& transform, const glm::vec2* textureCoords, const DrawQuadDefaultParams& p) {

		if (s_Data.QuadIndexCount >= Renderer2DData::MaxIndices) {
			//FlushAndReset();
			NextBatch();
		}

		for (int32_t i = 0; i < 4; i++) {
			s_Data.QuadVertexBufferPtr->Position = transform * s_Data.QuadVertexPositions[i];
			s_Data.QuadVertexBufferPtr->Color = p.tint;
			s_Data.QuadVertexBufferPtr->TexCoord = textureCoords[i]; // s_Data.TextureCoordinates[i];
			s_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
			s_Data.QuadVertexBufferPtr->TilingFactor = p.tilingFactor;
			s_Data.QuadVertexBufferPtr++;
		}

		s_Data.QuadIndexCount += 6;
		s_Data.Stats.QuadCount++;
	}


	void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const DrawQuadDefaultParams& p)
	{
		DrawQuad({ position.x, position.y, 0.0f }, size, p);
	}


	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const DrawQuadDefaultParams& p)
	{
		HZ_PROFILE_FUNCTION();
		glm::mat4 transform = CalculateTransform(position, size, p);
		DrawQuad(transform, p);
	}


	void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const Ref<Texture2D>& texture, const DrawQuadDefaultParams& p)
	{
		DrawQuad({ position.x, position.y, 0.0f }, size, texture, p);
	}


	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<Texture2D>& texture, const DrawQuadDefaultParams& p)
	{
		HZ_PROFILE_FUNCTION();
		HZ_CORE_ASSERT(texture, "Attempting to DrawQuad() with an empty texture!");

		// Try to find texture slot
		float textureIndex = GetTextureSlotIndex(texture);
		glm::mat4 transform = CalculateTransform(position, size, p);
		
		DrawQuad(transform, texture, p);
	}


	void Renderer2D::DrawQuad(const glm::mat4& transform, const DrawQuadDefaultParams& p)
	{
		HZ_PROFILE_FUNCTION();
		const float textureIndex = 0.0f;  // white texture
		LoopThroughSDataAndAssignValues(textureIndex, transform, s_Data.TextureCoordinates, p);
	}


	void Renderer2D::DrawQuad(const glm::mat4& transform, const Ref<Texture2D>& texture, const DrawQuadDefaultParams& p)
	{
		HZ_CORE_ASSERT(texture, "Attempting to DrawQuad() with an empty texture!");
		float textureIndex = GetTextureSlotIndex(texture);
		LoopThroughSDataAndAssignValues(textureIndex, transform, s_Data.TextureCoordinates, p);
	}


	void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const Ref<SubTexture2D>& subtexture, const DrawQuadDefaultParams& p) {
		DrawQuad({ position.x, position.y, 0.0f }, size, subtexture, p);
	}


	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<SubTexture2D>& subtexture, const DrawQuadDefaultParams& p) {
		HZ_PROFILE_FUNCTION();

		float textureIndex = GetTextureSlotIndex(subtexture->GetTexture());
		glm::mat4 transform = CalculateTransform(position, size, p);

		const glm::vec2* textureCoords = subtexture->GetTexCoords();
		LoopThroughSDataAndAssignValues(textureIndex, transform, textureCoords, p);
	}





	void Renderer2D::ResetStats()
	{
		memset( &s_Data.Stats, 0, sizeof(Statistics));
	}

	Renderer2D::Statistics Renderer2D::GetStats()
	{
		return s_Data.Stats;
	}


	float Renderer2D::GetTextureSlotIndex(const Ref<Texture2D>& texture) {
		// Try to find texture slot
		float textureIndex = 0.0f;
		for (uint32_t i = 1; i < s_Data.TextureSlotIndex; i++)    // alt: if (*s_Data.TextureSlots[i] == *texture)
		{
			if (*s_Data.TextureSlots[i].get() == *texture.get()) {   // syntax is not great here
				textureIndex = (float)i;
				break;
			}
		}

		// Only assign a new slot if texture wasn't found
		if (textureIndex == 0.0f)
		{
			if (s_Data.TextureSlotIndex >= Renderer2DData::MaxTextureSlots)
			{
				// Optional: flush and restart batch here if you want more than 32 textures
				HZ_CORE_WARN("Texture slot limit exceeded!");
				textureIndex = 0.0f;  // fallback to white if out of slots
			}
			else
			{
				textureIndex = (float)s_Data.TextureSlotIndex;
				s_Data.TextureSlots[s_Data.TextureSlotIndex] = texture;
				s_Data.TextureSlotIndex++;
			}
		}
		return textureIndex;
	}




}