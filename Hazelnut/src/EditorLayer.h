#pragma once

#include <Hazel.h>


namespace Hazel {

	class EditorLayer : public Hazel::Layer
	{
	public:
		EditorLayer();
		virtual ~EditorLayer() = default;

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnUpdate(Hazel::Timestep ts) override;
		virtual void OnImGuiRender() override;
		virtual void OnEvent(Hazel::Event& e) override;

	private:
		OrthogrphicCameraController m_CameraController;

		Ref<Hazel::VertexArray> m_SquareVA;
		Ref<Hazel::Shader> m_FlatColorShader;
		Hazel::Ref<Hazel::Texture2D> m_CheckerboardTexture;

		bool m_ViewportFocused = false;
		bool m_ViewportHovered = false;

		Ref<Hazel::Framebuffer> m_Framebuffer;

		Ref<Scene> m_ActiveScene;
		Entity m_SquareEntity;


		glm::vec4 m_SquareColor = { 0.2f, 0.3f, 0.8f, 1.0f };

		std::unordered_map<char, Ref<Hazel::SubTexture2D>> s_TextureMap;
		glm::vec2 m_ViewportSize = { 0, 0 };

	};
}


