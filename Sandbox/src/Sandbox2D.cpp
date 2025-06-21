#include "hzpch.h"
#include "Sandbox2D.h"
#include "imgui/imgui.h"
#include <glm/gtc/type_ptr.hpp>
//#include "Platform/OpenGL/OpenGLShader.h"


Sandbox2D::Sandbox2D() 
	: Layer("Sandbox2D")
	, m_CameraController(1280.0f / 720.0f)  // 16:9
{
}

void Sandbox2D::OnAttach()
{
	//// DRAW A SQUARE ////////////////////////////////////////////
	//m_SquareVA = Hazel::VertexArray::Create();

	//float squareVertices[3 * 4] = {
	//	-0.5f, -0.5f, 0.0f,
	//	0.5f, -0.5f, 0.0f ,
	//	0.5f,  0.5f, 0.0f , 
	//	-0.5f,  0.5f, 0.0f
	//};
	//Hazel::Ref<Hazel::VertexBuffer> squareVB;
	//squareVB.reset(Hazel::VertexBuffer::Create(squareVertices, sizeof(squareVertices)));

	//Hazel::BufferLayout squareVBLayout = {
	//	{Hazel::ShaderDataType::Float3, "a_Position" },

	//};
	//squareVB->SetLayout(squareVBLayout);
	//m_SquareVA->AddVertexBuffer(squareVB);

	//uint32_t squareIndices[6] = { 0,1,2,2,3,0 };

	//Hazel::Ref<Hazel::IndexBuffer> squareIB;
	//squareIB.reset(Hazel::IndexBuffer::Create(squareIndices, sizeof(squareIndices) / sizeof(uint32_t)));
	//m_SquareVA->SetIndexBuffer(squareIB);

	//m_FlatColorShader = Hazel::Shader::Create("assets/shaders/FlatColor.glsl");
}


void Sandbox2D::OnDetach()
{
}

void Sandbox2D::OnUpdate(Hazel::Timestep ts)
{
	m_CameraController.OnUpdate(ts);

	//// Render
	Hazel::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
	Hazel::RenderCommand::Clear();

	Hazel::Renderer2D::BeginScene(m_CameraController.GetCamera());

	Hazel::Renderer2D::DrawQuad({ -1.0f, 0.0f }, { 0.8f, 0.8f }, { 0.8f, 0.2f, 0.3f, 1.0f });
	Hazel::Renderer2D::DrawQuad({ 0.5f, -0.5f }, { 0.5f, 0.75f }, { 0.2f, 0.2f, 0.8f, 1.0f });
	
	Hazel::Renderer2D::EndScene();
}

void Sandbox2D::OnImGuiRender()
{
	ImGui::Begin("Settings");
	ImGui::ColorEdit4("Square Color", glm::value_ptr(m_SquareColor));
	ImGui::End();
}

void Sandbox2D::OnEvent(Hazel::Event& e)
{
	m_CameraController.OnEvent(e);
}