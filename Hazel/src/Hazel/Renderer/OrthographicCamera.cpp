#include "hzpch.h"
#include "OrthographicCamera.h"
#include <glm/gtc/matrix_transform.hpp>


namespace Hazel {

	Hazel::OrthographicCamera::OrthographicCamera(float left, float right, float bottom, float top)
		: m_ProjectionMatrix(glm::ortho(left,right,bottom, top, -1.0f, 1.0f))
		, m_ViewMatrix(1.0f)
	{
		HZ_PROFILE_FUNCTION();

		m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}

	void OrthographicCamera::SetProjection(float left, float right, float bottom, float top)
	{
		HZ_PROFILE_FUNCTION();

		m_ProjectionMatrix = glm::ortho(left, right, bottom, top, -1.0f, 1.0f);		
		m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}

	void OrthographicCamera::RecalculateViewMatrix()
	{
		HZ_PROFILE_FUNCTION();

		glm::mat4 transform =	glm::translate(glm::mat4(1.0f), m_Position) * 
								glm::rotate(glm::mat4(1.0f), glm::radians(m_Rotation), glm::vec3(0,0,1) );
		m_ViewMatrix = glm::inverse(transform);
		// note: glm uses column major order - so we need to multiply in the following order to make this work
		// properly (m_ViewProjectionMatrix var name implies View * Projection)  video 35 @ 14:05
		m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}
}
