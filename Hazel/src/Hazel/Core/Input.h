#pragma once

#include "Hazel/Core/Core.h"

namespace Hazel {

	// This is an interface for querying the state of input (mouse button down? key pressed? etc).  
	// The implementation for the interface is platform specific (windows uses GLFW functions) 
	// So the Input class is subclassed & implemented in the WindowsInput class.

	class HAZEL_API Input {
	public:
		virtual ~Input() = default;

		static bool IsKeyPressed(int keycode) { return s_Instance->IsKeyPressedImpl(keycode); }
		static bool IsMouseButtonPressed(int button) { return s_Instance->IsMouseButtonPressedImpl(button); }
		static std::pair<float,float> GetMousePosition() { return s_Instance->GetMousePositionImpl(); }
		static bool GetMouseX() { return s_Instance->GetMouseXImpl(); }
		static bool GetMouseY() { return s_Instance->GetMouseYImpl(); }

	protected:
		virtual bool IsKeyPressedImpl(int keycode) = 0;
		virtual bool IsMouseButtonPressedImpl(int button) = 0;
		virtual std::pair<float,float> GetMousePositionImpl() = 0;
		virtual float GetMouseXImpl() = 0;
		virtual float GetMouseYImpl() = 0;

	private:
		static Input* s_Instance;

	};
}