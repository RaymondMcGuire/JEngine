#include "JE_PCH.h"
#include "Application.h"
#include "Events/Event.h"
#include "glad/glad.h"

namespace JEngine {

	Application::Application() {
		// Create default JEngine Editor Window
		m_Window = std::unique_ptr<Window>(Window::CreateJEWindow());
		m_Window->SetEventCallback(EVENT_BIND_FUNCTION(Application::OnEvent));
	}

	void Application::OnEvent(Event& e) {
		EventDispatcher dispatcher(e);
		dispatcher.DispatchEvent<WindowCloseEvent>(EVENT_BIND_FUNCTION(Application::OnWindowCloseEvent));
		// JE_CORE_INFO("{0}", e);

		// Back trace layer to get the response layer
		for (auto it = m_LayerStack.end(); it != m_LayerStack.begin(); )
		{
			(*(--it))->OnEvent(e);
			if (e.m_handled)
				break;
		}
	}

	void Application::PushLayer(Layer* layer)
	{
		m_LayerStack.PushLayer(layer);
	}

	void Application::PushOverlay(Layer* overlay)
	{
		m_LayerStack.PushOverlay(overlay);
	}

	Application::~Application() {

	}

	void Application::Run() {
		//WindowResizeEvent e(1280,720);
		//JE_TRACE(e);
		while (m_Running) {
			// Clear Buffer
			glClearColor(0.3, 0.3, 0.3, 1);
			glClear(GL_COLOR_BUFFER_BIT);

			// Update Objects Based on layer order
			for (Layer* layer : m_LayerStack)
				layer->OnUpdate();

			// Render Logic
			m_Window->OnUpdate();
		}
	}

	bool Application::OnWindowCloseEvent(WindowCloseEvent& e)
	{
		m_Running = false;
		return true;
	}
}