#include "hzpch.h"
#include "VertexArray.h"
#include "Renderer.h"
#include "Platform/OpenGL/OpenGLVertexArray.h"

namespace Hazel {	

	VertexArray* VertexArray::Create()
	{
		switch (Renderer::GetAPI()) {
		case RendererAPI::None:
			HZ_CORE_ASSERT(false, "RenderAPI::None is not supported.");
			return nullptr;
			break;

		case RendererAPI::OpenGL:
			return new OpenGLVertexArray();
			break;

		}
		HZ_CORE_ASSERT(false, "Unknown RenderAPI!");
		return nullptr;	
	}
}