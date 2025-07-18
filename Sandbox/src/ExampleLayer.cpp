
#include "hzpch.h"
#include "ExampleLayer.h"
#include <Hazel.h>
#include "Platform/OpenGL/OpenGLShader.h"
#include "imgui/imgui.h"
#include <glm/gtc/type_ptr.hpp>


ExampleLayer::ExampleLayer()
	: Layer("Example")
	, m_CameraController(1280.0f / 720.0f)  // 16:9			
{

	// DRAW A TRIANGLE ////////////////////////////////////////////  
	m_VertexArray = Hazel::VertexArray::Create();

	// this describes the triange. The first 3 are coords, the last 4 are colors
	float vertices[3 * 7] = {
		-0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
		0.5f, -0.5f, 0.0f,	0.0f, 0.0f, 1.0f, 1.0f,
		0.0f, 0.5f, 0.0f,	1.0f, 1.0f, 0.0f, 1.0f
	};

	Hazel::Ref<Hazel::VertexBuffer> vertexBuffer = Hazel::VertexBuffer::Create(vertices, sizeof(vertices));
	//vertexBuffer.reset(Hazel::VertexBuffer::Create(vertices, sizeof(vertices)));
	Hazel::BufferLayout layout = {
		{Hazel::ShaderDataType::Float3, "a_Position" },
		{Hazel::ShaderDataType::Float4, "a_Color" }
	};
	vertexBuffer->SetLayout(layout);
	m_VertexArray->AddVertexBuffer(vertexBuffer);

	uint32_t indices[3] = { 0,1,2 };
	Hazel::Ref<Hazel::IndexBuffer> indexBuffer = Hazel::IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t));
	//indexBuffer.reset(Hazel::IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t)));
	m_VertexArray->SetIndexBuffer(indexBuffer);

	std::string vertexSrc = R"(
		#version 330 core
		layout(location = 0) in vec3 a_Position;
		layout(location = 1) in vec4 a_Color;

		uniform mat4 u_ViewProjection;
		uniform mat4 u_Transform;

		out vec3 v_Position;
		out vec4 v_Color;

		void main()
		{
			v_Position = a_Position;
			v_Color = a_Color;
			gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);
		}
	)";

	std::string fragmentSrc = R"(
		#version 330 core
		layout(location = 0) out vec4 color;

		in vec3 v_Position;
		in vec4 v_Color;
		void main()
		{
			color = vec4(v_Position * 0.5 + 0.5, 1.0);
			color = v_Color;
		}
	)";

	m_Shader = Hazel::Shader::Create("VertexPosColor", vertexSrc, fragmentSrc);


	// DRAW A SQUARE ////////////////////////////////////////////
	m_SquareVA = Hazel::VertexArray::Create();

	float squareVertices[5 * 4] = {
		-0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
			0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
			0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
		-0.5f,  0.5f, 0.0f, 0.0f, 1.0f
	};
	Hazel::Ref<Hazel::VertexBuffer> squareVB = Hazel::VertexBuffer::Create(squareVertices, sizeof(squareVertices));
	//squareVB.reset(Hazel::VertexBuffer::Create(squareVertices, sizeof(squareVertices)));

	Hazel::BufferLayout squareVBLayout = {
		{Hazel::ShaderDataType::Float3, "a_Position" },
		{Hazel::ShaderDataType::Float2, "a_TexCoord" }
	};
	squareVB->SetLayout(squareVBLayout);
	m_SquareVA->AddVertexBuffer(squareVB);

	uint32_t squareIndices[6] = { 0,1,2,2,3,0 };

	Hazel::Ref<Hazel::IndexBuffer> squareIB = Hazel::IndexBuffer::Create(squareIndices, sizeof(squareIndices) / sizeof(uint32_t));
	//squareIB.reset(Hazel::IndexBuffer::Create(squareIndices, sizeof(squareIndices) / sizeof(uint32_t)));
	m_SquareVA->SetIndexBuffer(squareIB);

	///////////////////////
	std::string flatColorShaderVertexSrc = R"(
		#version 330 core
		layout(location = 0) in vec3 a_Position;
			
		uniform mat4 u_ViewProjection;
		uniform mat4 u_Transform;

		out vec3 v_Position;
		out vec4 v_Color;

		void main()
		{
			v_Position = a_Position;
			gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);
		}
	)";

	std::string flatColorShaderFragmentSrc = R"(
		#version 330 core
		layout(location = 0) out vec4 color;
		in vec3 v_Position;	

		uniform vec3 u_Color;

		void main()
		{
			color = vec4(u_Color, 1.0);				
		}
	)";

	m_FlatColorShader = Hazel::Shader::Create("FlatColor", flatColorShaderVertexSrc, flatColorShaderFragmentSrc);


	auto textureShader = m_ShaderLibrary.Load("assets/shaders/Texture.glsl");

	m_Texture = Hazel::Texture2D::Create("assets/textures/Checkerboard.png");
	m_ChernoLogoTexture = Hazel::Texture2D::Create("assets/textures/ChernoLogo.png");

	textureShader->Bind();
	std::dynamic_pointer_cast<Hazel::OpenGLShader>(textureShader)->UploadUniformInt("u_Texture", 0);
}


void ExampleLayer::OnUpdate(Hazel::Timestep ts) {
	// Update
	m_CameraController.OnUpdate(ts);

	// Render
	Hazel::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
	Hazel::RenderCommand::Clear();

	Hazel::Renderer::BeginScene(m_CameraController.GetCamera());

	static glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));

	//glm::vec4 redColor(0.8f, 0.2f, 0.3f, 1.0f);
	//glm::vec4 blueColor(0.2f, 0.3f, 0.8f, 1.0f);

	//Hazel::MaterialRef material = new Hazel::Material(m_FlatColorShader);
	//Hazel::MaterialInstanceRef mi = new Hazel::MaterialInstance(material);
	//mi->Set("u_Color", redColor);
	//squareMesh->SetMaterial(mi);

	std::dynamic_pointer_cast<Hazel::OpenGLShader>(m_FlatColorShader)->Bind();
	std::dynamic_pointer_cast<Hazel::OpenGLShader>(m_FlatColorShader)->UploadUniformFloat3("u_Color", m_SquareColor);

	for (size_t y = 0; y < 20; y++)
	{
		for (size_t x = 0; x < 20; x++)
		{
			glm::vec3 pos(x * 0.11f, y * 0.11f, 0.0f);
			glm::mat4 transform = glm::translate(glm::mat4(1.0f), pos) * scale;

			Hazel::Renderer::Submit(m_FlatColorShader, m_SquareVA, transform);
		}
	}

	m_Texture->Bind();
	auto textureShader = m_ShaderLibrary.Get("Texture");

	Hazel::Renderer::Submit(textureShader, m_SquareVA, glm::scale(glm::mat4(1.0f), glm::vec3(1.5f)));
	m_ChernoLogoTexture->Bind();
	Hazel::Renderer::Submit(textureShader,
		m_SquareVA,
		glm::scale(glm::mat4(1.0f), glm::vec3(1.5f)));

	// triangle
	//Hazel::Renderer::Submit(m_Shader, m_VertexArray);

	Hazel::Renderer::EndScene();
}

 void ExampleLayer::OnImGuiRender()  {
	ImGui::Begin("Settings");
	ImGui::ColorEdit3("Square Color", glm::value_ptr(m_SquareColor));
	ImGui::End();
}

