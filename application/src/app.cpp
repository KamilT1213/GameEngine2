#include "app.hpp"
#include "startPoint.hpp"
#include "Lab.hpp"



App::App(const WindowProperties& winProps) : Application(winProps)
{
	m_layer = std::unique_ptr<Layer>(new Scene0(m_window));
}

Application* startApplication()
{
	WindowProperties props("GAMR3521", 1920 * 0.75f, 1080 * 0.75f);
	props.isHostingImGui = true;
	props.isResizable = false;
	return new App(props);
}