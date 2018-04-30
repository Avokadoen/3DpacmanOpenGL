#include "score.hpp"

/*
	This class exist for dealing with a custom render that can update
	vao data so that when pacman picks up score it can be represented in
	the game

*/

void Score::deleteIndices(int index) {
	indices.erase(indices.begin() + index * indicesStride, indices.begin() + index * indicesStride + indicesStride);
	hitboxes.erase(hitboxes.begin() + index, hitboxes.begin() + index + 1);

	bindEverything();
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * indices.size(), indices.data(), GL_DYNAMIC_DRAW);
	unBindEverything();
}

void Score::registerMesh(Mesh& mesh, std::vector<glob::Vertex>& vertex,
	std::vector<GLuint>& indices, std::vector<glob::Texture>& texture)
{
	glUseProgram(shaderProgram);
	for (auto&& index : indices) {
		index += vertexCount;
		this->indices.push_back(index);
	}

	indicesStride = indices.size();

	unsigned int diffuseNr = 1;
	unsigned int specularNr = 1;
	for (unsigned int i = 0; i < texture.size(); i++)
	{
		glActiveTexture(GL_TEXTURE0 + i); // activate proper texture unit before binding
										  // retrieve texture number (the N in diffuse_textureN)
		std::string number;
		std::string name = texture[i].type;
		if (name == "texture_diffuse")
			number = std::to_string(diffuseNr++);
		else if (name == "texture_specular")
			number = std::to_string(specularNr++);

		glUniform1i(glGetUniformLocation(shaderProgram, ("material." + name + number).c_str()), i);

		glBindTexture(GL_TEXTURE_2D, texture[i].id);
	}
	glActiveTexture(GL_TEXTURE0);

	bindEverything();

	glBufferSubData(GL_ARRAY_BUFFER, sizeof(glob::Vertex) * vertexCount, sizeof(glob::Vertex) * vertex.size(), vertex.data());
	glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * indexCount, sizeof(GLuint) * indices.size(), indices.data());

	vertexCount += vertex.size();
	indexCount += indices.size();

	transform = mesh.transform;
	prevTransform = transform;

	uniTrans = glGetUniformLocation(shaderProgram, "transform");
	glUniformMatrix4fv(uniTrans, 1, GL_FALSE, glm::value_ptr(transform));

	hitboxes.push_back(createHitbox(vertex));

	glUseProgram(0);
	unBindEverything();

}

std::array<glm::vec3, 8> Score::createHitbox(std::vector<glob::Vertex>& vertices){

	GLfloat leastX = 999999;
	GLfloat leastY = 999999;
	GLfloat leastZ = 999999;

	GLfloat greatestX = -999999;
	GLfloat greatestY = -999999;
	GLfloat greatestZ = -999999;

	for (auto vertex : vertices) {
		if (vertex.Position.x < leastX) {
			leastX = vertex.Position.x;
		}
		if (vertex.Position.x > greatestX) {
			greatestX = vertex.Position.x;
		}
		if (vertex.Position.y < leastY) {
			leastY = vertex.Position.y;
		}
		if (vertex.Position.y > greatestY) {
			greatestY = vertex.Position.y;
		}
		if (vertex.Position.z < leastZ) {
			leastZ = vertex.Position.z;
		}
		if (vertex.Position.z > greatestZ) {
			greatestZ = vertex.Position.z;
		}
	}

	float rbs = abs(greatestX - leastX) * 1.1; // relativ box scale

	glm::vec3 leftLowerFloor = glm::vec3(leastX - rbs, leastY - rbs, leastZ - rbs);
	glm::vec3 leftHigherFloor = glm::vec3(leastX - rbs, greatestY + rbs, leastZ - rbs);
	glm::vec3 rightLowerFloor = glm::vec3(greatestX + rbs, leastY - rbs, leastZ - rbs);
	glm::vec3 rightHigherFloor = glm::vec3(greatestX + rbs, greatestY + rbs, leastZ - rbs);
	glm::vec3 leftLowerRoof = glm::vec3(leastX - rbs, leastY - rbs, greatestZ + rbs);
	glm::vec3 leftHigherRoof = glm::vec3(leastX - rbs, greatestY + rbs, greatestZ + rbs);
	glm::vec3 rightLowerRoof = glm::vec3(greatestX + rbs, leastY - rbs, greatestZ + rbs);
	glm::vec3 rightHigherRoof = glm::vec3(greatestX + rbs, greatestY + rbs, greatestZ + rbs);

	std::array<glm::vec3, 8> rtrHitBox;
	rtrHitBox[0] = leftLowerFloor;
	rtrHitBox[1] = leftHigherFloor;
	rtrHitBox[2] = rightLowerFloor;
	rtrHitBox[3] = rightHigherFloor;
	rtrHitBox[4] = leftLowerRoof;
	rtrHitBox[5] = leftHigherRoof;
	rtrHitBox[6] = rightLowerRoof;
	rtrHitBox[7] = rightHigherRoof;

	return rtrHitBox;
}

int Score::checkScoreCollision(std::array<glm::vec3, 8> targetHitbox, glm::mat4 targetTransform) {

	int index = -1;
	for (auto hitbox : hitboxes) {
		index++;
		for (auto targetPoint : targetHitbox) {
			targetPoint = targetTransform * glm::vec4(targetPoint, 1);

			glm::vec4 transformedMemberLower = transform * glm::vec4(hitbox[0], 1);
			glm::vec4 transformedMemberHigher = transform * glm::vec4(hitbox[7], 1);

			if (transformedMemberLower.x <= targetPoint.x &&
				transformedMemberLower.y <= targetPoint.y &&
				transformedMemberHigher.x >= targetPoint.x &&
				transformedMemberHigher.y >= targetPoint.y)
				//transformedMemberLower.z <= targetPoint.z &&
				//transformedMemberHigher.z >= targetPoint.z)
			{
				return index;
			}

		}
	}
	return -1;
}