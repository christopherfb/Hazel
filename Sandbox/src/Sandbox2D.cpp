#include "hzpch.h"
#include "Sandbox2D.h"
#include "imgui/imgui.h"
#include <glm/gtc/type_ptr.hpp>



Sandbox2D::Sandbox2D() 
	: Layer("Sandbox2D")
	, m_CameraController(1280.0f / 720.0f)  // 16:9
{
}

void Sandbox2D::OnAttach()
{
	HZ_PROFILE_FUNCTION();
	m_CheckerboardTexture = Hazel::Texture2D::Create("assets/textures/Checkerboard.png");
}


void Sandbox2D::OnDetach()
{
	HZ_PROFILE_FUNCTION();
}

void Sandbox2D::OnUpdate(Hazel::Timestep ts)
{
	HZ_PROFILE_FUNCTION();


	m_CameraController.OnUpdate(ts);


	//// Render
	{
		HZ_PROFILE_SCOPE("Render Prep");
		Hazel::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
		Hazel::RenderCommand::Clear();
	}

	{
		HZ_PROFILE_SCOPE("Renderer Draw");
		// the UploadUniformMat4() call buried in BeginScene() takes a lot of time! (15ms)
		Hazel::Renderer2D::BeginScene(m_CameraController.GetCamera());

		Hazel::DrawQuadDefaultParams p;
		p.rotation = glm::radians(-45.0f);
		p.tint = { 0.8f, 0.2f, 0.3f, 1.0f };
		Hazel::Renderer2D::DrawQuad({ -1.0f, 0.0f }, { 0.8f, 0.8f }, { 0.8f, 0.2f, 0.3f, 1.0f }, p);


		Hazel::Renderer2D::DrawQuad({ 0.5f, -0.5f }, { 0.5f, 0.75f }, { 0.2f, 0.2f, 0.8f, 1.0f });

		Hazel::DrawQuadDefaultParams p2;
		p2.rotation = glm::radians(-45.0f);
		p2.tilingFactor = 10.0f;
		p2.tint = glm::vec4(1.0, 0.8, 0.8, 1.0);
		Hazel::Renderer2D::DrawQuad({ 0.0f, 0.0f, -0.1f }, { 10.0f, 10.0f }, m_CheckerboardTexture, p2);
		

		Hazel::Renderer2D::EndScene();
	}
}

void Sandbox2D::OnImGuiRender()
{
	HZ_PROFILE_FUNCTION();
	ImGui::Begin("Settings");
	ImGui::ColorEdit4("Square Color", glm::value_ptr(m_SquareColor));
	ImGui::End();
}

void Sandbox2D::OnEvent(Hazel::Event& e)
{
	m_CameraController.OnEvent(e);
}