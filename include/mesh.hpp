#pragma once

#include "GL/glew.h"
#include "glm.hpp"
#include "globals.hpp"

#include <vector>

/*
	The Mesh class is tasked with containing mesh data and supplying
	a safe interface to accesing and changing data in vao/vbo
*/

// TODO: remove?




class Mesh {
public:
	Mesh();
	Mesh(std::vector<glob::Vertex> vertex);
	void update(float deltaTime, glm::mat4 transformChange);
	//virtual glm::mat4 getTransform();
	virtual unsigned int getindexCount();
	glm::mat4 transform;
	
private:
	unsigned int indexCount;
	std::vector<GLfloat> hitbox;
	
	std::vector<glob::Texture> texture;
	//MeshData meshData;
};