#pragma once
#include "Hazel/Renderer/OrthographicCamera.h"
#include "Hazel/Core/Timestep.h"

#include "Hazel/Events/ApplicationEvent.h"
#include "Hazel/Events/MouseEvent.h"

namespace Hazel {

	class OrthogrphicCameraController {
	public:
		OrthogrphicCameraController(float aspectRatio, bool rotation = false);  // 2 units of vertical space

		void OnUpdate(Timestep ts);
		void OnEvent(Event& e);

		OrthographicCamera& GetCamera() { return m_Camera;	}
		const OrthographicCamera& GetCamera() const { return m_Camera;	}

		float GetZoomLevel() const { return m_ZoomLevel; }
		void SetZoomLevel(float level) { m_ZoomLevel = level; }

	private:
		bool OnMouseScrolled(MouseScrolledEvent& e);
		bool OnWindowResized(WindowResizeEvent& e);

	private:

		float m_AspectRatio;
		float m_ZoomLevel = 1.0f;
		OrthographicCamera m_Camera;
		bool m_Rotation;


		glm::vec3 m_CameraPosition{ 0.0f, 0.0f, 0.0f };
		float m_CameraTranslationSpeed = 5.0f;

		float m_CameraRotation = 0.0f;
		float m_CameraRotationSpeed = 180.0f;

	};

}