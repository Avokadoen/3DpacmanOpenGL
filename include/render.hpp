#pragma once

#include "GL/glew.h"
#include "shaderload.hpp"

#include "globals.hpp"
#include "mesh.hpp"
#include "camera.hpp"

#include <memory>
#include <vector>
#include <array>
#include <string>
#include <iostream>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>



/*
	This class is for rendering a specific type of mesh of same 
	stride. Its task is to simply render the given vbo vao using
	correct shaderprogram.
*/

/*
	modified to work with game class.
*/

class Render {
public:
	Render();
	virtual void draw();
	virtual void init(std::string vertPath, std::string fragPath, unsigned int VertexSize, unsigned int indexCount, std::shared_ptr<Camera> camera);
	virtual void registerMesh(Mesh& mesh, std::vector<glob::Vertex>& vertex, 
		std::vector<GLuint>& indices, std::vector<glob::Texture>& texture);
	virtual void setTexture(GLuint textureID);
	void bindEverything();
	void unBindEverything();

	// game	
	/* note:
	this is not render logic but we chose to store this here for time limitations
	*/
	glm::mat4 transform;
	glm::mat4 prevTransform;
	glob::Direction desiredDirection;
	glob::Direction direction;
	glob::Direction prevDirection;
	bool crashed;
	std::vector<std::array<glm::vec3, 8>> hitboxes;
	bool checkCollision(std::array<glm::vec3, 8> targetHitbox, glm::mat4 targetTransform);
	int checkTeleportCollision(std::array<glm::vec3, 8> targetHitbox, glm::mat4 targetTransform);
	virtual std::array<glm::vec3, 8> createHitbox(std::vector<glob::Vertex>& vertices);
protected:
	GLuint shaderProgram;
	GLuint vao;
	GLuint vbo;
	GLuint ebo;
	GLuint textureID;
	GLuint uniView;
	GLuint uniProj;
	GLuint uniTrans;

	std::shared_ptr<Camera> camera;

	// draw data
	unsigned int indexCount;
	unsigned int vertexCount;
	
	
};