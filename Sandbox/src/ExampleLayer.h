#pragma once

#include "Hazel/Core/Layer.h"
#include "Hazel/Renderer/Shader.h"
#include "Hazel/Renderer/VertexArray.h"
#include "Hazel/Renderer/Texture.h"
#include "Hazel/Renderer/OrthographicCameraController.h"


class ExampleLayer : public Hazel::Layer
{
public:
	ExampleLayer();

	void OnUpdate(Hazel::Timestep ts) override;
	void OnEvent(Hazel::Event& e) override 	{ m_CameraController.OnEvent(e); }
	virtual void OnImGuiRender() override;

private:
	Hazel::ShaderLibrary m_ShaderLibrary;

	Hazel::Ref<Hazel::Shader> m_Shader;

	Hazel::Ref<Hazel::VertexArray> m_VertexArray;
	Hazel::Ref<Hazel::VertexArray> m_SquareVA;

	Hazel::Ref<Hazel::Shader> m_FlatColorShader;
	//Hazel::Ref<Hazel::Shader> m_TextureShader;

	Hazel::Ref<Hazel::Texture2D> m_Texture;
	Hazel::Ref<Hazel::Texture2D> m_ChernoLogoTexture;

	Hazel::OrthogrphicCameraController m_CameraController;

	glm::vec3 m_SquareColor = { 0.2f, 0.3f, 0.8f };
};


