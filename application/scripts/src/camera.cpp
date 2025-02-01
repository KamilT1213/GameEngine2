#include "scripts/include/camera.hpp"
#include <string>
void CameraScript::onUpdate(float timestep)
{
	bool recalc = true;

	m_cameraForward ={ -m_actor.transform[2][0], -m_actor.transform[2][1], -m_actor.transform[2][2] };
	glm::vec3 Forward = glm::normalize(m_cameraForward * glm::vec3(1, 0, 1));

	m_actor.translation = *m_origin + m_offset + (m_cameraForward * -m_armLength);
	//Custom Mouse Controls 

	glm::vec2 MouseVector = m_winRef.doGetMouseVector();
	glm::vec3 CurrentCameraRotation = m_eularTargetRotation;

	glm::vec2 Normlizer = glm::normalize(MouseVector) * glm::distance(glm::vec2(0,0), MouseVector);

	if (abs(MouseVector.x)  > 0.0) {
		CurrentCameraRotation.y += m_turnSpeed  * -Normlizer.x;
		recalc = true;
	}

	if (abs(MouseVector.y)  > 0.0) {
		CurrentCameraRotation.x += m_turnSpeed  * -Normlizer.y;
		recalc = true;
	}

	if (CurrentCameraRotation.x > 0.9f) {
		CurrentCameraRotation.x = 0.9f;
	}
	else if (CurrentCameraRotation.x < -1.0) {
		CurrentCameraRotation.x = -1.0;
	}
	CurrentCameraRotation.z = 0;
	m_eularTargetRotation = CurrentCameraRotation;
	m_targetRotation = glm::quat(CurrentCameraRotation);

	m_actor.rotation = glm::slerp(m_actor.rotation, m_targetRotation, 1.0f);// timestep* m_turnSpeed * 0.90f);


	

	if (recalc) m_actor.recalc();
}

