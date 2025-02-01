#pragma once

#include <glm/glm.hpp>

#include "gameObjects/actor.hpp"
#include "windows/GLFWWindowImpl.hpp"
#include "core/log.hpp"

class CameraScript : public Script
{
public:
	CameraScript(Actor& actor, GLFWWindowImpl& win, glm::vec3* origin, glm::vec3 offset, float armLength, const glm::vec3& movementSpeed, float turnSpeed) :
		Script(actor),
		m_winRef(win),
		m_origin(origin),
		m_offset(offset),
		m_armLength(armLength),
		m_movementSpeed(movementSpeed),
		m_turnSpeed(turnSpeed)
	{
		m_targetRotation = actor.rotation;
		m_eularTargetRotation = glm::eulerAngles(actor.rotation);
	};
	virtual void onUpdate(float timestep) override;
	virtual void onKeyPress(KeyPressedEvent& e) override {};
	virtual void onKeyRelease(KeyReleasedEvent& e) override {};

	glm::vec3 m_cameraForward{ 0.0f, 0.0f , 1.0f };
	glm::vec3* m_origin;
	glm::vec3 m_offset{ 0,0,0 };
	float m_armLength;

private:
	GLFWWindowImpl& m_winRef;
	glm::vec3 m_movementSpeed{ 1.f, 1.f, 1.f };
	glm::vec3 m_eularTargetRotation;
	glm::quat m_targetRotation;
	float m_turnSpeed{ 0.5f };
	//bool m_mouseToggle{ false };
};
