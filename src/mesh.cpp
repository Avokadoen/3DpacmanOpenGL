#include "mesh.hpp"

#include "gtc/matrix_transform.hpp"

Mesh::Mesh() {
	transform = glm::mat4(1.0f);
}

Mesh::Mesh(std::vector<glob::Vertex> vertex) {
	// TODO: make hitbox

	transform = glm::mat4(1.0f);

	//indexCount = inds.size();

}

void Mesh::update(float deltaTime, glm::mat4 transformChange) {
	// TODO
}

unsigned int Mesh::getindexCount() {
	return indexCount;
}

