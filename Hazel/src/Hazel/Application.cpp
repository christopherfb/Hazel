#include "hzpch.h"

#include "Application.h"
#include "Hazel/Events/ApplicationEvent.h"
// #include "Hazel/Log.h" // now in pch

#include <GLFW/glfw3.h>

namespace Hazel {

	Application::Application()
	{
		m_Window = std::unique_ptr<Window>(Window::Create());

	}

	Application::~Application()
	{
	}

	void Application::Run()
	{
		//WindowResizeEvent e(1280, 720);       
		// 
		//// HZ_TRACE(e);  // In the video - this line was used, but it didn't compile.  
		//// A comment said the fix was to add e.ToString() (see below)
		//if (e.IsInCategory(EventCategoryApplication)) {
		//	HZ_TRACE(e.ToString());
		//}

		//if (e.IsInCategory(EventCategoryInput)) {
		//	HZ_TRACE(e.ToString());
		//}

		while (m_Running) 
		{
			glClearColor(1, 0, 1, 1);
			glClear(GL_COLOR_BUFFER_BIT);
			m_Window->OnUpdate();
		}
	}
}
