#include "camera.h"

namespace gl
{
	void Camera::ProcessKeyboard(CameraMovementEnum direction, float deltaTime)
	{
		float velocity = MovementSpeed * deltaTime;
		if (direction == CameraMovementEnum::FORWARD)
			position += front * velocity;
		if (direction == CameraMovementEnum::BACKWARD)
			position -= front * velocity;
		if (direction == CameraMovementEnum::LEFT)
			position -= right * velocity;
		if (direction == CameraMovementEnum::RIGHT)
			position += right * velocity;
	}

	void Camera::ProcessMouseMovement(float xoffset, float yoffset, float deltaTime, GLboolean constrainpitch)
	{
		float Velocity = MouseSensitivity * deltaTime;

		xoffset *= MouseSensitivity;
		yoffset *= MouseSensitivity;

		/*xoffset *= Velocity;
		yoffset *= Velocity;*/

		yaw += xoffset;
		pitch += yoffset;

		// make sure that when pitch is out of bounds, screen doesn't get
		// flipped
		if (constrainpitch)
		{
			if (pitch > 89.0f)
				pitch = 89.0f;
			if (pitch < -89.0f)
				pitch = -89.0f;
		}

		// update front, right and up Vectors using the updated Euler
		// angles
		updateCameraVectors();
	}

	void Camera::updateCameraVectors()
	{
		// calculate the new front vector
		glm::vec3 front_;
		front_.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
		front_.y = sin(glm::radians(pitch));
		front_.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
		front = glm::normalize(front_);
		// also re-calculate the right and up vector
		right = glm::normalize(glm::cross(front, world_up));
		// normalize the vectors, because their length gets closer to 0 the
		// more you look up or down which results in slower movement.
		up = glm::normalize(glm::cross(right, front));
	}


	
}