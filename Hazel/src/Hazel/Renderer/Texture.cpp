#include "hzpch.h"
#include "Texture.h"
#include "Renderer.h"
#include "Platform/OpenGL/OpenGLTexture.h"

namespace Hazel {
	Ref<Texture2D> Texture2D::Create(uint32_t width, uint32_t height)
	{
		switch (Renderer::GetAPI()) {
		case RendererAPI::API::None:
			HZ_CORE_ASSERT(false, "RenderAPI::None is not supported.");
			return nullptr;
			break;

		case RendererAPI::API::OpenGL:
			return CreateRef<OpenGLTexture2D>(width, height);
			break;
		}
		HZ_CORE_ASSERT(false, "Unknown RenderAPI!");
		return nullptr;
	}


	Ref<Texture2D> Texture2D::Create(const std::string& path) {
		switch (Renderer::GetAPI()) {
		case RendererAPI::API::None:
			HZ_CORE_ASSERT(false, "RenderAPI::None is not supported.");
			return nullptr;
			break;

		case RendererAPI::API::OpenGL:
			return CreateRef<OpenGLTexture2D>(path);
			break;
		}
		HZ_CORE_ASSERT(false, "Unknown RenderAPI!");
		return nullptr;
	}

}
