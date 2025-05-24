#include "hzpch.h"

#include "Application.h"


// #include "Hazel/Log.h" // now in pch

#include <GLFW/glfw3.h>

#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)

namespace Hazel {

	Application::Application()
	{
		m_Window = std::unique_ptr<Window>(Window::Create());
		// the long version
		//m_Window->SetEventCallback(std::bind(&Application::OnEvent, this, std::placeholders::_1));
		m_Window->SetEventCallback(BIND_EVENT_FN(OnEvent));

	}

	Application::~Application()
	{
	}

	void Application::OnEvent(Event& e)
	{
		HZ_CORE_TRACE("{0}", e.ToString());
		//HZ_CORE_TRACE(e.ToString());
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>( BIND_EVENT_FN(OnWindowClose));
		

	}

	void Application::Run()
	{		
		while (m_Running) 
		{
			glClearColor(1, 0, 1, 1);
			glClear(GL_COLOR_BUFFER_BIT);
			m_Window->OnUpdate();
		}
	}

	bool Application::OnWindowClose(WindowCloseEvent& e)
	{
		m_Running = false;
		return true;
	}

}
