#include "camera.hpp"

Camera::Camera() {
	position = glm::vec3(3.5f, -6.0f, 13.0f);

	view = glm::lookAt(
		position,
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 0.0f, 1.0f)
	);

	projection = glm::perspective(glob::FOV, (float)(glob::WINDOW_X / glob::WINDOW_Y), glob::MIN_RENDER_DISTANCE, glob::MAX_RENDER_DISTANCE);

	speed = 1.5f;
}

void Camera::newView(glm::vec3 pos, glm::vec3 target) {
	position = pos;


	view = glm::lookAt(
		position,
		target,
		glm::vec3(0.0f, 0.0f, 1.0f)
	);
}