#pragma once

#include "render.hpp"

class Score : public Render{
public:
	
	void deleteIndices(int index);
	virtual void registerMesh(Mesh& mesh, std::vector<glob::Vertex>& vertex,
			std::vector<GLuint>& indices, std::vector<glob::Texture>& texture);
	int checkScoreCollision(std::array<glm::vec3, 8> targetHitbox, glm::mat4 targetTransform);
	virtual std::array<glm::vec3, 8> createHitbox(std::vector<glob::Vertex>& vertices);
private:
	int indicesStride;
	std::vector<GLuint> indices;
};