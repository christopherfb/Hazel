#pragma once

#include "Hazel/Core/Core.h"
#include "Hazel/Core/KeyCodes.h"

namespace Hazel {

	// This is an interface for querying the state of input (mouse button down? key pressed? etc).  
	// The implementation for the interface is platform specific (windows uses GLFW functions) 
	// So the Input class is subclassed & implemented in the WindowsInput class.

	class Input {
	public:
		
		static bool IsKeyPressed(KeyCode keycode);
		static bool IsMouseButtonPressed(int button);
		static std::pair<float, float> GetMousePosition();
		static float GetMouseX();
		static float GetMouseY();
	};
}