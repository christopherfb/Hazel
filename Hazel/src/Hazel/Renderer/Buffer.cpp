#include "hzpch.h"
#include "Buffer.h"
#include "Renderer.h"
#include "Platform/OpenGL/OpenGLBuffer.h"

namespace Hazel {


	Ref<VertexBuffer> VertexBuffer::Create(uint32_t size)
	{
		switch (Renderer::GetAPI()) {
		case RendererAPI::API::None:
			HZ_CORE_ASSERT(false, "RenderAPI::None is not supported.");
			return nullptr;
			break;

		case RendererAPI::API::OpenGL:
			return CreateRef<OpenGLVertexBuffer>(size);
			break;

		}
		HZ_CORE_ASSERT(false, "Unknown RenderAPI!");
		return nullptr;
	}


	Ref<VertexBuffer> VertexBuffer::Create(float* vertices, uint32_t size)
	{
		switch (Renderer::GetAPI()) {
		case RendererAPI::API::None: 
			HZ_CORE_ASSERT(false, "RenderAPI::None is not supported.");
			return nullptr;
			break;

		case RendererAPI::API::OpenGL:
			return CreateRef<OpenGLVertexBuffer>(vertices, size);
			break;

		}
		HZ_CORE_ASSERT(false, "Unknown RenderAPI!");
		return nullptr;
	}

	Ref<IndexBuffer> IndexBuffer::Create(uint32_t* indices, uint32_t count)
	{
		switch (Renderer::GetAPI()) {
		case RendererAPI::API::None:
			HZ_CORE_ASSERT(false, "RenderAPI::None is not supported.");
			return nullptr;
			break;

		case RendererAPI::API::OpenGL:
			return CreateRef<OpenGLIndexBuffer>(indices, count);
			break;

		}
		HZ_CORE_ASSERT(false, "Unknown RenderAPI!");
		return nullptr;
	}
}