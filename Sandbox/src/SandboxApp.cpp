
#include <Hazel.h>

// --- Entry Point ----------------------
#include "Hazel/Core/EntryPoint.h"
// --------------------------------------


//#include "Platform/OpenGL/OpenGLShader.h"

//#include "imgui/imgui.h"

// sample code from: https://glm.g-truc.net/0.9.9/index.html just to show that the camera works.
//#include <glm/vec3.hpp> // glm::vec3
//#include <glm/vec4.hpp> // glm::vec4
//#include <glm/mat4x4.hpp> // glm::mat4
//#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective

//#include "Hazel/Renderer/Shader.h" 
//#include <glm/gtc/type_ptr.hpp>

#include "ExampleLayer.h"
#include "Sandbox2D.h"



//glm::mat4 camera(float Translate, glm::vec2 const& Rotate)
//{
//	glm::mat4 Projection = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 100.f);
//	glm::mat4 View = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -Translate));
//	View = glm::rotate(View, Rotate.y, glm::vec3(-1.0f, 0.0f, 0.0f));
//	View = glm::rotate(View, Rotate.x, glm::vec3(0.0f, 1.0f, 0.0f));
//	glm::mat4 Model = glm::scale(glm::mat4(1.0f), glm::vec3(0.5f));
//	return Projection * View * Model;
//}



class Sandbox : public Hazel::Application
{
public:
	Sandbox() 
	{
		//PushLayer(new ExampleLayer());
		PushLayer(new Sandbox2D());

	}

	~Sandbox() {
	}
};


Hazel::Application* Hazel::CreateApplication() {
	return new Sandbox();
}
