#pragma once

#include <glm/glm.hpp>

#include "gameObjects/actor.hpp"
#include "windows/GLFWWindowImpl.hpp"
#include "core/log.hpp"

#include "scripts/include/camera.hpp"

class ControllerScript : public Script
{
public:
	ControllerScript(Actor& actor, Actor& camera, GLFWWindowImpl& win, const glm::vec3& movementSpeed, float turnSpeed) :
		Script(actor),
		m_winRef(win),
		m_camera(camera),
		m_movementSpeed(movementSpeed),
		m_turnSpeed(turnSpeed)
	{}
	virtual void onUpdate(float timestep) override;
	virtual void onKeyPress(KeyPressedEvent& e) override {};
	virtual void onKeyRelease(KeyReleasedEvent& e) override {};

	glm::vec3 FloorCast{ 0,0.5f,0 };

private:
	GLFWWindowImpl& m_winRef;
	Actor& m_camera;
	glm::vec3 m_movementSpeed{ 1.f, 1.f, 1.f };
	glm::vec3 m_velocity{ 0.001f,0,0 };
	bool m_onFloor{ false };
	bool m_creativeMode{ true };
	float timer{ 0.0f };
	bool m_mouseToggled{ false };
	float m_turnSpeed{ 0.5f };
	//bool m_mouseToggle{ false };
};
