#include "hzpch.h"
#include "Sandbox2D.h"
#include "imgui/imgui.h"
#include <glm/gtc/type_ptr.hpp>



Sandbox2D::Sandbox2D()
	: Layer("Sandbox2D")
	, m_CameraController(1280.0f / 720.0f) // 16:9
{
}

void Sandbox2D::OnAttach()
{
	HZ_PROFILE_FUNCTION();
	m_CheckerboardTexture = Hazel::Texture2D::Create("assets/textures/Checkerboard.png");
	m_SpriteSheet = Hazel::Texture2D::Create("assets/game/textures/RPGpack_sheet_2X.png");

	// Init here
	m_Particle.ColorBegin = { 254 / 255.0f, 212 / 255.0f, 123 / 255.0f, 1.0f };
	m_Particle.ColorEnd = { 254 / 255.0f, 109 / 255.0f, 41 / 255.0f, 1.0f };
	m_Particle.SizeBegin = 0.5f, m_Particle.SizeVariation = 0.3f, m_Particle.SizeEnd = 0.0f;
	m_Particle.LifeTime = 5.0f;
	m_Particle.Velocity = { 0.0f, 0.0f };
	m_Particle.VelocityVariation = { 3.0f, 1.0f };
	m_Particle.Position = { 0.0f, 0.0f };
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
	Hazel::Renderer2D::ResetStats();

	{
		HZ_PROFILE_SCOPE("Render Prep");
		Hazel::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
		Hazel::RenderCommand::Clear();
	}

#if 0
	{
		static float rotation = 0.0f;
		rotation += ts * 50.0f;

		HZ_PROFILE_SCOPE("Renderer Draw");
		// the UploadUniformMat4() call buried in BeginScene() takes a lot of time! (15ms)
		Hazel::Renderer2D::BeginScene(m_CameraController.GetCamera());

		// red box
		Hazel::Renderer2D::DrawQuadDefaultParams p;
		p.rotationInRad = glm::radians( -45.0f);
		p.tint = { 0.8f, 0.2f, 0.3f, 1.0f };
		Hazel::Renderer2D::DrawQuad({ 1.0f, 0.0f }, { 0.8f, 0.8f }, p);

		p.rotationInRad = glm::radians(0.0f);
		p.tint = { 0.8f, 0.2f, 0.3f, 1.0f };
		Hazel::Renderer2D::DrawQuad({ -1.0f, 0.0f }, { 0.8f, 0.8f }, p);

		// blue box
		Hazel::Renderer2D::DrawQuadDefaultParams p5;
		p5.tint = { 0.2f, 0.2f, 0.8f, 1.0f };
		Hazel::Renderer2D::DrawQuad({ 0.5f, -0.5f }, { 0.5f, 0.75f }, p5 );

		// large checkerboard
		Hazel::Renderer2D::DrawQuadDefaultParams p2;
		p2.tilingFactor = 10.0f;	
		//p2.tint = glm::vec4(1.0, 0.8, 0.8, 1.0);
		Hazel::Renderer2D::DrawQuad({ 0.0f, 0.0f, -0.1f }, { 20.0f, 20.0f }, m_CheckerboardTexture, p2);

		// small checkerboard
		Hazel::Renderer2D::DrawQuadDefaultParams p3;
		p3.tilingFactor = 20.0f;
		p3.rotationInRad = glm::radians(rotation);
		Hazel::Renderer2D::DrawQuad({ -2.0f, 0.0f, 0.0f }, { 1.0f, 1.0f }, m_CheckerboardTexture, p3);
		

	/*	Hazel::Renderer2D::EndScene();


		Hazel::Renderer2D::BeginScene(m_CameraController.GetCamera());*/
		for (float y = -5.0; y < 5.0f; y += 0.5f) {
			for (float x = -5.0; x < 5.0f; x += 0.5f) {
				
				Hazel::Renderer2D::DrawQuadDefaultParams p;
				//p.tint = { 1.0f, 1.0f, 1.0f, 0.3f };
				p.tint = { (x + 5.0f) / 10.0f, 0.4f, (y + 5.0f) / 10.0f, 0.7f };
				Hazel::Renderer2D::DrawQuad({ x, y }, { 0.45f, 0.45f }, p);				
			}
		}
		Hazel::Renderer2D::EndScene();

	}
#endif

	if (Hazel::Input::IsMouseButtonPressed(HZ_MOUSE_BUTTON_LEFT))
	{
		auto [x, y] = Hazel::Input::GetMousePosition();
		auto width = Hazel::Application::Get().GetWindow().GetWidth();
		auto height = Hazel::Application::Get().GetWindow().GetHeight();

		auto bounds = m_CameraController.GetBounds();
		auto pos = m_CameraController.GetCamera().GetPosition();
		x = (x / width) * bounds.GetWidth() - bounds.GetWidth() * 0.5f;
		y = bounds.GetHeight() * 0.5f - (y / height) * bounds.GetHeight();
		m_Particle.Position = { x + pos.x, y + pos.y };
		const int particlesPerTick = 50;
		for (int i = 0; i < particlesPerTick; i++)
			m_ParticleSystem.Emit(m_Particle);
	}

	m_ParticleSystem.OnUpdate(ts);
	m_ParticleSystem.OnRender(m_CameraController.GetCamera());

	// GAME
	Hazel::Renderer2D::BeginScene(m_CameraController.GetCamera());
	Hazel::Renderer2D::DrawQuad({ 0.0f, 0.0f, 0.5f}, { 1.0f, 1.0f }, m_SpriteSheet);
	Hazel::Renderer2D::EndScene();


}

void Sandbox2D::OnImGuiRender()
{
	HZ_PROFILE_FUNCTION();
	ImGui::Begin("Settings");
	ImGui::ColorEdit4("Square Color", glm::value_ptr(m_SquareColor));

	auto stats = Hazel::Renderer2D::GetStats();

	ImGui::Text("Renderer2D Stats:");
	ImGui::Text("Draw Calls: %d", stats.DrawCalls);
	ImGui::Text("Quads: %d", stats.QuadCount);
	ImGui::Text("Vertices:: %d", stats.GetTotalVertexCount());
	ImGui::Text("Indices:: %d", stats.GetTotalIndexCount());


	ImGui::End();
}

void Sandbox2D::OnEvent(Hazel::Event& e)
{
	m_CameraController.OnEvent(e);
}