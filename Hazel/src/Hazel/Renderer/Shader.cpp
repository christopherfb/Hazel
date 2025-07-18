#include "hzpch.h"
#include "Shader.h"
#include "Renderer.h"
#include "Platform/OpenGL/OpenGLShader.h"

namespace Hazel {

	/////////////////////////////////////////
	// SHADER 
	/////////////////////////////////////////
	
	Ref<Shader> Shader::Create(const std::string& filepath)
	{
		switch (Renderer::GetAPI()) {
			case RendererAPI::API::None:
				HZ_CORE_ASSERT(false, "RenderAPI::None is not supported.");
				return nullptr;
				break;

			case RendererAPI::API::OpenGL:
				return std::make_shared<OpenGLShader>(filepath);
				break;
		}
		HZ_CORE_ASSERT(false, "Unknown RenderAPI!");
		return nullptr;
	}


	Ref<Shader> Shader::Create(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc)
	{
		switch (Renderer::GetAPI()) {
		case RendererAPI::API::None:
			HZ_CORE_ASSERT(false, "RenderAPI::None is not supported.");
			return nullptr;
			break;

		case RendererAPI::API::OpenGL:
			return std::make_shared<OpenGLShader>(name, vertexSrc, fragmentSrc);
			break;
		}
		HZ_CORE_ASSERT(false, "Unknown RenderAPI!");
		return nullptr;
	}


	/////////////////////////////////////////
	// SHADER LIBRARY 
	/////////////////////////////////////////

	void ShaderLibrary::Add(const std::string& name, const Ref<Shader>& shader)
	{
		HZ_CORE_ASSERT(!Exists(name), "Shader already exists!");
		m_Shaders[name] = shader;
	}

	void ShaderLibrary::Add(const Ref<Shader>& shader)
	{
		auto& name = shader->GetName();
		Add(name, shader);
	}


	Ref<Shader> ShaderLibrary::Load(const std::string& filepath)
	{
		auto shader = Shader::Create(filepath);
		Add(shader);
		return shader;
	}

	Ref<Shader> ShaderLibrary::Load(const std::string& name, const std::string& filepath)
	{
		auto shader = Shader::Create(filepath);
		Add(name, shader);
		return shader;
	}
	Ref<Shader> ShaderLibrary::Get(const std::string& name)
	{
		HZ_CORE_ASSERT(Exists(name), "Shader not found!");
		return m_Shaders[name];
	}
}