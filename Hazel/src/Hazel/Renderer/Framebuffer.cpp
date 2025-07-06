

#include "hzpch.h"
#include "Framebuffer.h"
#include "Hazel/Renderer/Renderer.h"
#include "Platform/OpenGL/OpenGLFramebuffer.h"

namespace Hazel {

	Ref<Framebuffer> Framebuffer::Create(const FramebufferSpecification& spec) {

		
			switch (Renderer::GetAPI()) {
			case RendererAPI::API::None:
				HZ_CORE_ASSERT(false, "RenderAPI::None is not supported.");
				return nullptr;
				break;

			case RendererAPI::API::OpenGL:
				return std::make_shared<OpenGLFramebuffer>(spec);
				break;
			}
			HZ_CORE_ASSERT(false, "Unknown RenderAPI!");
			return nullptr;
		
	}
}