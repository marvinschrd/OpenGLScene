#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

namespace gl {

	// Defines several possible options for camera movement. Used as
	// abstraction to stay away from window-system specific input methods
	enum class CameraMovementEnum {
		FORWARD,
		BACKWARD,
		LEFT,
		RIGHT
	};

	// Default camera values
	const float YAW = -90.0f;
	const float PITCH = 0.0f;
	const float SPEED = 50.0f;
	const float SENSITIVITY = 0.2f;
	const float ZOOM = 45.0f;


	// An abstract camera class that processes input and calculates the
	// corresponding Euler Angles, Vectors and Matrices for use in OpenGL
	class Camera
	{
	public:
		// camera Attributes
		glm::vec3 position;
		glm::vec3 front;
		glm::vec3 vertical = glm::vec3(0.0f, 1.0f,0.0f);
		glm::vec3 up;
		glm::vec3 right;
		glm::vec3 world_up;
		// euler Angles
		float yaw;
		float pitch;
		float roll;
		// camera options
		float MovementSpeed;
		float MouseSensitivity;
		float Zoom;
		float fov_ = 45.0f;

		// constructor with vectors
		Camera(
			glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), 
			glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), 
			float yaw = YAW, 
			float pitch = PITCH) : 
			front(glm::vec3(0.0f, 0.0f, -1.0f)), 
			MovementSpeed(SPEED), 
			MouseSensitivity(SENSITIVITY), 
			Zoom(ZOOM)
		{
			this->position = position;
			this->world_up = up;
			this->yaw = yaw;
			this->pitch = pitch;
			updateCameraVectors();
		}
		// constructor with scalar values
		Camera(
			float posX, float posY, float posZ, 
			float upX, float upY, float upZ, 
			float yaw, float pitch) : 
			front(glm::vec3(0.0f, 0.0f, -1.0f)),
		
			MovementSpeed(SPEED), 
			MouseSensitivity(SENSITIVITY), 
			Zoom(ZOOM)
		{
			position = glm::vec3(posX, posY, posZ);
			world_up = glm::vec3(upX, upY, upZ);
			yaw = yaw;
			pitch = pitch;
			updateCameraVectors();
		}

		// returns the view matrix calculated using Euler Angles and the LookAt
		// Matrix
		glm::mat4 GetViewMatrix()
		{
			return glm::lookAt(position, position + front, up);
		}

		// processes input received from any keyboard-like input system.
		// Accepts input parameter in the form of camera defined ENUM (to
		// abstract it from windowing systems)
		void ProcessKeyboard(CameraMovementEnum direction, float deltaTime);

		// processes input received from a mouse input system. Expects the
		// offset value in both the x and y direction.
		void ProcessMouseMovement(
			float xoffset,
			float yoffset,
			float deltaTime,
			GLboolean constrainpitch = true
		);

		// processes input received from a mouse scroll-wheel event. Only
		// requires input on the vertical wheel-axis
		void ProcessMouseScroll(float yoffset)
		{
			Zoom -= (float)yoffset;
			if (Zoom < 1.0f)
				Zoom = 1.0f;
			if (Zoom > 45.0f)
				Zoom = 45.0f;
		}

		void LookAt(glm::vec3 target, glm::vec3 lookUp = glm::vec3(0, 1, 0))
		{
			front = glm::normalize(target - position);
			right = glm::normalize(glm::cross(front, world_up));
			up = glm::normalize(glm::cross(front, right));
			//world_up = glm::normalize(glm::cross(front, right));
		}

		glm::mat4 GetProjection()
		{
			return glm::perspective(glm::radians(fov_), 4.0f / 3.0f, 0.1f, 100.f);
		}

	private:
		// calculates the front vector from the Camera's (updated) Euler Angles
		void updateCameraVectors();
	};

} // End namespace gl.
