#pragma once
#include "Hazel/Renderer/Camera.h"

namespace Hazel {
	class SceneCamera : public Camera {
	public:
		enum class ProjectionType { Perspective = 0, Orthographic};

	public:
		SceneCamera();
		//SceneCamera(const glm::mat4& projection)
		//	: Camera(projection) {
		//}
		virtual ~SceneCamera() = default;

		void SetOrthographic(float size, float nearClip, float farClip);
		void SetPerspective(float fov, float nearClip, float farClip);

		void SetViewportSize(uint32_t width, uint32_t height);

		ProjectionType GetProjectionType() const { return m_ProjectionType; }
		void SetProjectionType(ProjectionType projType) { m_ProjectionType = projType; RecalculateProjection(); }

		// Orthographic
		float GetOrthographicSize() const { return m_OrthographicSize; }
		void  SetOrthographicSize(float size) { m_OrthographicSize = size; RecalculateProjection(); }

		float GetOrthographicNearClip() const { return m_OrthographicNear; }
		void SetOrthographicNearClip(float value)  { m_OrthographicNear = value; RecalculateProjection(); }

		float GetOrthographicFarClip() const { return m_OrthographicFar; }
		void SetOrthographicFarClip(float value) { m_OrthographicFar = value; RecalculateProjection();	}


		// Perspective
		float GetPerspectiveVerticalFOV() const { return m_PerspectiveFOV; }
		void  SetPerspectiveVerticalFOV(float value) { m_PerspectiveFOV = value; RecalculateProjection(); }

		float GetPerspectiveNearClip() const { return m_PerspectiveNear; }
		void SetPerspectiveNearClip(float value) { m_PerspectiveNear = value; RecalculateProjection(); }

		float GetPerspectiveFarClip() const { return m_PerspectiveFar; }
		void SetPerspectiveFarClip(float value) { m_PerspectiveFar = value; RecalculateProjection(); }

	private:
		void RecalculateProjection();

	private:
		ProjectionType m_ProjectionType = ProjectionType::Orthographic;

		float m_OrthographicSize = 10.0f;
		float m_OrthographicNear = -1.0f;
		float m_OrthographicFar = 1.0f;
	
		float m_PerspectiveFOV = glm::radians(45.0f);
		float m_PerspectiveNear = 0.01f;
		float m_PerspectiveFar = 1000.0f;

		float m_AspectRatio = 1.0f;
	};
}