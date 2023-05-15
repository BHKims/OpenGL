#include "Camera.h"

float lastX = 400;
float lastY = 300;
bool firstMouse = true;

void Camera::mouseInput(double xpos, double ypos) {
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = (xpos - lastX) * sensitivity;
	float yoffset = (lastY - ypos) * sensitivity;

	lastX = xpos;
	lastY = ypos;

	yaw += xoffset;
	pitch += yoffset;

	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	glm::vec3 direction;
	direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	direction.y = sin(glm::radians(pitch));
	direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	Front = glm::normalize(direction);
}

void Camera::scrollInput(double xoffset, double yoffset) {
	fov -= (float)yoffset;
	if (fov < 1.0f)
		fov = 1.0f;
	if (fov > 45.0f)
		fov = 45.0f;
}

void Camera::keyboardInput(GLFWwindow* window, float deltaTime) {
	const float cameraSpeed = speedCoeff * deltaTime;

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		Pos += cameraSpeed * Front;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		Pos -= cameraSpeed * Front;

	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		Pos -= glm::normalize(glm::cross(Front, Up)) * cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		Pos += glm::normalize(glm::cross(Front, Up)) * cameraSpeed;
}