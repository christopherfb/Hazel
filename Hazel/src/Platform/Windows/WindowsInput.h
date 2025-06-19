#pragma once

#include "Hazel/Core/Input.h"


namespace Hazel {

	// The platform specific implementation of the Input class which queries the state of input devices (mouse button down?, key pressed?, etc.)

	class HAZEL_API WindowsInput : public Input 
	{
	protected:
		virtual bool IsKeyPressedImpl(int keycode) override;
		virtual bool IsMouseButtonPressedImpl(int button) override;
		virtual std::pair<float, float> GetMousePositionImpl() override;
		virtual float GetMouseXImpl() override;
		virtual float GetMouseYImpl() override;
	};
}
