#include "hzpch.h"
#include "Sandbox2D.h"
#include "imgui/imgui.h"
#include <glm/gtc/type_ptr.hpp>


static const char* s_MapTiles =
"WWWWWWWWWWWWWWWWWWWWWWWW"
"WWWWWWWWWWWWWWWWWWWWWWWW"
"WWDDWWWWWWWWWWWWWWWWWWWW"
"WWWWWWWWWWWWWWWWWWWWWWWW"
"WWDDWWWWWWWWWWWWWWWWWWWW"
"WWWWWWWWWWWWWWWWWWWWWWWW"
"WWWWWWWWWWWWWWWWWWWWWWWW"
"WWWWWWWWWWWWWWWWWWWWWWWW"
"WWWWWWWWWWWWWWWWWWWWWWWW"
"WWWWWWWWWWWWWWWWWWWWWWWW"
"WWWWWWWWWWWWWWWWWWWWWWWW"
"WWWWWWWWWWWWWWWWWWWWWWWW"
"WWWWWWWWWWWWWWWWWWWWWWCW"
"WWWWWWWWWWWWWWWWWWWWWWWD";


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
	m_TextureStairs = Hazel::SubTexture2D::CreateFromCoords(m_SpriteSheet, { 7, 6 }, { 128,128 });
	m_TextureTree = Hazel::SubTexture2D::CreateFromCoords(m_SpriteSheet, { 2, 1 }, { 128,128 }, { 1,2 });
	

	m_MapWidth = 24;
	m_MapHeight = strlen(s_MapTiles) / m_MapWidth;

	s_TextureMap['D'] = Hazel::SubTexture2D::CreateFromCoords(m_SpriteSheet, {6, 11}, {128,128});
	s_TextureMap['W'] = Hazel::SubTexture2D::CreateFromCoords(m_SpriteSheet, {11, 11}, {128,128});

	// Init here
	m_Particle.ColorBegin = { 254 / 255.0f, 212 / 255.0f, 123 / 255.0f, 1.0f };
	m_Particle.ColorEnd = { 254 / 255.0f, 109 / 255.0f, 41 / 255.0f, 1.0f };
	m_Particle.SizeBegin = 0.5f, m_Particle.SizeVariation = 0.3f, m_Particle.SizeEnd = 0.0f;
	m_Particle.LifeTime = 5.0f;
	m_Particle.Velocity = { 0.0f, 0.0f };
	m_Particle.VelocityVariation = { 3.0f, 1.0f };
	m_Particle.Position = { 0.0f, 0.0f };

	//m_CameraController.SetZoomLevel(5.0f);

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

#if 1
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
		

		// draw the multicolor grid
		for (float y = -5.0; y < 5.0f; y += 0.5f) {
			for (float x = -5.0; x < 5.0f; x += 0.5f) {
				
				Hazel::Renderer2D::DrawQuadDefaultParams p;
				//p.tint = { 1.0f, 1.0f, 1.0f, 0.3f };
				p.tint = { (x + 5.0f) / 10.0f, 0.4f, (y + 5.0f) / 10.0f, 0.7f };
				Hazel::Renderer2D::DrawQuad({ x, y }, { 0.45f, 0.45f }, p);				
			}
		}
		Hazel::Renderer2D::EndScene();
		//m_Framebuffer->Unbind();

	}
#endif

	//if (Hazel::Input::IsMouseButtonPressed(HZ_MOUSE_BUTTON_LEFT))
	//{
	//	auto [x, y] = Hazel::Input::GetMousePosition();
	//	auto width = Hazel::Application::Get().GetWindow().GetWidth();
	//	auto height = Hazel::Application::Get().GetWindow().GetHeight();

	//	auto bounds = m_CameraController.GetBounds();
	//	auto pos = m_CameraController.GetCamera().GetPosition();
	//	x = (x / width) * bounds.GetWidth() - bounds.GetWidth() * 0.5f;
	//	y = bounds.GetHeight() * 0.5f - (y / height) * bounds.GetHeight();
	//	m_Particle.Position = { x + pos.x, y + pos.y };
	//	const int particlesPerTick = 50;
	//	for (int i = 0; i < particlesPerTick; i++)
	//		m_ParticleSystem.Emit(m_Particle);
	//}

	//m_ParticleSystem.OnUpdate(ts);
	//m_ParticleSystem.OnRender(m_CameraController.GetCamera());

	//// GAME
	//Hazel::Renderer2D::BeginScene(m_CameraController.GetCamera());
	////Hazel::Renderer2D::DrawQuad({ 0.0f, 0.0f, 0.5f}, { 1.0f, 1.0f }, m_SpriteSheet);
	////Hazel::Renderer2D::DrawQuad({ 0.0f, 0.0f, 0.5f}, { 1.0f, 1.0f }, m_TextureStairs);
	////Hazel::Renderer2D::DrawQuad({ 1.0f, 0.0f, 0.5f}, { 1.0f, 1.0f }, m_TextureBarrel);
	////Hazel::Renderer2D::DrawQuad({ -1.0f, 0.0f, 0.5f}, { 1.0f, 2.0f }, m_TextureTree);

	//for (uint32_t y = 0; y < m_MapHeight; y++) {
	//	for (uint32_t x = 0; x < m_MapWidth; x++) {
	//		char tileType = s_MapTiles[x + (y * m_MapWidth)];
	//		Hazel::Ref<Hazel::SubTexture2D> texture;

	//		if (s_TextureMap.find(tileType) != s_TextureMap.end()) {
	//			texture = s_TextureMap[tileType];
	//		}
	//		else {
	//			texture = m_TextureStairs;
	//		}
	//		//Hazel::Renderer2D::DrawQuad({ x - m_MapWidth/2, y-m_MapHeight/2, 0.5f}, { 1.0f, 1.0f }, texture );
	//		Hazel::Renderer2D::DrawQuad({ x - (m_MapWidth / 2.0f) ,(y * -1.0f) + (m_MapHeight / 2.0f) , 0.5f }, { 1.0f, 1.0f }, texture);
	//	}
	//}
	//Hazel::Renderer2D::EndScene();


}

void Sandbox2D::OnImGuiRender()
{
	HZ_PROFILE_FUNCTION();


	ImGui::Begin("Settings");


	auto stats = Hazel::Renderer2D::GetStats();

	ImGui::Text("Renderer2D Stats:");
	ImGui::Text("Draw Calls: %d", stats.DrawCalls);
	ImGui::Text("Quads: %d", stats.QuadCount);
	ImGui::Text("Vertices:: %d", stats.GetTotalVertexCount());
	ImGui::Text("Indices:: %d", stats.GetTotalIndexCount());

	ImGui::ColorEdit4("Square Color", glm::value_ptr(m_SquareColor));

	//uint32_t textureId = m_CheckerboardTexture; //m_Framebuffer->GetColorAttachmentRendererID();

	//ImGui::Image((void*)textureId, ImVec2{ 1280, 720 });
	ImGui::End();
	
}

void Sandbox2D::OnEvent(Hazel::Event& e)
{
	m_CameraController.OnEvent(e);
}