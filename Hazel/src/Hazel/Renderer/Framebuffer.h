#pragma once

#include "Hazel/Core/Core.h"

namespace Hazel {


	struct FramebufferSpecification {
		uint32_t Width;
		uint32_t Height;
		uint32_t Samples = 1;

		// when this is true, same as glBindFramebuffer(0) - which essentially says "unbind my frame buffers, I want to render to the screen"
		// allows you to create a frame buffer which doesn't "exist" vid 71  @ 18:00
		bool SwapChainTarget = false;  
	};


	class Framebuffer {
	public:
		static Ref<Framebuffer> Create(const FramebufferSpecification& spec);

		// This will likely be removed in the future because it doesn't make sense for a Framebuffer to be bound.
		virtual void Bind() = 0;
		virtual void Unbind() = 0;

		virtual void Resize(uint32_t width, uint32_t height) = 0;

		virtual uint32_t GetColorAttachmentRendererID() const = 0;
		virtual const FramebufferSpecification& GetSpecification() const = 0;

	};
}