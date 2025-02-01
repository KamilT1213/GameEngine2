#include "scripts/include/controller.hpp"
#include <string>

void ControllerScript::onUpdate(float timestep)
{
	bool recalc = false;

	
	glm::vec3 forward = { -m_camera.transform[2][0], -m_camera.transform[2][1], -m_camera.transform[2][2] };// ((CameraScript&)m_camera.m_script).m_cameraForward;// { -m_actor.transform[2][0], -m_actor.transform[2][1], -m_actor.transform[2][2] };
	forward = glm::normalize(forward * glm::vec3(1, 0, 1));

	glm::vec3 right = glm::cross(forward,glm::vec3(0,1,0));

	if (m_winRef.doIsKeyPressed(GLFW_KEY_F1)) { timer += timestep;
	if (timer > 1) {
		timer = 0;
		if (!m_creativeMode) {
			m_actor.translation += glm::vec3(0, 1, 0);
			recalc = true;
		}
		m_creativeMode = !m_creativeMode;

	}
	}
	else {
		timer = 0;
	}
	if (!m_creativeMode) {
		if (m_winRef.doIsKeyPressed(GLFW_KEY_W)) { m_velocity += forward * m_movementSpeed.z * timestep; recalc = true; }
		if (m_winRef.doIsKeyPressed(GLFW_KEY_A)) { m_velocity -= right * m_movementSpeed.x * timestep; recalc = true; }
		if (m_winRef.doIsKeyPressed(GLFW_KEY_S)) { m_velocity -= forward * m_movementSpeed.z * timestep; recalc = true; }
		if (m_winRef.doIsKeyPressed(GLFW_KEY_D)) { m_velocity += right * m_movementSpeed.x * timestep; recalc = true; }
		if (m_winRef.doIsKeyPressed(GLFW_KEY_TAB) && !m_mouseToggled) { m_winRef.doSwitchInput(); m_mouseToggled = true; }
		if (!m_winRef.doIsKeyPressed(GLFW_KEY_TAB) && m_mouseToggled) { m_mouseToggled = false; }

		//printf(std::to_string(FloorCast.y).c_str());
		//printf("\n");

		float distance = -(FloorCast.y);

		if (distance > 0.5f) {
			m_onFloor = false;

		}
		else {
			m_onFloor = true;
			if (distance < 0.5f) {
				m_actor.translation.y += (0.5f - distance);
			}


		}

		if (!m_onFloor) {
			m_velocity += glm::vec3(0, -9.8f, 0) * timestep;
		}
		else {



			m_velocity *= glm::vec3(1, 0, 1);
			if (m_winRef.doIsKeyPressed(GLFW_KEY_SPACE)) {
				m_velocity += glm::vec3(0, 4, 0);
				m_onFloor = false;
			}
		}

		m_velocity += -m_velocity * 0.125f * timestep;

		if (m_onFloor) {
			m_velocity += -(m_velocity * glm::vec3(1, 0, 1)) * (1 - (glm::distance(glm::vec3(0), m_velocity) / (9.8f * 2.0f * 4.15f))) * timestep;
		}
		if (timestep > 4) {
			m_velocity = glm::vec3(0);
		}
		m_actor.translation += m_velocity * timestep;

		if (glm::distance(glm::vec3(0), m_velocity * glm::vec3(1, 0, 1)) > 0.01f) {
			m_actor.rotation = glm::rotation(glm::vec3(0, 0, -1), glm::normalize(m_velocity * glm::vec3(1, 0, 1)));
		}
		else {
			m_actor.rotation = glm::rotation(glm::vec3(0, 0, -1), forward);
		}


		m_actor.recalc();
	}
	else {
		m_velocity = glm::vec3(0);
		if (m_winRef.doIsKeyPressed(GLFW_KEY_W)) { m_actor.translation += forward * m_movementSpeed.z * timestep; recalc = true; m_velocity += forward; }
		if (m_winRef.doIsKeyPressed(GLFW_KEY_A)) { m_actor.translation -= right * m_movementSpeed.x * timestep; recalc = true; m_velocity -= right;}
		if (m_winRef.doIsKeyPressed(GLFW_KEY_S)) { m_actor.translation -= forward * m_movementSpeed.z * timestep; recalc = true; m_velocity -= forward;}
		if (m_winRef.doIsKeyPressed(GLFW_KEY_D)) { m_actor.translation += right * m_movementSpeed.x * timestep; recalc = true; m_velocity += right;}
		if (m_winRef.doIsKeyPressed(GLFW_KEY_SPACE)) { m_actor.translation += glm::vec3(0, 1, 0) * m_movementSpeed.x * timestep; recalc = true; }
		if (m_winRef.doIsKeyPressed(GLFW_KEY_LEFT_SHIFT)) { m_actor.translation += glm::vec3(0, -1, 0) * m_movementSpeed.x * timestep; recalc = true; }
		if (m_winRef.doIsKeyPressed(GLFW_KEY_TAB) && !m_mouseToggled) { m_winRef.doSwitchInput(); m_mouseToggled = true; }
		if (!m_winRef.doIsKeyPressed(GLFW_KEY_TAB) && m_mouseToggled) { m_mouseToggled = false; }
		if (glm::distance(glm::vec3(0), m_velocity) > 0.01f) {
			m_actor.rotation = glm::rotation(glm::vec3(0, 0, -1), glm::normalize(m_velocity * glm::vec3(1, 0, 1)));
		}
	}

	if (recalc) m_actor.recalc();
}

