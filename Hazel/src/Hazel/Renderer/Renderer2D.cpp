#include "hzpch.h"
#include "Renderer2D.h"

#include "VertexArray.h"
#include "Shader.h"
#include "RenderCommand.h"
//#include "Platform/OpenGL/OpenGLShader.h"
#include <glm/gtc/matrix_transform.hpp>


namespace Hazel {
	
    const DrawQuadDefaultParams Renderer2D::drawQuadDefaultParams;

	struct QuadVertex {
		glm::vec3 Position;
		glm::vec4 Color;
		glm::vec2 TexCoord;
		// TODO: texid, maskid
	};

	struct Renderer2DData
	{
		// maximums per draw call
		const uint32_t MaxQuads = 10000;
		const uint32_t MaxVertices = MaxQuads * 4;;
		const uint32_t MaxIndices = MaxQuads * 6;

		Ref<VertexArray> QuadVertexArray;
		Ref<VertexBuffer> QuadVertexBuffer;
		Ref<Shader> TextureShader;
		Ref<Texture2D> WhiteTexture;

		uint32_t QuadIndexCount = 0;  // increment by 6 every time we draw (6 indices per quad)
		QuadVertex* QuadVertexBufferBase = nullptr;
		QuadVertex* QuadVertexBufferPtr = nullptr;

	};


	static Renderer2DData s_Data;

	void Renderer2D::Init()
	{
		HZ_PROFILE_FUNCTION();

		//s_Data = new Renderer2DData();

		//// DRAW A SQUARE ////////////////////////////////////////////
		s_Data.QuadVertexArray = VertexArray::Create();

		//float squareVertices[5 * 4] = {
		//	-0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
		//	0.5f, -0.5f, 0.0f , 1.0f, 0.0f,
		//	0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
		//	-0.5f,  0.5f, 0.0f, 0.0f, 1.0f
		//};
		//Ref<VertexBuffer> quadVB = VertexBuffer::Create(squareVertices, sizeof(squareVertices));
		//Ref<VertexBuffer> quadVB = VertexBuffer::Create(s_Data.MaxVertices * sizeof(QuadVertex));
		s_Data.QuadVertexBuffer = VertexBuffer::Create(s_Data.MaxVertices * sizeof(QuadVertex));

		BufferLayout quadVBLayout = {
			{ShaderDataType::Float3, "a_Position" },
			{ShaderDataType::Float4, "a_Color" },
			{ShaderDataType::Float2, "a_TexCoord" }
		};
		s_Data.QuadVertexBuffer->SetLayout(quadVBLayout);
		s_Data.QuadVertexArray->AddVertexBuffer(s_Data.QuadVertexBuffer);

		s_Data.QuadVertexBufferBase = new QuadVertex[s_Data.MaxVertices];


		uint32_t* quadIndices = new uint32_t[s_Data.MaxIndices];  // 60,000 uint32_t's.. 240 kb
		uint32_t offset = 0;
		for (uint32_t i = 0; i < s_Data.MaxIndices; i+=6) {
			quadIndices[i + 0] = offset + 0;
			quadIndices[i + 1] = offset + 1;
			quadIndices[i + 2] = offset + 2;

			quadIndices[i + 3] = offset + 2;
			quadIndices[i + 4] = offset + 3;
			quadIndices[i + 5] = offset + 0;

			offset += 4;
		}
		Ref<IndexBuffer> squareIB = IndexBuffer::Create(quadIndices, s_Data.MaxIndices);
		s_Data.QuadVertexArray->SetIndexBuffer(squareIB);
		delete[] quadIndices;


		//uint32_t squareIndices[6] = { 0,1,2,2,3,0 };



		s_Data.WhiteTexture = Texture2D::Create(1, 1); 
		uint32_t whiteTextureData = 0xffffffff;

		s_Data.WhiteTexture->SetData(&whiteTextureData, sizeof(uint32_t) );

		//s_Data.FlatColorShader = Shader::Create("assets/shaders/FlatColor.glsl");
		s_Data.TextureShader = Shader::Create("assets/shaders/Texture.glsl");
		s_Data.TextureShader->Bind();
		s_Data.TextureShader->SetInt("u_Texture", 0);
	}

	void Renderer2D::Shutdown()
	{
		HZ_PROFILE_FUNCTION();

		//delete s_Data;
	}

	void Renderer2D::BeginScene(const OrthographicCamera& camera)
	{
		HZ_PROFILE_FUNCTION();
		s_Data.TextureShader->Bind();
		s_Data.TextureShader->SetMat4("u_ViewProjection", camera.GetViewProjectionMatrix());

		s_Data.QuadIndexCount = 0;
		s_Data.QuadVertexBufferPtr = s_Data.QuadVertexBufferBase;
	}

	void Renderer2D::EndScene()
	{
		HZ_PROFILE_FUNCTION();

		uint32_t dataSize = (uint8_t*)s_Data.QuadVertexBufferPtr - (uint8_t*)s_Data.QuadVertexBufferBase;
		s_Data.QuadVertexBuffer->SetData(s_Data.QuadVertexBufferBase, dataSize);
		Flush();
	}

	void Renderer2D::Flush()
	{
		RenderCommand::DrawIndexed(s_Data.QuadVertexArray, s_Data.QuadIndexCount);
	}



	void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color, const DrawQuadDefaultParams& p)
	{
		DrawQuad({ position.x, position.y, 0.0f }, size, color, p);
	}

	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color, const DrawQuadDefaultParams& p)
	{
		HZ_PROFILE_FUNCTION();


		s_Data.QuadVertexBufferPtr->Position = position;
		s_Data.QuadVertexBufferPtr->Color = color;
		s_Data.QuadVertexBufferPtr->TexCoord = { 0.0f, 0.0f };
		s_Data.QuadVertexBufferPtr++;

		s_Data.QuadVertexBufferPtr->Position = { position.x + size.x, position.y , 0.0f };
		s_Data.QuadVertexBufferPtr->Color = color;
		s_Data.QuadVertexBufferPtr->TexCoord = { 1.0f, 0.0f };
		s_Data.QuadVertexBufferPtr++;

		s_Data.QuadVertexBufferPtr->Position = { position.x + size.x, position.y + size.y , 0.0f };
		s_Data.QuadVertexBufferPtr->Color = color;
		s_Data.QuadVertexBufferPtr->TexCoord = { 1.0f, 1.0f };
		s_Data.QuadVertexBufferPtr++;

		s_Data.QuadVertexBufferPtr->Position = { position.x, position.y + size.y , 0.0f };
		s_Data.QuadVertexBufferPtr->Color = color;
		s_Data.QuadVertexBufferPtr->TexCoord = { 0.0f, 1.0f };
		s_Data.QuadVertexBufferPtr++;

		s_Data.QuadIndexCount += 6;

		// don't need to bind if already bound - but video 51 said it was possible to get to that state
		// Binding 2x wont' hurt, but maybe track with a bool to optimize?
		//s_Data.FlatColorShader->Bind();
		//s_Data.TextureShader->SetFloat4("u_Color", color);

		// this now happens when we flush our renderer at the end of our batch.
		//s_Data.TextureShader->SetFloat("u_TilingFactor", 1.0);
		//s_Data.WhiteTexture->Bind();

		//glm::mat4 transform = glm::translate(glm::mat4(1.0f), position);
		//if (p.rotation != 0.0f) {
		//	transform *= glm::rotate(glm::mat4(1.0f), p.rotation, { 0, 0, 1.0f });
		//}
		//transform *= glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });
		//
		//s_Data.TextureShader->SetMat4("u_Transform", transform);

		//s_Data.QuadVertexArray->Bind();
		//RenderCommand::DrawIndexed(s_Data.QuadVertexArray);
	}

	void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const Ref<Texture2D>& texture, const DrawQuadDefaultParams& p)
	{
		DrawQuad({ position.x, position.y, 0.0f }, size, texture, p);
	}

	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<Texture2D>& texture, const DrawQuadDefaultParams& p)
	{
		HZ_PROFILE_FUNCTION();
		s_Data.TextureShader->SetFloat4("u_Color", p.tint);
		s_Data.TextureShader->SetFloat("u_TilingFactor", p.tilingFactor);
		texture->Bind();

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position);
		if (p.rotation != 0.0f) {
			transform *= glm::rotate(glm::mat4(1.0f), p.rotation, { 0, 0, 1.0f });
		}
		transform *= glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

		s_Data.TextureShader->SetMat4("u_Transform", transform);

		s_Data.QuadVertexArray->Bind();
		RenderCommand::DrawIndexed(s_Data.QuadVertexArray);
	}


	// I would have had to write and maintain all this if I hadn't used DrawQuadDefaultParams.  Yuck!

	//void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color)
	//{
	//	DrawQuad({ position.x, position.y, 0.0f }, size, color);
	//}

	//void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color)
	//{
	//	HZ_PROFILE_FUNCTION();
	//	// don't need to bind if already bound - but video 51 said it was possible to get to that state
	//	// Binding 2x wont' hurt, but maybe track with a bool to optimize?
	//	//s_Data.FlatColorShader->Bind();
	//	s_Data.TextureShader->SetFloat4("u_Color", color);
	//	s_Data.TextureShader->SetFloat("u_TilingFactor", 1.0);
	//	s_Data.WhiteTexture->Bind();

	//	glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) *  /* TODO:  * rotation  */
	//		glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

	//	s_Data.TextureShader->SetMat4("u_Transform", transform);

	//	s_Data.QuadVertexArray->Bind();
	//	RenderCommand::DrawIndexed(s_Data.QuadVertexArray);
	//}

	//void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const Ref<Texture2D>& texture, const glm::vec4& tint, float tilingFactor)
	//{
	//	DrawQuad({ position.x, position.y, 0.0f }, size, texture, tint);
	//}

	//void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<Texture2D>& texture, const glm::vec4& tint, float tilingFactor)
	//{
	//	HZ_PROFILE_FUNCTION();
	//	//s_Data.TextureShader->SetFloat4("u_Color", glm::vec4(1.0f));
	//	s_Data.TextureShader->SetFloat4("u_Color", tint);
	//	s_Data.TextureShader->SetFloat("u_TilingFactor", tilingFactor);


	//	//s_Data.TextureShader->Bind();
	//	texture->Bind();

	//	glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) *  /* TODO:  * rotation  */
	//		glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

	//	s_Data.TextureShader->SetMat4("u_Transform", transform);

	//	s_Data.QuadVertexArray->Bind();
	//	RenderCommand::DrawIndexed(s_Data.QuadVertexArray);
	//}

	//void Renderer2D::DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const glm::vec4& color)
	//{
	//	DrawRotatedQuad({ position.x, position.y, 0.0f }, size, rotation, color);
	//}

	//void Renderer2D::DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const glm::vec4& color)
	//{
	//	HZ_PROFILE_FUNCTION();
	//	// don't need to bind if already bound - but video 51 said it was possible to get to that state
	//	// Binding 2x wont' hurt, but maybe track with a bool to optimize?
	//	//s_Data.FlatColorShader->Bind();
	//	s_Data.TextureShader->SetFloat4("u_Color", color);
	//	s_Data.TextureShader->SetFloat("u_TilingFactor", 1.0);
	//	s_Data.WhiteTexture->Bind();

	//	glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)   /* TODO:  * rotation  */
	//		* glm::rotate(glm::mat4(1.0f), rotation, { 0, 0, 1.0f })
	//		* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });
	//		

	//	s_Data.TextureShader->SetMat4("u_Transform", transform);

	//	s_Data.QuadVertexArray->Bind();
	//	RenderCommand::DrawIndexed(s_Data.QuadVertexArray);
	//}

	//void Renderer2D::DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, const Ref<Texture2D>& texture, float rotation, const glm::vec4& tint, float tilingFactor)
	//{
	//	DrawRotatedQuad({ position.x, position.y, 0.0f }, size, texture, rotation, tint);
	//}

	//void Renderer2D::DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, const Ref<Texture2D>& texture, float rotation, const glm::vec4& tint, float tilingFactor)
	//{
	//	HZ_PROFILE_FUNCTION();
	//	//s_Data.TextureShader->SetFloat4("u_Color", glm::vec4(1.0f));
	//	s_Data.TextureShader->SetFloat4("u_Color", tint);
	//	s_Data.TextureShader->SetFloat("u_TilingFactor", tilingFactor);


	//	//s_Data.TextureShader->Bind();
	//	texture->Bind();

	//	glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)   /* TODO:  * rotation  */
	//		* glm::rotate(glm::mat4(1.0f), rotation, { 0, 0, 1.0f })
	//		* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

	//	s_Data.TextureShader->SetMat4("u_Transform", transform);

	//	s_Data.QuadVertexArray->Bind();
	//	RenderCommand::DrawIndexed(s_Data.QuadVertexArray);
	//}

}