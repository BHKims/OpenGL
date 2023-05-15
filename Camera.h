#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <functional>

class Camera
{
public:
	glm::vec3 Pos = glm::vec3(0.0f, 0.0f, 3.0f);
	glm::vec3 Front = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 Up = glm::vec3(0.0f, 1.0f, 0.0f);
	
	float yaw = -90.0f;
	float pitch = 0.0f;
	float fov = 45.0f; // field of view. fov gets smaller, then zooming in.

	float sensitivity = 0.1f;
	float speedCoeff = 2.5f;

	void mouseInput(double xpos, double ypos); // Update Front vector
	void scrollInput(double xoffset, double yoffset); // Update FOV (Field of View)
	void keyboardInput(GLFWwindow* window, float deltaTime); // Update Pos vector

	inline glm::vec3 getPos() { return Pos; }
	inline glm::vec3 getFront() { return Front; }
	inline glm::vec3 getUp() { return Up; }
	inline float getFOV() { return fov;	}
};

#endif