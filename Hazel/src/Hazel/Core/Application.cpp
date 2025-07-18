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


	Application::Application(const std::string& name)
	{
		HZ_PROFILE_FUNCTION();

		HZ_CORE_ASSERT(!s_Instance, "Application already exists!");
		s_Instance = this;
		
		//m_Window = std::unique_ptr<Window>(Window::Create());
		m_Window = Window::Create(WindowProps(name));
		m_Window->SetEventCallback(HZ_BIND_EVENT_FN(Application::OnEvent));		

		Renderer::Init();

		m_ImGuiLayer = new ImGuiLayer();  // ownership of the layer gets transferred to the Layer stack (which handles deleting it).
		PushOverlay(m_ImGuiLayer);
	}

	Application::~Application()
	{
		HZ_PROFILE_FUNCTION();

		Renderer::Shutdown();
	}

	void Application::OnEvent(Event& e)
	{
		HZ_PROFILE_FUNCTION();
		//HZ_CORE_TRACE("{0}", e.ToString());
		
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>( HZ_BIND_EVENT_FN(Application::OnWindowClose));
		dispatcher.Dispatch<WindowResizeEvent>( HZ_BIND_EVENT_FN(Application::OnWindowResize));
		
		for (auto it = m_LayerStack.end(); it != m_LayerStack.begin(); ) {
			(*--it)->OnEvent(e);
			if (e.Handled) {
				break;
			}
		}
	}

	void Application::PushLayer(Layer* layer)
	{
		HZ_PROFILE_FUNCTION();
		m_LayerStack.PushLayer(layer);
		layer->OnAttach();
	}

	void Application::PushOverlay(Layer* layer)
	{
		HZ_PROFILE_FUNCTION();
		m_LayerStack.PushOverlay(layer);
		layer->OnAttach();
	}

	void Application::Close()
	{
		m_Running = false;
	}

	void Application::Run()
	{		
		HZ_PROFILE_FUNCTION();
		while (m_Running) 
		{
			HZ_PROFILE_SCOPE("RunLoop");
			float time = (float)glfwGetTime();  // replace with Platform::GetTime() to do this in a platform independent way.
			Timestep timestep = time - m_LastFrameTime;
			m_LastFrameTime = time;

			if (!m_Minimized) {
				{
					HZ_PROFILE_SCOPE("LayerStack OnUpdates");
					for (Layer* layer : m_LayerStack) {
						layer->OnUpdate(timestep);
					}
				}

				m_ImGuiLayer->Begin();
				{
					HZ_PROFILE_SCOPE("LayerStack OnImGuiRender");
					for (Layer* layer : m_LayerStack) {
						layer->OnImGuiRender();
					}
				}
				m_ImGuiLayer->End();
			}

			m_Window->OnUpdate();
		}
	}

	bool Application::OnWindowClose(WindowCloseEvent& e)
	{
		m_Running = false;
		return true;
	}

	bool Application::OnWindowResize(WindowResizeEvent& e) {

		m_Minimized = e.GetWidth() == 0 || e.GetHeight() == 0;

		if (!m_Minimized) {
			Renderer::OnWindowResize(e.GetWidth(), e.GetHeight());
		}

		return false;

		/*if (e.GetWidth() == 0 || e.GetHeight() == 0) {
			m_Mimimized = true;
			return false;
		}
		m_Mimimized = false;
		
		return false;*/
	}

}
