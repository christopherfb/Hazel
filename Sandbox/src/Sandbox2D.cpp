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

	Hazel::FramebufferSpecification fbSpec;
	fbSpec.Width = 1280;
	fbSpec.Height = 720;
	
	m_Framebuffer = Hazel::Framebuffer::Create(fbSpec);

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
		m_Framebuffer->Bind();

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

	m_Framebuffer->Unbind();
}

void Sandbox2D::OnImGuiRender()
{
	HZ_PROFILE_FUNCTION();
	//ImGui::Begin("Settings");
	//ImGui::ColorEdit4("Square Color", glm::value_ptr(m_SquareColor));

	//auto stats = Hazel::Renderer2D::GetStats();

	//ImGui::Text("Renderer2D Stats:");
	//ImGui::Text("Draw Calls: %d", stats.DrawCalls);
	//ImGui::Text("Quads: %d", stats.QuadCount);
	//ImGui::Text("Vertices:: %d", stats.GetTotalVertexCount());
	//ImGui::Text("Indices:: %d", stats.GetTotalIndexCount());

	//ImGui::End();






	// If you strip some features of, this demo is pretty much equivalent to calling DockSpaceOverViewport()!
   // In most cases you should be able to just call DockSpaceOverViewport() and ignore all the code below!
   // In this specific demo, we are not using DockSpaceOverViewport() because:
   // - we allow the host window to be floating/moveable instead of filling the viewport (when opt_fullscreen == false)
   // - we allow the host window to have padding (when opt_padding == true)
   // - we have a local menu bar in the host window (vs. you could use BeginMainMenuBar() + DockSpaceOverViewport() in your code!)
   // TL;DR; this demo is more complicated than what you would normally use.
   // If we removed all the options we are showcasing, this demo would become:
   //     void ShowExampleAppDockSpace()
   //     {
   //         ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());
   //     }
	bool dockingEnabled = true;
	if (dockingEnabled) {


		static bool dockspaceOpen = true;
		static bool opt_fullscreen = true;
		static bool opt_padding = false;
		static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

		// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
		// because it would be confusing to have two docking targets within each others.
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
		if (opt_fullscreen)
		{
			const ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImGui::SetNextWindowPos(viewport->WorkPos);
			ImGui::SetNextWindowSize(viewport->WorkSize);
			ImGui::SetNextWindowViewport(viewport->ID);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
			window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
			window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
		}
		else
		{
			dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
		}

		// When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background
		// and handle the pass-thru hole, so we ask Begin() to not render a background.
		if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
			window_flags |= ImGuiWindowFlags_NoBackground;

		// Important: note that we proceed even if Begin() returns false (aka window is collapsed).
		// This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
		// all active windows docked into it will lose their parent and become undocked.
		// We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
		// any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
		if (!opt_padding)
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("DockSpace Demo", &dockspaceOpen, window_flags);
		if (!opt_padding)
			ImGui::PopStyleVar();

		if (opt_fullscreen)
			ImGui::PopStyleVar(2);

		// Submit the DockSpace
		ImGuiIO& io = ImGui::GetIO();
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
		}

		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				// Disabling fullscreen would allow the window to be moved to the front of other windows,
				// which we can't undo at the moment without finer window depth/z control.

				if (ImGui::MenuItem("Exit")) { Hazel::Application::Get().Close(); }

				ImGui::EndMenu();
			}


			ImGui::EndMenuBar();
		}


		{
			ImGui::Begin("Settings");
			ImGui::ColorEdit4("Square Color", glm::value_ptr(m_SquareColor));

			auto stats = Hazel::Renderer2D::GetStats();

			ImGui::Text("Renderer2D Stats:");
			ImGui::Text("Draw Calls: %d", stats.DrawCalls);
			ImGui::Text("Quads: %d", stats.QuadCount);
			ImGui::Text("Vertices:: %d", stats.GetTotalVertexCount());
			ImGui::Text("Indices:: %d", stats.GetTotalIndexCount());

			uint32_t textureId = m_Framebuffer->GetColorAttachmentRendererID();

			ImGui::Image((void*)textureId, ImVec2{ 1280, 720 });
			ImGui::End();
		}

		ImGui::End();
	}
}

void Sandbox2D::OnEvent(Hazel::Event& e)
{
	m_CameraController.OnEvent(e);
}