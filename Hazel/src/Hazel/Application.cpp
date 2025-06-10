#include "hzpch.h"

#include "Application.h"


// #include "Hazel/Log.h" // now in pch

//#include <GLFW/glfw3.h>

#include <glad/glad.h>  //  Video:14 @ 19:38 explains this.


namespace Hazel {

//#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)

	Application* Application::s_Instance = nullptr;

	static GLenum ShaderDataTypeToOpenGLBaseType(ShaderDataType type) {
		switch (type)
		{
			case Hazel::ShaderDataType::None:	
			case Hazel::ShaderDataType::Float:	
			case Hazel::ShaderDataType::Float2:	
			case Hazel::ShaderDataType::Float3:	
			case Hazel::ShaderDataType::Float4:	
			case Hazel::ShaderDataType::Mat3:	
			case Hazel::ShaderDataType::Mat4:	return GL_FLOAT; break;

			case Hazel::ShaderDataType::Int:	
			case Hazel::ShaderDataType::Int2:	
			case Hazel::ShaderDataType::Int3:	
			case Hazel::ShaderDataType::Int4:	return GL_INT;	break;

			case Hazel::ShaderDataType::Bool:	return GL_BOOL;	break;
		}
		HZ_CORE_ASSERT(false, "Unknown ShaderDataType!");
		return 0;
	}

	Application::Application()
	{
		HZ_CORE_ASSERT(!s_Instance, "Application already exists!");
		s_Instance = this;
		
		m_Window = std::unique_ptr<Window>(Window::Create());
		m_Window->SetEventCallback(HZ_BIND_EVENT_FN(Application::OnEvent));
		// the long version
		//m_Window->SetEventCallback(std::bind(&Application::OnEvent, this, std::placeholders::_1));

		m_ImGuiLayer = new ImGuiLayer();  // ownership of the layer gets transferred to the Layer stack (which handles deleting it).
		PushOverlay(m_ImGuiLayer);

		// vertex array
		// vertex buffer
		// index buffer
		// shader

		glGenVertexArrays(1, &m_VertexArray);
		glBindVertexArray(m_VertexArray);

		//glGenBuffers(1, &m_IndexBuffer);
		//glBindBuffer(GL_ARRAY_BUFFER, m_VertexBuffer);

		// this describes the triange. The first 3 are coords, the last 4 are colors
		float vertices[3 * 7] = {
			-0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
			0.5f, -0.5f, 0.0f,	0.0f, 0.0f, 1.0f, 1.0f,
			0.0f, 0.5f, 0.0f,	1.0f, 1.0f, 0.0f, 1.0f
		};

		m_VertexBuffer.reset(VertexBuffer::Create(vertices, sizeof(vertices)));
		//VertexBuffer buffer = VertexBuffer::Create(sizeof(vertices), vertices);
		//buffer.Bind();
		//glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		BufferLayout layout = {
			{ShaderDataType::Float3, "a_Position" },
			{ShaderDataType::Float4, "a_Color" },
			//{ShaderDataType::Float3, "a_Normal" }
		};
		m_VertexBuffer->SetLayout(layout);

		//BufferLayout layout2(layout)
		uint32_t index = 0;

		const auto& layout2 = m_VertexBuffer->GetLayout();

		for (const auto& element : layout2) {
			glEnableVertexAttribArray(index);
			glVertexAttribPointer(index,
				element.GetComponentCount(),
				ShaderDataTypeToOpenGLBaseType(element.Type),
				element.Normalized ? GL_TRUE : GL_FALSE,
				layout2.GetStride(), //3 * sizeof(float),
				(const void*)element.Offset); // nullptr);
			index++;
		}


		uint32_t indices[3] = {
			0,1,2
		};

		m_IndexBuffer.reset(IndexBuffer::Create(indices, sizeof(indices)/sizeof(uint32_t)));
		//glGenBuffers(1, &m_IndexBuffer);
		//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndexBuffer);
		//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
		
		std::string vertexSrc = R"(
			#version 330 core
			layout(location = 0) in vec3 a_Position;
			layout(location = 1) in vec4 a_Color;
			out vec3 v_Position;
			out vec4 v_Color;

			void main()
			{
				v_Position = a_Position;
				v_Color = a_Color;
				gl_Position = vec4(a_Position, 1.0);
			}
		)";

		std::string fragmentSrc = R"(
			#version 330 core
			layout(location = 0) out vec4 color;

			in vec3 v_Position;
			in vec4 v_Color;
			void main()
			{
				color = vec4(v_Position * 0.5 + 0.5, 1.0);
				color = v_Color;
			}
		)";

		m_Shader.reset(new Shader(vertexSrc, fragmentSrc));
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
			glClearColor(0.1f, 0.1f, 0.1f, 1);
			glClear(GL_COLOR_BUFFER_BIT);

			m_Shader->Bind();

			glBindVertexArray(m_VertexArray);
			glDrawElements(GL_TRIANGLES, m_IndexBuffer->GetCount(), GL_UNSIGNED_INT, nullptr);

			for (Layer* layer : m_LayerStack) {
				layer->OnUpdate();
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
