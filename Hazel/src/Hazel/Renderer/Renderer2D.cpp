#include "hzpch.h"
#include "Renderer2D.h"

#include "VertexArray.h"
#include "Shader.h"
#include "RenderCommand.h"
//#include "Platform/OpenGL/OpenGLShader.h"
#include <glm/gtc/matrix_transform.hpp>


namespace Hazel {
	
    const DrawQuadDefaultParams Renderer2D::drawQuadDefaultParams;

	struct Renderer2DStorage
	{
		Ref<VertexArray> QuadVertexArray;
		Ref<Shader> TextureShader;
		Ref<Texture2D> WhiteTexture;
	};

	static Renderer2DStorage* s_Data;

	void Renderer2D::Init()
	{
		HZ_PROFILE_FUNCTION();

		s_Data = new Renderer2DStorage();

		// DRAW A SQUARE ////////////////////////////////////////////
		s_Data->QuadVertexArray = VertexArray::Create();

		float squareVertices[5 * 4] = {
			-0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
			0.5f, -0.5f, 0.0f , 1.0f, 0.0f,
			0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
			-0.5f,  0.5f, 0.0f, 0.0f, 1.0f
		};
		Ref<VertexBuffer> squareVB;
		squareVB.reset(VertexBuffer::Create(squareVertices, sizeof(squareVertices)));

		BufferLayout squareVBLayout = {
			{ShaderDataType::Float3, "a_Position" },
			{ShaderDataType::Float2, "a_TexCoord" }
		};
		squareVB->SetLayout(squareVBLayout);
		s_Data->QuadVertexArray->AddVertexBuffer(squareVB);

		uint32_t squareIndices[6] = { 0,1,2,2,3,0 };

		Ref<IndexBuffer> squareIB;
		squareIB.reset(IndexBuffer::Create(squareIndices, sizeof(squareIndices) / sizeof(uint32_t)));
		s_Data->QuadVertexArray->SetIndexBuffer(squareIB);


		s_Data->WhiteTexture = Texture2D::Create(1, 1); 
		uint32_t whiteTextureData = 0xffffffff;

		s_Data->WhiteTexture->SetData(&whiteTextureData, sizeof(uint32_t) );

		//s_Data->FlatColorShader = Shader::Create("assets/shaders/FlatColor.glsl");
		s_Data->TextureShader = Shader::Create("assets/shaders/Texture.glsl");
		s_Data->TextureShader->Bind();
		s_Data->TextureShader->SetInt("u_Texture", 0);
	}

	void Renderer2D::Shutdown()
	{
		HZ_PROFILE_FUNCTION();

		delete s_Data;
	}

	void Renderer2D::BeginScene(const OrthographicCamera& camera)
	{
		HZ_PROFILE_FUNCTION();
		s_Data->TextureShader->Bind();
		s_Data->TextureShader->SetMat4("u_ViewProjection", camera.GetViewProjectionMatrix());
	}

	void Renderer2D::EndScene()
	{
		HZ_PROFILE_FUNCTION();
	}



	void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color, const DrawQuadDefaultParams& p)
	{
		DrawQuad({ position.x, position.y, 0.0f }, size, color, p);
	}

	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color, const DrawQuadDefaultParams& p)
	{
		HZ_PROFILE_FUNCTION();
		// don't need to bind if already bound - but video 51 said it was possible to get to that state
		// Binding 2x wont' hurt, but maybe track with a bool to optimize?
		//s_Data->FlatColorShader->Bind();
		s_Data->TextureShader->SetFloat4("u_Color", color);
		s_Data->TextureShader->SetFloat("u_TilingFactor", 1.0);
		s_Data->WhiteTexture->Bind();

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position);
		if (p.rotation != 0.0f) {
			transform *= glm::rotate(glm::mat4(1.0f), p.rotation, { 0, 0, 1.0f });
		}
		transform *= glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });
		
		s_Data->TextureShader->SetMat4("u_Transform", transform);

		s_Data->QuadVertexArray->Bind();
		RenderCommand::DrawIndexed(s_Data->QuadVertexArray);
	}

	void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const Ref<Texture2D>& texture, const DrawQuadDefaultParams& p)
	{
		DrawQuad({ position.x, position.y, 0.0f }, size, texture, p);
	}

	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<Texture2D>& texture, const DrawQuadDefaultParams& p)
	{
		HZ_PROFILE_FUNCTION();
		s_Data->TextureShader->SetFloat4("u_Color", p.tint);
		s_Data->TextureShader->SetFloat("u_TilingFactor", p.tilingFactor);
		texture->Bind();

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position);
		if (p.rotation != 0.0f) {
			transform *= glm::rotate(glm::mat4(1.0f), p.rotation, { 0, 0, 1.0f });
		}
		transform *= glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

		s_Data->TextureShader->SetMat4("u_Transform", transform);

		s_Data->QuadVertexArray->Bind();
		RenderCommand::DrawIndexed(s_Data->QuadVertexArray);
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
	//	//s_Data->FlatColorShader->Bind();
	//	s_Data->TextureShader->SetFloat4("u_Color", color);
	//	s_Data->TextureShader->SetFloat("u_TilingFactor", 1.0);
	//	s_Data->WhiteTexture->Bind();

	//	glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) *  /* TODO:  * rotation  */
	//		glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

	//	s_Data->TextureShader->SetMat4("u_Transform", transform);

	//	s_Data->QuadVertexArray->Bind();
	//	RenderCommand::DrawIndexed(s_Data->QuadVertexArray);
	//}

	//void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const Ref<Texture2D>& texture, const glm::vec4& tint, float tilingFactor)
	//{
	//	DrawQuad({ position.x, position.y, 0.0f }, size, texture, tint);
	//}

	//void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<Texture2D>& texture, const glm::vec4& tint, float tilingFactor)
	//{
	//	HZ_PROFILE_FUNCTION();
	//	//s_Data->TextureShader->SetFloat4("u_Color", glm::vec4(1.0f));
	//	s_Data->TextureShader->SetFloat4("u_Color", tint);
	//	s_Data->TextureShader->SetFloat("u_TilingFactor", tilingFactor);


	//	//s_Data->TextureShader->Bind();
	//	texture->Bind();

	//	glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) *  /* TODO:  * rotation  */
	//		glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

	//	s_Data->TextureShader->SetMat4("u_Transform", transform);

	//	s_Data->QuadVertexArray->Bind();
	//	RenderCommand::DrawIndexed(s_Data->QuadVertexArray);
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
	//	//s_Data->FlatColorShader->Bind();
	//	s_Data->TextureShader->SetFloat4("u_Color", color);
	//	s_Data->TextureShader->SetFloat("u_TilingFactor", 1.0);
	//	s_Data->WhiteTexture->Bind();

	//	glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)   /* TODO:  * rotation  */
	//		* glm::rotate(glm::mat4(1.0f), rotation, { 0, 0, 1.0f })
	//		* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });
	//		

	//	s_Data->TextureShader->SetMat4("u_Transform", transform);

	//	s_Data->QuadVertexArray->Bind();
	//	RenderCommand::DrawIndexed(s_Data->QuadVertexArray);
	//}

	//void Renderer2D::DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, const Ref<Texture2D>& texture, float rotation, const glm::vec4& tint, float tilingFactor)
	//{
	//	DrawRotatedQuad({ position.x, position.y, 0.0f }, size, texture, rotation, tint);
	//}

	//void Renderer2D::DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, const Ref<Texture2D>& texture, float rotation, const glm::vec4& tint, float tilingFactor)
	//{
	//	HZ_PROFILE_FUNCTION();
	//	//s_Data->TextureShader->SetFloat4("u_Color", glm::vec4(1.0f));
	//	s_Data->TextureShader->SetFloat4("u_Color", tint);
	//	s_Data->TextureShader->SetFloat("u_TilingFactor", tilingFactor);


	//	//s_Data->TextureShader->Bind();
	//	texture->Bind();

	//	glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)   /* TODO:  * rotation  */
	//		* glm::rotate(glm::mat4(1.0f), rotation, { 0, 0, 1.0f })
	//		* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

	//	s_Data->TextureShader->SetMat4("u_Transform", transform);

	//	s_Data->QuadVertexArray->Bind();
	//	RenderCommand::DrawIndexed(s_Data->QuadVertexArray);
	//}

}