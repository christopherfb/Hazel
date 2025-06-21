#include "hzpch.h"
#include "VertexArray.h"
#include "Renderer.h"
#include "Platform/OpenGL/OpenGLVertexArray.h"

namespace Hazel {	

	Ref<VertexArray> VertexArray::Create()
	{
		switch (Renderer::GetAPI()) {
		case RendererAPI::API::None:
			HZ_CORE_ASSERT(false, "RenderAPI::None is not supported.");
			return nullptr;
			break;

		case RendererAPI::API::OpenGL:
			return std::make_shared<OpenGLVertexArray>();
			break;
		}
		HZ_CORE_ASSERT(false, "Unknown RenderAPI!");
		return nullptr;	
	}
}