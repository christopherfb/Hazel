#include "hzpch.h"
#include "EditorLayer.h"
#include "imgui/imgui.h"
#include <glm/gtc/type_ptr.hpp>

namespace Hazel {

	


	EditorLayer::EditorLayer()
		: Layer("EditorLayer")
		, m_CameraController(1280.0f / 720.0f) // 16:9
	{
	}

	void EditorLayer::OnAttach()
	{
		HZ_PROFILE_FUNCTION();
		m_CheckerboardTexture = Texture2D::Create("assets/textures/Checkerboard.png");

		FramebufferSpecification fbSpec;
		fbSpec.Width = 1280;
		fbSpec.Height = 720;

		m_Framebuffer = Framebuffer::Create(fbSpec);

		m_ActiveScene = CreateRef<Scene>();

		m_SquareEntity = m_ActiveScene->CreateEntity("Green Square");
		m_SquareEntity.AddComponent<SpriteRendererComponent>( glm::vec4{ 0.0f, 1.0f, 0.0f, 1.0f });

		m_SquareEntity2 = m_ActiveScene->CreateEntity("Red Square");
		m_SquareEntity2.GetComponent<TransformComponent>().Translation.x = 2.0f;
		m_SquareEntity2.AddComponent<SpriteRendererComponent>( glm::vec4{ 1.0f, 0.0f, 0.0f, 1.0f });

		m_CameraEntity = m_ActiveScene->CreateEntity("Camera A");
		m_CameraEntity.AddComponent<CameraComponent>();

		//m_SecondCamera = m_ActiveScene->CreateEntity("Camera B");
		//auto& cc = m_SecondCamera.AddComponent<CameraComponent>();
		//cc.Primary = false;

		class CameraController : public ScriptableEntity
		{
		public:
			void OnCreate() {
				auto& translation = GetComponent<TransformComponent>().Translation;
				translation.x = rand() % 10 - 5.0f;
			}


			void OnDestroy(){}
			void OnUpdate(Timestep ts) {
				float speed = 5.0f;
				//std::cout << "OnUpdate() Timestep:" << ts << "\n";
				auto& translation = GetComponent<TransformComponent>().Translation;
				if (Input::IsKeyPressed(KeyCode::A)) {
					translation.x -= speed * ts;;
				}
				if (Input::IsKeyPressed(KeyCode::D)) {
					translation.x += speed * ts;;
				}
				if (Input::IsKeyPressed(KeyCode::W)) {
					translation.y += speed * ts;;
				}
				if (Input::IsKeyPressed(KeyCode::S)) {
					translation.y -= speed * ts;;
				}

			}
		};

		m_CameraEntity.AddComponent<NativeScriptComponent>().Bind<CameraController>();
		//m_SecondCamera.AddComponent<NativeScriptComponent>().Bind<CameraController>();


		m_SceneHierarchyPanel.SetContext(m_ActiveScene);
	}


	void EditorLayer::OnDetach()
	{
		HZ_PROFILE_FUNCTION();
	}

	void EditorLayer::OnUpdate(Timestep ts)
	{
		HZ_PROFILE_FUNCTION();

		// Resize
		FramebufferSpecification spec = m_Framebuffer->GetSpecification();
		//HZ_INFO("vp size x: {0} spec width:{1}", m_ViewportSize.x, spec.Width);
		if (
			m_ViewportSize.x > 0.0f && m_ViewportSize.y > 0.0f && // zero sized framebuffer is invalid
			(spec.Width != m_ViewportSize.x || spec.Height != m_ViewportSize.y))
		{
			//HZ_CORE_WARN("EditorLayer vp size change");
			m_Framebuffer->Resize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
			m_CameraController.OnResize(m_ViewportSize.x, m_ViewportSize.y);

			m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
			//m_EditorCamera.SetViewportSize(m_ViewportSize.x, m_ViewportSize.y);
		}

		// Update
		if (m_ViewportFocused) {
			m_CameraController.OnUpdate(ts);
		}

		//// Render
		Renderer2D::ResetStats();
		m_Framebuffer->Bind();
		RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
		RenderCommand::Clear();

		// Update Scene
		m_ActiveScene->OnUpdate(ts);

		m_Framebuffer->Unbind();
	}

	void EditorLayer::OnImGuiRender()
	{
		HZ_PROFILE_FUNCTION();
		//ImGui::Begin("Settings");
		//ImGui::ColorEdit4("Square Color", glm::value_ptr(m_SquareColor));

		//auto stats = Renderer2D::GetStats();

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
		ImGuiStyle& style = ImGui::GetStyle();
		float minWinSizeX = style.WindowMinSize.x;
		style.WindowMinSize.x = 370.0f;

		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
		}

		style.WindowMinSize.x = minWinSizeX;

		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				// Disabling fullscreen would allow the window to be moved to the front of other windows,
				// which we can't undo at the moment without finer window depth/z control.

				if (ImGui::MenuItem("Exit")) { Application::Get().Close(); }

				ImGui::EndMenu();
			}


			ImGui::EndMenuBar();
		}

		m_SceneHierarchyPanel.OnImGuiRender();

		{
			ImGui::Begin("Stats");

			auto stats = Renderer2D::GetStats();
		
			ImGui::Text("Renderer2D Stats:");
			ImGui::Text("Draw Calls: %d", stats.DrawCalls);
			ImGui::Text("Quads: %d", stats.QuadCount);
			ImGui::Text("Vertices:: %d", stats.GetTotalVertexCount());
			ImGui::Text("Indices:: %d", stats.GetTotalIndexCount());

			ImGui::End();
		}

		{
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
			ImGui::Begin("Viewport");
			//HZ_CORE_WARN("Focused: {0}", ImGui::IsWindowFocused());
			//HZ_CORE_WARN("Hovered: {0}", ImGui::IsWindowHovered());

			m_ViewportFocused = ImGui::IsWindowFocused();
			m_ViewportHovered = ImGui::IsWindowHovered();
			Application::Get().GetImGuiLayer()->BlockEvents(!m_ViewportFocused || !m_ViewportHovered);
		
			ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
			m_ViewportSize = { viewportPanelSize.x, viewportPanelSize.y };

			uint32_t textureId = m_Framebuffer->GetColorAttachmentRendererID();
			ImGui::Image((void*)textureId, viewportPanelSize, ImVec2{ 0,1 }, ImVec2{ 1,0 });
			ImGui::End();
			ImGui::PopStyleVar();
		}

		ImGui::End();
	}

	void EditorLayer::OnEvent(Event& e)
	{
		m_CameraController.OnEvent(e);
	}

}
