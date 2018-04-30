#include "render.hpp"

/*
	sources:
	https://open.gl/transformations : 3D transformations
*/


/*
	TODO:
	-1:
	Render only takes one transform. this should be a array of transform with size of max
	meshes. drawing should send mesh index to shader which shader will use to access correct transform. 
	(updater / controller task)
*/

/*
	The render class has been modified to deal with a lot the local game logic for a
	object. There is no logical reason for this being there other then easy access and
	allowed use to achieve a lot of the desired game logic in a fast way
*/


Render::Render() {
	// TODO: this should be done by parent/controller (class controlling renderer and updater)
	//mesh = std::make_shared<Mesh>();
	direction = glob::RIGHT;
	indexCount = 0;
	vertexCount = 0;
}


void Render::draw() {

	bindEverything();
	glUseProgram(shaderProgram);

	glUniformMatrix4fv(uniTrans, 1, GL_FALSE, glm::value_ptr(transform));
	glUniformMatrix4fv(uniView, 1, GL_FALSE, glm::value_ptr(camera->view));

	// TODO: make sure Render has meshes
	
	glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, (GLvoid*)0);
	
	glUseProgram(0);
	unBindEverything();

}

void Render::init(std::string vertPath, std::string fragPath, unsigned int vertexPrMesh, unsigned int IndexPrMesh, std::shared_ptr<Camera> camera) {

	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ebo);

	glBindVertexArray(vao);	
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * IndexPrMesh * glob::MAX_OBJECT_PR_RENDER, nullptr, GL_STATIC_DRAW);
	
	// TODO: string with shader path should be saved and check against 
	shaderProgram = create_program(vertPath.c_str(), fragPath.c_str());

	glUseProgram(shaderProgram);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glob::Vertex), (void*)offsetof(glob::Vertex, Position));

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(glob::Vertex), (void*)offsetof(glob::Vertex, Normal));
	// vertex texture coords
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(glob::Vertex), (void*)offsetof(glob::Vertex, TexCoords));

	glBufferData(GL_ARRAY_BUFFER, sizeof(glob::Vertex) * vertexPrMesh * glob::MAX_OBJECT_PR_RENDER, nullptr, GL_STATIC_DRAW);


	this->camera = camera;
	uniView = glGetUniformLocation(shaderProgram, "view");
	glUniformMatrix4fv(uniView, 1, GL_FALSE, glm::value_ptr(this->camera->view));


	uniProj = glGetUniformLocation(shaderProgram, "proj");
	glUniformMatrix4fv(uniProj, 1, GL_FALSE, glm::value_ptr(this->camera->projection));
	
	glUseProgram(0);
	unBindEverything();
	
}

void Render::registerMesh(Mesh& mesh, std::vector<glob::Vertex>& vertex, std::vector<GLuint>& indices, std::vector<glob::Texture>& texture) {
	glUseProgram(shaderProgram);
	for (auto&& index : indices) {
		index += vertexCount;
	}

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

void Render::setTexture(GLuint textureID) {
	this->textureID = textureID;
}

void Render::bindEverything() {
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBindTexture(GL_TEXTURE_2D, textureID);
}

void Render::unBindEverything() {
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, GL_TEXTURE0);
}

/*
	param: hitbox to be tested against render members
	return: if TARGET is WITHIN render member hitbox
*/
bool Render::checkCollision(std::array<glm::vec3, 8> targetHitbox, glm::mat4 targetTransform) {
	int lowerIndex;
	int higherIndex;
	if (direction == glob::RIGHT) {
		lowerIndex = 0;
		higherIndex = 7;
	}
	else if (direction == glob::DOWN) {
		lowerIndex = 5;
		higherIndex = 2;
	}
	else if (direction == glob::LEFT) {
		lowerIndex = 7;
		higherIndex = 0;
	}
	else if (direction == glob::UP) {
		lowerIndex = 2;
		higherIndex = 5;
	}

	for (auto hitbox : hitboxes) {
		for (auto targetPoint : targetHitbox) {
			targetPoint = targetTransform * glm::vec4(targetPoint, 1);

			glm::vec4 transformedMemberLower = transform * glm::vec4(hitbox[lowerIndex], 1);
			glm::vec4 transformedMemberHigher = transform * glm::vec4(hitbox[higherIndex], 1);

			if (transformedMemberLower.x <= targetPoint.x &&
				transformedMemberLower.y <= targetPoint.y &&
				transformedMemberHigher.x >= targetPoint.x &&
				transformedMemberHigher.y >= targetPoint.y )
				//transformedMemberLower.z <= targetPoint.z &&
				//transformedMemberHigher.z >= targetPoint.z)
			{ 
				return true;
			}
			
		}
	}
	return false;
}

int Render::checkTeleportCollision(std::array<glm::vec3, 8> targetHitbox, glm::mat4 targetTransform) {
	int lowerIndex;
	int higherIndex;
	if (direction == glob::RIGHT) {
		lowerIndex = 0;
		higherIndex = 7;
	}
	else if (direction == glob::DOWN) {
		lowerIndex = 5;
		higherIndex = 2;
	}
	else if (direction == glob::LEFT) {
		lowerIndex = 7;
		higherIndex = 0;
	}
	else if (direction == glob::UP) {
		lowerIndex = 2;
		higherIndex = 5;
	}

	int index = -1;
	for (auto hitbox : hitboxes) {
		index++;
		for (auto targetPoint : targetHitbox) {
			targetPoint = targetTransform * glm::vec4(targetPoint, 1);

			glm::vec4 transformedMemberLower = transform * glm::vec4(hitbox[lowerIndex], 1);
			glm::vec4 transformedMemberHigher = transform * glm::vec4(hitbox[higherIndex], 1);

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

std::array<glm::vec3, 8> Render::createHitbox(std::vector<glob::Vertex>& vertices) {
	
	GLfloat leastX		= 999999; 
	GLfloat leastY		= 999999; 
	GLfloat leastZ		= 999999;

	GLfloat greatestX	= -999999; 
	GLfloat greatestY	= -999999;
	GLfloat greatestZ	= -999999;

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

	glm::vec3 leftLowerFloor	= glm::vec3(leastX, leastY, leastZ);
	glm::vec3 leftHigherFloor	= glm::vec3(leastX, greatestY, leastZ);
	glm::vec3 rightLowerFloor	= glm::vec3(greatestX, leastY, leastZ);
	glm::vec3 rightHigherFloor	= glm::vec3(greatestX, greatestY, leastZ);
	glm::vec3 leftLowerRoof		= glm::vec3(leastX, leastY, greatestZ);
	glm::vec3 leftHigherRoof	= glm::vec3(leastX, greatestY, greatestZ);
	glm::vec3 rightLowerRoof	= glm::vec3(greatestX, leastY, greatestZ);
	glm::vec3 rightHigherRoof	= glm::vec3(greatestX, greatestY, greatestZ);

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
