#include "hzpch.h"

#include "Application.h"


// #include "Hazel/Log.h" // now in pch
//#include <GLFW/glfw3.h>
//#include <glad/glad.h>  //  Video:14 @ 19:38 explains this.

#include "Hazel/Renderer/Renderer.h"
#include <GLFW/glfw3.h>


namespace Hazel {

//#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)

	Application* Application::s_Instance = nullptr;


	Application::Application()
		
	{
		HZ_CORE_ASSERT(!s_Instance, "Application already exists!");
		s_Instance = this;
		
		m_Window = std::unique_ptr<Window>(Window::Create());
		m_Window->SetEventCallback(HZ_BIND_EVENT_FN(Application::OnEvent));		
		// the long version
		//m_Window->SetEventCallback(std::bind(&Application::OnEvent, this, std::placeholders::_1));
		//m_Window->SetVSync(false);

		m_ImGuiLayer = new ImGuiLayer();  // ownership of the layer gets transferred to the Layer stack (which handles deleting it).
		PushOverlay(m_ImGuiLayer);
	}

	Application::~Application()
	{
	}

	void Application::OnEvent(Event& e)
	{
		//HZ_CORE_TRACE("{0}", e.ToString());
		
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>( HZ_BIND_EVENT_FN(Application::OnWindowClose));
		
		for (auto it = m_LayerStack.end(); it != m_LayerStack.begin(); ) {
			(*--it)->OnEvent(e);
			if (e.Handled) {
				break;
			}
		}
	}

	void Application::PushLayer(Layer* layer)
	{
		m_LayerStack.PushLayer(layer);
		layer->OnAttach();
	}

	void Application::PushOverlay(Layer* layer)
	{
		m_LayerStack.PushOverlay(layer);
		layer->OnAttach();
	}

	void Application::Run()
	{		
		while (m_Running) 
		{
			float time = (float)glfwGetTime();  // replace with Platform::GetTime() to do this in a platform independent way.
			Timestep timestep = time - m_LastFrameTime;
			m_LastFrameTime = time;

			for (Layer* layer : m_LayerStack) {
				layer->OnUpdate(timestep);
			}

			m_ImGuiLayer->Begin();
			for (Layer* layer : m_LayerStack) {
				layer->OnImGuiRender();
			}
			m_ImGuiLayer->End();

			// testing input
			//auto [x, y] = Input::GetMousePosition();			
			//HZ_CORE_TRACE("mouse pos {0}, {1}", x, y);
			//HZ_CORE_TRACE("is key pressed {0}", Input::IsKeyPressed(HZ_KEY_SPACE));  // spacebar
			//HZ_CORE_TRACE("is mouse button 0 pressed {0}", Input::IsMouseButtonPressed(0) );  // left click

			m_Window->OnUpdate();
		}
	}

	bool Application::OnWindowClose(WindowCloseEvent& e)
	{
		m_Running = false;
		return true;
	}

}
