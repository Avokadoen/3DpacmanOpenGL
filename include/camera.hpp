#pragma once

#include "globals.hpp"

#include "glm.hpp"
#include <gtc/matrix_transform.hpp>

#include <memory>

// camera should have a weak pointer to a transform 
//		if pointer is initiated it will observe trans

class Camera {
public:
	Camera();
	void setNewProjection();
	void setCameraTarget();
	void setCameraOffset();
	void newView(glm::vec3 pos, glm::vec3 target);
	void rotate();

	glm::vec3 position;
	//glm::vec3 target;
	glm::mat4 projection;
	glm::mat4 view;
	float speed;
};