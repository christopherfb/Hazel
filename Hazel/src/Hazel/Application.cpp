#include "Application.h"
#include "Hazel/Events/ApplicationEvent.h"
#include "Hazel/Log.h"

namespace Hazel {

	Application::Application()
	{
	}

	Application::~Application()
	{
	}

	void Application::Run()
	{
		WindowResizeEvent e(1280, 720);
		
		// HZ_TRACE(e);  // In the video - this line was used, but it didn't compile.  
		// A comment said the fix was to add e.ToString() (see below)
		if (e.IsInCategory(EventCategoryApplication)) {
			HZ_TRACE(e.ToString());
		}

		if (e.IsInCategory(EventCategoryInput)) {
			HZ_TRACE(e.ToString());
		}

		while (true);
	}
}
