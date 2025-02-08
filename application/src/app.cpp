#include "app.hpp"
#include "startPoint.hpp"
#include "Lab.hpp"



App::App(const WindowProperties& winProps) : Application(winProps)
{
	m_layer = std::unique_ptr<Layer>(new Scene0(m_window));
}

Application* startApplication()
{
	WindowProperties props("GAMR3521",  1920 * 0.5f, 1080 * 0.5f);
	props.isHostingImGui = true;
	props.isResizable = false;
	return new App(props);
}